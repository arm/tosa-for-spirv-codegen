//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Module.hpp>
#include <algorithm>
#include <iostream>
#include <TosaForSpirvCodegen.hpp>

#include <functional>
#include <memory>
#include <ostream>
#include <unordered_set>

namespace tfsc::spirv
{
class Module;
}

namespace tfsc
{
using namespace spv;
using namespace spirv;

std::shared_ptr<Module> CreateModule(TOSAVersion version)
{
    auto module = std::make_unique<Module>();

    module->EmplaceInstruction(OpCapability, {Operand{CapabilityVulkanMemoryModel}});
    module->EmplaceInstruction(OpCapability, {Operand{CapabilityShader}});
    module->EmplaceInstruction(OpCapability, {Operand{CapabilityInt8}});
    module->EmplaceInstruction(OpCapability, {Operand{CapabilityInt16}});
    module->EmplaceInstruction(OpCapability, {Operand{CapabilityInt64}});
    module->EmplaceInstruction(OpCapability, {Operand{CapabilityFloat16}});
    module->EmplaceInstruction(OpCapability, {Operand{CapabilityGraphARM}});
    module->EmplaceInstruction(OpCapability, {Operand{CapabilityTensorsARM}});
    module->EmplaceInstruction(OpCapability, {Operand{CapabilityReplicatedCompositesEXT}});

    module->EmplaceInstruction(OpExtension, {Operand{"SPV_ARM_graph"}});
    module->EmplaceInstruction(OpExtension, {Operand{"SPV_ARM_tensors"}});
    module->EmplaceInstruction(OpExtension, {Operand{"SPV_EXT_replicated_composites"}});

    const std::string versionStr = "TOSA.001000.1";
    module->EmplaceInstruction(OpExtInstImport, {RESID, Operand{versionStr}});

    module->EmplaceInstruction(OpMemoryModel, {Operand(AddressingModelLogical), Operand(MemoryModelVulkan)});

    return module;
}

constexpr int32_t GetMinorVersion();
unsigned int GetResId(const Instruction& instruction);
void WriteOperand(const Operand& operand, std::vector<uint32_t>& binary);
void WriteInstruction(const Instruction& instruction, std::vector<uint32_t>& binary);
void WriteInstructionsRecursive(const Instruction* instruction,
                                std::vector<uint32_t>& binary,
                                std::set<unsigned int>& visitedInstructions);

std::vector<uint32_t> WriteToBinary(const std::shared_ptr<Module>& module)
{
    std::vector<uint32_t> binary;
    const auto spirv = module->GetSpirvGraph();
    std::set<unsigned int> visitedInstructions;

    auto writeInstructionsOfType = [&](const Op op) {
        const auto [begin, end] = module->GetInstructionsOfType(op);
        for (auto it = begin; it != end; ++it)
        {
            WriteInstruction(*it, binary);
            visitedInstructions.emplace(GetResId(*it));
        }
    };

    auto writeInstructionsOfTypeRecursive = [&visitedInstructions, &module, &binary](const Op op) {
        const auto [begin, end] = module->GetInstructionsOfType(op);
        for (auto it = begin; it != end; ++it)
        {
            WriteInstructionsRecursive(&(*it), binary, visitedInstructions);
        }
    };

    auto sortThenWriteInstructions =
        [&](const Op opType, const std::function<bool(const Instruction& lhs, const Instruction& rhs)>& comparator) {
            const auto [instructionBegin, instructionEnd] = module->GetInstructionsOfType(opType);
            std::vector<Instruction> instructions{instructionBegin, instructionEnd};
            std::sort(begin(instructions), end(instructions), comparator);
            std::for_each(begin(instructions), end(instructions), [&](const Instruction& inst) {
                WriteInstruction(inst, binary);
                visitedInstructions.emplace(GetResId(inst));
            });
        };

    // Sort a range of instructions by the given comparator and then write them
    auto sortThenWriteInstructionsRecursive =
        [&](const Op opType, const std::function<bool(const Instruction& lhs, const Instruction& rhs)>& comparator) {
            const auto [instructionBegin, instructionEnd] = module->GetInstructionsOfType(opType);
            std::vector<Instruction> instructions{instructionBegin, instructionEnd};
            std::sort(begin(instructions), end(instructions), comparator);
            std::for_each(begin(instructions), end(instructions), [&](const Instruction& inst) {
                WriteInstructionsRecursive(&inst, binary, visitedInstructions);
                visitedInstructions.emplace(GetResId(inst));
            });
        };

    const auto sortByResId = [](const Instruction& lhs, const Instruction& rhs) {
        return GetResId(lhs) < GetResId(rhs);
    };

    binary.push_back(MagicNumber);
    const uint32_t version = (1 << 16) | (GetMinorVersion() << 8);
    binary.push_back(version);

    // Use the ARM vendor ID, which is 5.
    // This is defined in SPIRV-Headers/include/spirv/spir-v.xml
    constexpr uint32_t vendor = 5 << 16;
    binary.push_back(vendor);

    const uint32_t bound = spirv.size();
    binary.push_back(bound);
    // Output the schema (reserved for use and must be 0)
    binary.push_back(0);

    writeInstructionsOfType(OpCapability);
    writeInstructionsOfType(OpExtension);
    writeInstructionsOfType(OpExtInstImport);
    writeInstructionsOfType(OpMemoryModel);

    sortThenWriteInstructions(OpDecorate, [](const Instruction& lhs, const Instruction& rhs) {
        return GetResId(*lhs.m_Operands[0].m_InstructionPtr) < GetResId(*rhs.m_Operands[0].m_InstructionPtr);
    });

    sortThenWriteInstructionsRecursive(OpVariable, sortByResId);
    writeInstructionsOfTypeRecursive(OpGraphConstantARM);

    const auto [inputBegin, inputEnd] = module->GetInstructionsOfType(OpGraphInputARM);
    // Sort input operators by their ResIds
    std::vector<Instruction> inputInstructions{inputBegin, inputEnd};
    std::sort(begin(inputInstructions), end(inputInstructions), [](const Instruction& lhs, const Instruction& rhs) {
        return lhs.m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord <
               rhs.m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord;
    });
    // Resolve all dependencies of the inputs
    for (const auto& input : inputInstructions)
    {
        for (const Operand& operand : input.m_Operands)
        {
            if (operand.m_Type == INSTRUCTION_POINTER)
            {
                WriteInstructionsRecursive(operand.m_InstructionPtr, binary, visitedInstructions);
            }
        }
    }

    // Resolve all dependencies of the tosa operators
    const auto [tosaBegin, tosaEnd] = module->GetInstructionsOfType(OpExtInst);
    // Sort tosa operators by their ResIds
    std::vector<Instruction> TOSAInstructions{tosaBegin, tosaEnd};
    std::sort(begin(TOSAInstructions), end(TOSAInstructions), [](const Instruction& lhs, const Instruction& rhs) {
        return lhs.m_Operands[1].m_LiteralWord < rhs.m_Operands[1].m_LiteralWord;
    });
    for (const auto& TOSAInstruction : TOSAInstructions)
    {
        for (const Operand& operand : TOSAInstruction.m_Operands)
        {
            if (operand.m_Type == INSTRUCTION_POINTER && operand.m_InstructionPtr->m_Opcode != OpExtInst &&
                operand.m_InstructionPtr->m_Opcode != OpCompositeExtract &&
                operand.m_InstructionPtr->m_Opcode != OpGraphInputARM)
            {
                WriteInstructionsRecursive(operand.m_InstructionPtr, binary, visitedInstructions);
            }
        }
    }

    const auto [outputBegin, outputEnd] = module->GetInstructionsOfType(OpGraphSetOutputARM);
    // Sort output operators by their ResIds
    std::vector<Instruction> outputInstructions{outputBegin, outputEnd};
    std::sort(begin(outputInstructions), end(outputInstructions), [](const Instruction& lhs, const Instruction& rhs) {
        return lhs.m_Operands[1].m_InstructionPtr->m_Operands[2].m_LiteralWord <
               rhs.m_Operands[1].m_InstructionPtr->m_Operands[2].m_LiteralWord;
    });
    // Resolve all dependencies of outputs
    for (const auto& outputInstruction : outputInstructions)
    {
        for (const Operand& operand : outputInstruction.m_Operands)
        {
            if (operand.m_Type == INSTRUCTION_POINTER && operand.m_InstructionPtr->m_Opcode != OpExtInst &&
                operand.m_InstructionPtr->m_Opcode != OpCompositeExtract &&
                operand.m_InstructionPtr->m_Opcode != OpGraphSetOutputARM &&
                operand.m_InstructionPtr->m_Opcode != OpGraphInputARM)
            {
                WriteInstructionsRecursive(operand.m_InstructionPtr, binary, visitedInstructions);
            }
        }
    }

    writeInstructionsOfType(OpTypeGraphARM);
    writeInstructionsOfType(OpGraphEntryPointARM);
    writeInstructionsOfType(OpGraphARM);

    std::for_each(inputInstructions.begin(), inputInstructions.end(), [&](const Instruction& inst) {
        WriteInstruction(inst, binary);
        visitedInstructions.emplace(GetResId(inst));
    });

    std::for_each(TOSAInstructions.begin(), TOSAInstructions.end(), [&](const Instruction& inst) {
        WriteInstruction(inst, binary);
        visitedInstructions.emplace(GetResId(inst));
    });

    writeInstructionsOfType(OpCompositeExtract);
    std::for_each(outputInstructions.begin(), outputInstructions.end(), [&](const Instruction& inst) {
        WriteInstruction(inst, binary);
        visitedInstructions.emplace(GetResId(inst));
    });

    writeInstructionsOfType(OpGraphEndARM);

    for (const Instruction& ins : spirv)
    {
        const auto resId = GetResId(ins);
        if (visitedInstructions.find(resId) == visitedInstructions.end())
        {
            std::cout << "Instruction { OPCODE : " << ins.m_Opcode << " ,RESID : " << resId << " } not written\n";
        }
    }
    return binary;
}

void WriteInstruction(const Instruction& instruction, std::vector<uint32_t>& binary)
{
    // High 16 bit : Word Count
    // Low 16 bit  : Opcode
    uint32_t word = instruction.GetOpCode();
    word |= instruction.m_WordCount << 16;
    binary.push_back(word);
    for (const Operand& operand : instruction.m_Operands)
    {
        WriteOperand(operand, binary);
    }
}

void WriteOperand(const Operand& operand, std::vector<uint32_t>& binary)
{
    switch (operand.m_Type)
    {
        case LITERAL_STRING:
        {
            std::basic_string<char>* str = operand.m_LiteralStr;
            std::string s = *str;

            const char* data = str->c_str();
            const size_t numWords = str->size() / 4;

            for (unsigned Idx = 0; Idx < numWords; Idx++)
            {
                binary.push_back(*reinterpret_cast<const uint32_t*>(&data[4 * Idx]));
            }

            const uint32_t remainder = str->size() % 4;
            uint32_t lastWord = 0;
            if (remainder)
            {
                for (unsigned Idx = 0; Idx < remainder; Idx++)
                {
                    // This takes the remaining bytes that don't take up a full word.
                    // Then combines it using << 8 to place them into an uint32_t.
                    lastWord |= data[4 * numWords + Idx] << 8 * Idx;
                }
            }

            binary.push_back(lastWord);
            break;
        }
        case RES_ID:
        case LITERAL_WORD:
        {
            binary.push_back(operand.m_LiteralWord);
            break;
        }
        case INSTRUCTION_POINTER:
        {
            for (const Operand& instructionOperand : operand.m_InstructionPtr->m_Operands)
            {
                if (instructionOperand.m_Type == RES_ID)
                {
                    WriteOperand(instructionOperand, binary);
                    break;
                }
            }
        }
        default:
        {
            break;
        }
    }
}

void WriteInstructionsRecursive(const Instruction* instruction,
                                std::vector<uint32_t>& binary,
                                std::set<unsigned int>& visitedInstructions)
{
    auto resId = GetResId(*instruction);
    if (visitedInstructions.find(resId) != visitedInstructions.end())
    {
        return;
    }
    visitedInstructions.emplace(resId);
    for (const Operand& operand : instruction->m_Operands)
    {
        if (operand.m_Type == INSTRUCTION_POINTER)
        {
            WriteInstructionsRecursive(operand.m_InstructionPtr, binary, visitedInstructions);
        }
    }
    WriteInstruction(*instruction, binary);
}

unsigned int GetResId(const Instruction& instruction)
{
    for (const auto& operand : instruction.m_Operands)
    {
        if (operand.m_Type == RES_ID)
        {
            return operand.m_LiteralWord;
        }
    }
    return 0;
}

constexpr int32_t GetMinorVersion()
{
    switch (Version)
    {
        case 0x00010000: return 0; break;
        case 0x00010100: return 1; break;
        case 0x00010200: return 2; break;
        case 0x00010300: return 3; break;
        case 0x00010400: return 4; break;
        case 0x00010500: return 5; break;
        case 0x00010600: return 6; break;
        default:
            std::cerr << "WriteHeader(): Unsupported SPIR-V version." << std::endl;
            return -1;
            break;
    }
}
} // namespace tfsc
