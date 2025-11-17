//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <ValidationUtils.hpp>

using namespace tfsc;

void CheckDataType(const spirv::Instruction* instr, tosa::DataType expectedDataType)
{
    std::ostringstream oss;
    const auto& operands = instr->m_Operands;
    switch (instr->GetOpCode())
    {
        case (spv::OpTypeFloat):
        {
            if (operands.size() != 3 && operands.size() != 2)
            {
                oss << "CheckDataType OpTypeFloat, has invalid operand size of: " << operands.size();
                break;
            }
            break;
        }
        case (spv::OpTypeInt):
        {
            if (operands.size() != 3)
            {
                oss << "CheckDataType OpTypeInt, has invalid operand size of: " << operands.size();
            }
            break;
        }
        case (spv::OpTypeBool):
        {
            if (operands.size() != 1)
            {
                oss << "CheckDataType OpTypeBool, has invalid operand size of: " << operands.size();
            }
            break;
        }
        default: oss << "CheckDataType expected OpTypeInt/Float/Bool, but got: " << instr->GetOpCode();
    }

    if (!oss.str().empty())
    {
        throw std::invalid_argument(oss.str());
    }

    const auto typeCheck = [&](const spirv::Instruction* instruction) {
        switch (expectedDataType)
        {
            case tosa::DataType::int4_t:
            case tosa::DataType::int8_t:
            case tosa::DataType::uint8_t:
                return instruction->m_Opcode == spv::OpTypeInt && instruction->m_Operands[1].m_LiteralWord == 8 &&
                       instruction->m_Operands[2].m_LiteralWord == 0;
            case tosa::DataType::int16_t:
            case tosa::DataType::uint16_t:
                return instruction->m_Opcode == spv::OpTypeInt && instruction->m_Operands[1].m_LiteralWord == 16 &&
                       instruction->m_Operands[2].m_LiteralWord == 0;
            case tosa::DataType::int32_t:
            case tosa::DataType::uint32_t:
                return instruction->m_Opcode == spv::OpTypeInt && instruction->m_Operands[1].m_LiteralWord == 32 &&
                       instruction->m_Operands[2].m_LiteralWord == 0;
            case tosa::DataType::int48_t:
                return instruction->m_Opcode == spv::OpTypeInt && instruction->m_Operands[1].m_LiteralWord == 64 &&
                       instruction->m_Operands[2].m_LiteralWord == 0;
            case tosa::DataType::float16_t:
                return instruction->m_Opcode == spv::OpTypeFloat && instruction->m_Operands[1].m_LiteralWord == 16;
            case tosa::DataType::float32_t:
                return instruction->m_Opcode == spv::OpTypeFloat && instruction->m_Operands[1].m_LiteralWord == 32;
            case tosa::DataType::bfloat16_t:
                return instruction->m_Opcode == spv::OpTypeFloat && instruction->m_Operands[1].m_LiteralWord == 16;
            case tosa::DataType::bool_t: return instruction->m_Opcode == spv::OpTypeBool;
            default: throw std::invalid_argument("Unhandled DataType in validation");
        }
    };

    if (!typeCheck(instr))
    {
        std::ostringstream oss;
        oss << "Expected type does not match actual SPIR-V type.\n"
            << "  Expected: " << static_cast<int>(expectedDataType) << "\n"
            << "  Got Op:   " << static_cast<int>(instr->m_Opcode);
        if (operands.size() == 2)
            oss << ", width=" << instr->m_Operands[1].m_LiteralWord;
        if (operands.size() == 3)
            oss << ", qualifier=" << instr->m_Operands[2].m_LiteralWord;
        throw std::invalid_argument(oss.str());
    }
}

void CheckResID(const spirv::Operand& operand)
{
    if (operand.m_Type != spirv::RES_ID || operand.m_LiteralWord == 0)
    {
        std::ostringstream oss;
        oss << "Invalid ResID: Type=" << operand.m_Type << ", Value=" << operand.m_LiteralWord;
        throw std::invalid_argument(oss.str());
    }
}

