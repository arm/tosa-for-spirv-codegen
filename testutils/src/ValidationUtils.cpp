//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <ValidationUtils.hpp>

using namespace tosa2spirv;

void CheckDataType(const spirv::Instruction* instr, tosa::DataType expectedDataType)
{
    if (instr->GetOpCode() != spv::OpTypeInt && instr->GetOpCode() != spv::OpTypeFloat &&
        instr->GetOpCode() != spv::OpTypeBool)
    {
        std::ostringstream oss;
        oss << "CheckDataType expected OpTypeInt/Float/Bool, but got: " << instr->GetOpCode();
        throw std::invalid_argument(oss.str());
    }

    bool isValid = false;

    unsigned int width = 0;
    unsigned int qualifier = 0;

    if (instr->GetOpCode() == spv::OpTypeInt)
    {
        width = instr->m_Operands[1].m_LiteralWord;
        qualifier = instr->m_Operands[2].m_LiteralWord;
    }
    else if (instr->GetOpCode() == spv::OpTypeFloat)
    {
        width = instr->m_Operands[1].m_LiteralWord;
    }

    // Spirv OpCode Validation
    switch (instr->GetOpCode())
    {
        case spv::OpTypeInt: isValid = (qualifier == 0) || (width >= 8 && width <= 32); break;
        case spv::OpTypeFloat: isValid = width >= 16 && width <= 32; break;
        case spv::OpTypeBool: isValid = true; break;
        default: isValid = false; break;
    }

    if (!isValid)
    {
        std::ostringstream oss;
        oss << "Invalid DataType Encoding: opcode=" << instr->GetOpCode() << ", width=" << width
            << ", qualifier=" << qualifier;
        throw std::invalid_argument(oss.str());
    }

    if (expectedDataType == tosa::DataType::null_t)
    {
        // Dont care about the specific type if null_t
        return;
    }

    const spirv::Instruction* typeInstr = instr;
    if (instr->m_Operands[0].m_Type == spirv::INSTRUCTION_POINTER)
    {
        typeInstr = instr->m_Operands[0].m_InstructionPtr;
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
            << "  Got Op:   " << static_cast<int>(typeInstr->m_Opcode)
            << ", width=" << typeInstr->m_Operands[1].m_LiteralWord
            << ", qualifier=" << typeInstr->m_Operands[2].m_LiteralWord;
        throw std::invalid_argument(oss.str());
    }
}

void CheckResID(const spirv::Operand& operand)
{
    if (operand.m_Type != spirv::RES_ID || (operand.m_Type == spirv::RES_ID && operand.m_LiteralWord == 0))
    {
        std::ostringstream oss;
        oss << "Invalid ResID: Type=" << operand.m_Type << ", Value=" << operand.m_LiteralWord;
        throw std::invalid_argument(oss.str());
    }
}

void CheckConstant(const spirv::Instruction* instruction,
                   const tosa::DataType& expectedDataType,
                   std::optional<uint32_t> expectedValue)
{
    if (!instruction || instruction->m_Operands.size() < 3)
    {
        throw std::invalid_argument("Invalid instruction passed to CheckConstant");
    }

    if (instruction->GetOpCode() != spv::OpConstant ||
        instruction->m_Operands[0].m_Type != spirv::INSTRUCTION_POINTER ||
        instruction->m_Operands[1].m_Type != spirv::RES_ID || instruction->m_Operands[2].m_Type != spirv::LITERAL_WORD)
    {
        std::ostringstream oss;
        oss << "Invalid Constant Data: OpCode=" << instruction->GetOpCode()
            << ", Operand[0] m_Type=" << instruction->m_Operands[0].m_Type
            << ", Operand[1] m_Type=" << instruction->m_Operands[1].m_Type
            << ", Operand[2] m_Type=" << instruction->m_Operands[2].m_Type;
        throw std::invalid_argument(oss.str());
    }

    CheckDataType(instruction->m_Operands[0].m_InstructionPtr, expectedDataType);
    CheckResID(instruction->m_Operands[1]);

    if (expectedValue.has_value() && instruction->m_Operands[2].m_LiteralWord != expectedValue.value())
    {
        std::ostringstream oss;
        oss << "Constant value mismatch. Expected: " << expectedValue.value()
            << ", Got: " << instruction->m_Operands[2].m_LiteralWord;
        throw std::invalid_argument(oss.str());
    }
}

void CheckConstantComposite(const spirv::Instruction* instruction,
                            const std::vector<uint32_t>& expectedValues,
                            tosa::DataType expectedType)
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
        CheckDataType(elementType, expectedType);
    }
    else
    {
        CheckDataType(typeOp.m_InstructionPtr, expectedType);
    }

    CheckResID(resId);

    if (!expectedValues.empty() && opcode == spv::OpConstantComposite)
    {
        size_t base = 2;
        for (size_t i = 0; i < expectedValues.size(); ++i)
        {
            if (instruction->m_Operands.size() <= base + i)
            {
                std::ostringstream oss;
                oss << "Missing value in composite constant at index " << i;
                throw std::invalid_argument(oss.str());
            }

            const auto& op = instruction->m_Operands[base + i];

            if (op.m_Type != spirv::INSTRUCTION_POINTER || !op.m_InstructionPtr ||
                op.m_InstructionPtr->m_Opcode != spv::OpConstant)
            {
                std::ostringstream oss;
                oss << "Invalid operand at index " << i << " in composite constant.";
                throw std::invalid_argument(oss.str());
            }

            const uint32_t actual = op.m_InstructionPtr->m_Operands[2].m_LiteralWord;

            if (actual != expectedValues[i])
            {
                std::ostringstream oss;
                oss << "Mismatched value at index " << i << ": expected " << expectedValues[i] << ", got " << actual;
                throw std::invalid_argument(oss.str());
            }
        }
    }
}

void CheckTensorType(const spirv::Instruction* instruction,
                     tosa::DataType expectedDataType,
                     uint32_t expectedRank,
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
    CheckConstantComposite(shapeOp.m_InstructionPtr, expectedShape, tosa::DataType::uint32_t);
}