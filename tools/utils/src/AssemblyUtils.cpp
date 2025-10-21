//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "EnumMaps.hpp"
#include <AssemblyUtils.hpp>
#include <cstring>

#include <spirv-tools/libspirv.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace testutils
{

std::string DisassembleSPIRV(const std::vector<uint32_t>& binary, bool runValidation, bool friendlyNames)
{
    spvtools::SpirvTools tools{SPV_ENV_UNIVERSAL_1_6};

    if (!tools.IsValid())
    {
        std::cerr << "DisassembleSPIRV(): Failed to validate SPIR-V tools." << std::endl;
        return {};
    }

    std::string humanReadableSpirV;

    static const spvtools::MessageConsumer messageConsumer =
        [](spv_message_level_t level, const char*, const spv_position_t& pos, const char* message) {
            std::string levelStr;
            switch (level)
            {
                case SPV_MSG_FATAL: levelStr = "FATAL"; break;
                case SPV_MSG_INTERNAL_ERROR: levelStr = "INTERNAL ERROR"; break;
                case SPV_MSG_ERROR: levelStr = "ERROR"; break;
                case SPV_MSG_WARNING: levelStr = "WARNING"; break;
                case SPV_MSG_INFO: levelStr = "INFO"; break;
                case SPV_MSG_DEBUG: levelStr = "DEBUG"; break;
            }
            std::cout << "spvtools reported " << message << "(" << levelStr.c_str() << ") at position: " << pos.index
                      << std::endl;
        };
    tools.SetMessageConsumer(messageConsumer);

    if (runValidation && !tools.Validate(binary))
    {
        std::cerr << "DisassembleSPIRV():Failed to validate SPIR-V program." << std::endl;
        return {};
    }
    uint32_t options = SPV_BINARY_TO_TEXT_OPTION_INDENT | SPV_BINARY_TO_TEXT_OPTION_NO_HEADER;
    if (friendlyNames)
    {
        options |= SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES;
    }
    // Return SPIR-V in human-readable form with indentation and friendly names
    // for types.
    if (!tools.Disassemble(binary, &humanReadableSpirV, options))
    {
        // Exception
        std::cerr << "DisassembleSPIRV():Failed to disassemble SPIR-V program." << std::endl;
        return {};
    }

    return humanReadableSpirV;
}

using tfsc::spirv::Module;
using tfsc::spirv::Operand;
using ResIdMap = std::unordered_map<std::string, Operand>;

std::vector<Operand> ParseOperand(const std::string& token, ResIdMap& resIdMap)
{
    if (token.empty())
    {
        throw std::invalid_argument("Empty operand token");
    }

    if (const auto it = resIdMap.find(token); it != resIdMap.end())
    {
        return {it->second};
    }

    // If the token starts with a double quote, it is treated as a string literal.
    if (token.front() == '"')
    {
        if (token.back() != '"')
        {
            throw std::invalid_argument("Mismatched quotes in operand: " + token);
        }
        const std::string inner = token.substr(1, token.size() - 2);
        return {Operand(inner)};
    }

    // Keep only chars that can appear in a numeric literal.
    auto isNumericChar = [](unsigned char c) {
        return static_cast<bool>(std::isdigit(c)) || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E';
    };

    std::string numToken = token;
    while (!numToken.empty() && !isNumericChar(static_cast<unsigned char>(numToken.back())))
    {
        numToken.pop_back();
    }
    if (numToken.empty())
    {
        throw std::invalid_argument("Invalid terminal operand: " + token);
    }

    // If it looks like a float (has '.' or exponent), parse as float32 and pack bits.
    if (numToken.find('.') != std::string::npos || numToken.find('e') != std::string::npos ||
        numToken.find('E') != std::string::npos)
    {
        try
        {
            const float f = std::stof(numToken); // single-precision only
            static_assert(sizeof(float) == 4, "Float is not 32-bit on this platform");
            uint32_t bits = 0;
            std::memcpy(&bits, &f, sizeof(bits)); // preserve IEEE-754 bit pattern
            return {Operand(bits)};
        }
        catch (const std::out_of_range&)
        {
            throw std::invalid_argument("Invalid terminal operand (float32 out of range): " + token);
        }
        catch (const std::exception&)
        {
            throw std::invalid_argument("Invalid terminal operand (float32 parse): " + token);
        }
    }

    // treat as int
    try
    {
        const uint64_t value = std::stoull(numToken);
        const auto low = static_cast<uint32_t>(value);
        const auto high = static_cast<uint32_t>(value >> 32);
        if (high == 0)
        {
            return {Operand(low)};
        }
        else
        {
            return {Operand{high}, Operand{low}};
        }
    }
    catch (const std::out_of_range&)
    {
        throw std::invalid_argument("Invalid terminal operand (out of range): " + token);
    }
    catch (const std::exception&)
    {
        throw std::invalid_argument("Invalid terminal operand: " + token);
    }
}

std::vector<Operand> ParseTosaInstruction(std::istringstream& line, ResIdMap& resIdMap)
{
    std::string token;
    std::vector<Operand> operands;
    line >> token;
    operands.push_back(resIdMap.at(token));

    line >> token;
    operands.push_back(resIdMap.at(token));

    line >> token;
    const auto tosaOp = GetTosaOpEnum(token);
    operands.emplace_back(static_cast<unsigned int>(tosaOp));

    while (line >> token)
    {
        const auto vals = ParseOperand(token, resIdMap);
        operands.insert(operands.end(), vals.begin(), vals.end());
    }

    return operands;
}

std::vector<Operand> ParseOp(spv::Op op, std::istringstream& line, ResIdMap& resIdMap)
{
    std::string token;
    std::vector<Operand> operands;

    // ops requiring custom handling
    switch (op)
    {
        case spv::Op::OpExtInst:
        {
            return ParseTosaInstruction(line, resIdMap);
        }
        case spv::Op::OpDecorate:
        {
            line >> token;
            auto vals = ParseOperand(token, resIdMap);
            operands.insert(operands.end(), vals.begin(), vals.end());
            line >> token;
            if (token == "DescriptorSet")
            {
                operands.emplace_back(spv::DecorationDescriptorSet);
            }
            else if (token == "Binding")
            {
                operands.emplace_back(spv::DecorationBinding);
            }
            line >> token;
            vals = ParseOperand(token, resIdMap);
            operands.insert(operands.end(), vals.begin(), vals.end());
            return operands;
        }
        case spv::Op::OpMemoryModel:
        {
            return {Operand(spv::AddressingModelLogical), Operand(spv::MemoryModelVulkan)};
        }
        case spv::Op::OpCapability:
        {
            line >> token;
            const spv::Capability capability = GetCapabilityEnum(token);
            return {Operand{static_cast<unsigned int>(capability)}};
        }
        case spv::Op::OpTypePointer:
        {
            // skip first operand
            operands.emplace_back(spv::StorageClassUniformConstant);
            line >> token;
            line >> token;
            const auto vals = ParseOperand(token, resIdMap);
            operands.insert(operands.end(), vals.begin(), vals.end());
            return operands;
        }
        case spv::Op::OpVariable:
        {
            line >> token;
            const auto vals = ParseOperand(token, resIdMap);
            operands.insert(operands.end(), vals.begin(), vals.end());
            operands.emplace_back(spv::StorageClassUniformConstant);
            return operands;
        }
        default:
        {
        }
    }

    while (line >> token)
    {
        const auto vals = ParseOperand(token, resIdMap);
        operands.insert(operands.end(), vals.begin(), vals.end());
    }

    return operands;
}

void ParseSpirvTextImpl(const std::string& text, Module& module, ResIdMap& resIdMap, bool firstPass)
{
    std::istringstream input(text);
    std::string line;
    int lineNumber = 0;

    while (std::getline(input, line))
    {
        ++lineNumber;
        std::istringstream iss(line);
        std::string token;
        spv::Op opcode = spv::Op::OpNop;
        std::vector<Operand> operands;

        std::string resId;

        while (iss >> token)
        {
            if (resId.empty() && token.find('%') == 0)
            {
                resId = token;
                continue;
            }
            // Skip tokens until we find one starting with "Op"
            if (token.find("Op") == 0)
            {
                try
                {
                    opcode = GetOpEnum(token);
                    if (firstPass && (opcode == spv::Op::OpDecorate || opcode == spv::Op::OpGraphEntryPointARM ||
                                      opcode == spv::Op::OpName))
                    {
                        // These ops may not have their ResIds resolved at this point, so we
                        // wait for the second pass
                        break;
                    }
                    operands = ParseOp(opcode, iss, resIdMap);
                }
                catch (const std::invalid_argument& ex)
                {
                    std::cout << "Line " << lineNumber << ": " << ex.what() << std::endl;
                    break;
                }
            }
        }
        if (firstPass && (opcode == spv::Op::OpDecorate || opcode == spv::Op::OpGraphEntryPointARM) ||
            opcode == spv::Op::OpNop)
        {
            continue;
        }

        if (!resId.empty())
        {
            // ResId position needs to be specified
            unsigned int pos = GetResultIdPosition(opcode);
            operands.insert(operands.begin() + pos, tfsc::spirv::RESID);
        }

        Operand op;
        if (firstPass && opcode == spv::Op::OpVariable)
        {
            op = module.EmplaceInstructionNonUnique(opcode, operands);
        }
        else
        {
            op = module.EmplaceInstruction(opcode, operands);
        }

        if (!resId.empty())
            resIdMap.insert({resId, op});
    }
}

std::shared_ptr<Module> LoadSPIRVDisassembly(const std::string& text)
{
    auto module = std::make_shared<Module>();
    ResIdMap resIdMap;

    ParseSpirvTextImpl(text, *module, resIdMap, true);
    ParseSpirvTextImpl(text, *module, resIdMap, false);

    return module;
}
} // namespace testutils