void CheckConstant(const spirv::Instruction* instruction,
                   const tosa::DataType& expectedDataType,
                   std::optional<uint32_t> expectedValue0,
                   std::optional<uint32_t> expectedValue1)
{
    if (!instruction)
    {
        throw std::invalid_argument("Invalid instruction passed to CheckConstant");
    }

    const auto opcode = instruction->GetOpCode();
    const bool isBool = (expectedDataType == tosa::DataType::bool_t);
    const bool isInt48 = (expectedDataType == tosa::DataType::int48_t);

    // ---- Operand count checks by opcode/type ----
    if (opcode == spv::OpConstantTrue || opcode == spv::OpConstantFalse)
    {
        // Boolean constants: ResultType, ResultId (no literal words)
        if (!isBool)
        {
            std::ostringstream oss;
            oss << "Boolean constant with non-boolean expected type. OpCode=" << opcode;
            throw std::invalid_argument(oss.str());
        }
        if (instruction->m_Operands.size() != 2u)
        {
            throw std::invalid_argument("Bool Constant must have 2 operands");
        }
    }
    else if (opcode == spv::OpConstant)
    {
        // Integer constants (including int48_t)
        const std::size_t expectedOperands = isInt48 ? 4u : 3u;
        if (instruction->m_Operands.size() != expectedOperands)
        {
            if (!isInt48 && instruction->m_Operands.size() != 3u)
            {
                std::cout << "instruction->GetOpCode(): " << opcode;
                throw std::invalid_argument("Int DataType must have 3 operands");
            }
            if (isInt48 && instruction->m_Operands.size() != 4u)
            {
                throw std::invalid_argument("Int48 DataType must have 4 operands");
            }
        }
    }
    else
    {
        std::ostringstream oss;
        oss << "Unsupported opcode in CheckConstant: " << opcode;
        throw std::invalid_argument(oss.str());
    }

    // ---- Operand type checks ----
    // Common first two operands: ResultType, ResultId
    if (instruction->m_Operands[0].m_Type != spirv::INSTRUCTION_POINTER ||
        instruction->m_Operands[1].m_Type != spirv::RES_ID)
    {
        std::ostringstream oss;
        oss << "Invalid Constant Data (common operands). OpCode=" << opcode
            << ", Operand[0] m_Type=" << instruction->m_Operands[0].m_Type
            << ", Operand[1] m_Type=" << instruction->m_Operands[1].m_Type;
        throw std::invalid_argument(oss.str());
    }

    if (opcode == spv::OpConstant)
    {
        if (instruction->m_Operands[2].m_Type != spirv::LITERAL_WORD ||
            (isInt48 && instruction->m_Operands[3].m_Type != spirv::LITERAL_WORD))
        {
            std::ostringstream oss;
            oss << "Invalid Constant Data (literal operands). OpCode=" << opcode
                << ", Operand[2] m_Type=" << instruction->m_Operands[2].m_Type;
            if (isInt48)
                oss << ", Operand[3] m_Type=" << instruction->m_Operands[3].m_Type;
            throw std::invalid_argument(oss.str());
        }
    }

    // ---- Semantic checks shared by all constants ----
    CheckDataType(instruction->m_Operands[0].m_InstructionPtr, expectedDataType);
    CheckResID(instruction->m_Operands[1]);

    // ---- Value checks ----
    if (opcode == spv::OpConstant)
    {
        if (expectedValue0.has_value() && instruction->m_Operands[2].m_LiteralWord != expectedValue0.value())
        {
            std::ostringstream oss;
            oss << "Constant value mismatch (word0). Expected: " << expectedValue0.value()
                << ", Got: " << instruction->m_Operands[2].m_LiteralWord;
            throw std::invalid_argument(oss.str());
        }

        // NOTE: fixed bug — check the second literal word (index 3) for int48_t.
        if (isInt48 && expectedValue1.has_value() && instruction->m_Operands[3].m_LiteralWord != expectedValue1.value())
        {
            std::ostringstream oss;
            oss << "Constant value mismatch (word1). Expected: " << expectedValue1.value()
                << ", Got: " << instruction->m_Operands[3].m_LiteralWord;
            throw std::invalid_argument(oss.str());
        }
    }
    else
    {
        // Boolean constants; allow expectedValue0/1 (0=false, nonzero=true)
        const bool actual = (opcode == spv::OpConstantTrue);

        if (expectedValue0.has_value())
        {
            const bool expected = (expectedValue0.value() != 0u);
            if (expected != actual)
            {
                std::ostringstream oss;
                oss << "Constant bool mismatch. Expected: " << (expected ? "true" : "false")
                    << ", Got: " << (actual ? "true" : "false");
                throw std::invalid_argument(oss.str());
            }
        }
        if (expectedValue1.has_value())
        {
            const bool expected = (expectedValue1.value() != 0u);
            if (expected != actual)
            {
                std::ostringstream oss;
                oss << "Constant bool mismatch (expectedValue1). Expected: " << (expected ? "true" : "false")
                    << ", Got: " << (actual ? "true" : "false");
                throw std::invalid_argument(oss.str());
            }
        }
    }
}

void CheckConstantComposite(const spirv::Instruction* instruction,
                            const std::vector<uint32_t>& expectedValues,
                            tosa::DataType expectedConstantType,
                            tosa::DataType expectedCompositeType)
{
    const auto opcode = instruction->GetOpCode();
    if (opcode != spv::OpConstantComposite && opcode != spv::OpConstantCompositeReplicateEXT &&
        opcode != spv::OpConstantNull)
    {
        std::ostringstream oss;
        oss << "Unexpected opcode for constant composite: " << opcode;
        throw std::invalid_argument(oss.str());
    }

    const auto& typeOp = instruction->m_Operands[0];
    const auto& resId = instruction->m_Operands[1];

    if (typeOp.m_Type != spirv::INSTRUCTION_POINTER || !typeOp.m_InstructionPtr)
    {
        throw std::invalid_argument("Invalid or missing type in composite constant.");
    }

    const spirv::Instruction* tensorTypeInstr = typeOp.m_InstructionPtr;

    if ((tensorTypeInstr->GetOpCode() == spv::OpTypeTensorARM || tensorTypeInstr->GetOpCode() == spv::OpTypeArray) &&
        tensorTypeInstr->m_Operands[1].m_Type == spirv::INSTRUCTION_POINTER)
    {
        const auto* elementType = tensorTypeInstr->m_Operands[1].m_InstructionPtr;
        CheckDataType(elementType, expectedConstantType);
    }
    else
    {
        CheckDataType(typeOp.m_InstructionPtr, expectedCompositeType);
    }

    CheckResID(resId);

    if (expectedValues.empty() || opcode != spv::OpConstantComposite)
    {
        return;
    }
    const size_t base = 2;
    const unsigned int constantSize = expectedCompositeType == tosa::DataType::int48_t ? 2 : 1;
    for (size_t i = 0; i < expectedValues.size(); i += constantSize)
    {
        if (instruction->m_Operands.size() <= base + i)
        {
            std::ostringstream oss;
            oss << "Missing value in composite constant at index " << i;
            throw std::invalid_argument(oss.str());
        }

        const auto constantOp = instruction->m_Operands[base + i];

        if (constantOp.m_Type != spirv::INSTRUCTION_POINTER || !constantOp.m_InstructionPtr)
        {
            std::ostringstream oss;
            oss << "Invalid operand at index " << i << " in composite constant.";
            throw std::invalid_argument(oss.str());
        }

        CheckConstant(constantOp.m_InstructionPtr, expectedConstantType, expectedValues[i]);
    }
}

void CheckTensorType(const spirv::Instruction* instruction,
                     const tosa::DataType expectedDataType,
                     const uint32_t expectedRank,
                     const std::vector<uint32_t>& expectedShape)
{
    if (!instruction || instruction->GetOpCode() != spv::OpTypeTensorARM)
    {
        throw std::invalid_argument("Invalid instruction in CheckTensorType: Expected OpTypeTensorARM.");
    }

    const auto& dataTypeOp = instruction->m_Operands[1];
    const auto& rankOp = instruction->m_Operands[2];
    const auto& shapeOp = instruction->m_Operands[3];

    // DataType check
    if (dataTypeOp.m_Type != spirv::INSTRUCTION_POINTER || !dataTypeOp.m_InstructionPtr)
    {
        throw std::invalid_argument("Invalid data type operand in tensor: not an instruction pointer.");
    }

    CheckDataType(dataTypeOp.m_InstructionPtr, expectedDataType);

    // Rank check
    if (rankOp.m_Type != spirv::INSTRUCTION_POINTER || !rankOp.m_InstructionPtr)
    {
        throw std::invalid_argument("Invalid rank operand in tensor: not an instruction pointer.");
    }

    const auto* rankInstr = rankOp.m_InstructionPtr;

    if (rankInstr->GetOpCode() != spv::OpConstant || rankInstr->m_Operands.size() < 3)
    {
        throw std::invalid_argument("Rank operand is not a valid OpConstant.");
    }

    const auto rankValue = rankInstr->m_Operands[2].m_LiteralWord;
    if (rankValue != expectedRank)
    {
        std::ostringstream oss;
        oss << "Tensor rank mismatch. Expected: " << expectedRank << ", Got: " << rankValue;
        throw std::invalid_argument(oss.str());
    }
    // Shape check
    if (shapeOp.m_Type != spirv::INSTRUCTION_POINTER || !shapeOp.m_InstructionPtr)
    {
        throw std::invalid_argument("Invalid shape operand in tensor: not an instruction pointer.");
    }
    // Check constant composite inside shape
    CheckConstantComposite(shapeOp.m_InstructionPtr, expectedShape, tosa::DataType::uint32_t, expectedDataType);
}
