//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <SpirvParser.hpp>

namespace tosaregen
{
using namespace tfsc;

tosa::DataType GetDataTypeFromInstruction(const spirv::Instruction& typeInstruction)
{
    // Based on SPIRVDefinitions.cpp inverse of the function 'tfsc::spirv::CreateDataType'
    if (typeInstruction.GetOpCode() == spv::Op::OpTypeInt)
    {
        const auto bitWidth = typeInstruction.m_Operands[1].m_LiteralWord;
        switch (bitWidth)
        {
            case 8: return tosa::DataType::uint8_t;
            case 16: return tosa::DataType::uint16_t;
            case 32: return tosa::DataType::uint32_t;
            case 64: return tosa::DataType::int48_t;
            default: throw std::invalid_argument{"GetDataTypeFromInstruction: Invalid OpTypeInt instruction operands"};
        }
    }
    else if (typeInstruction.GetOpCode() == spv::Op::OpTypeFloat)
    {
        const auto bitWidth = typeInstruction.m_Operands[1].m_LiteralWord;
        if (typeInstruction.m_Operands.size() == 3)
        {
            const auto fpEncoding = typeInstruction.m_Operands[2].m_LiteralWord;
            if (bitWidth == 16 && fpEncoding == 0)
            {
                return tosa::DataType::bfloat16_t;
            }
            throw std::invalid_argument{
                "GetDataTypeFromInstruction: Invalid OpTypeFloat encoding instruction operands"};
        }
        switch (bitWidth)
        {
            case 16: return tosa::DataType::float16_t;
            case 32: return tosa::DataType::float32_t;
            default:
                throw std::invalid_argument{
                    "GetDataTypeFromInstruction: Invalid OpTypeFloat bit width instruction operands"};
        }
    }
    else if (typeInstruction.GetOpCode() == spv::Op::OpTypeBool)
    {
        return tosa::DataType::bool_t;
    }
    else
    {
        throw std::invalid_argument{"GetDataTypeFromInstruction: Argument must be type defining instruction"};
    }
}

std::vector<uint32_t> GetTensorShapeFromInstruction(const spirv::Instruction& shapeInstruction)
{
    if (shapeInstruction.GetOpCode() == spv::Op::OpConstantCompositeReplicateEXT)
    {
        const uint32_t value = shapeInstruction.m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord;
        const auto array = shapeInstruction.m_Operands[0].m_InstructionPtr;
        const uint32_t size = array->m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord;
        return std::vector<uint32_t>(size, value);
    }
    else if (shapeInstruction.GetOpCode() == spv::Op::OpConstantComposite)
    {
        std::vector<uint32_t> shape;
        for (size_t idx = 2; idx < shapeInstruction.m_Operands.size(); ++idx)
        {
            const auto constituentInstruction = shapeInstruction.m_Operands[idx].m_InstructionPtr;
            shape.push_back(constituentInstruction->m_Operands[2].m_LiteralWord);
        }
        return shape;
    }
    else
    {
        throw std::invalid_argument{"GetTensorShapeFromInstruction: Invalid shape instruction opcode"};
    }
}

tosa::Tensor GetTensorFromInstruction(const spirv::Instruction& tensorInstruction)
{
    if (tensorInstruction.GetOpCode() != spv::Op::OpTypeTensorARM)
    {
        throw std::invalid_argument{"GetTensorFromInstruction: must supply OpTypeTensorArm instruction"};
    }

    const auto typeInstruction = tensorInstruction.m_Operands[1].m_InstructionPtr;
    const tosa::DataType type = GetDataTypeFromInstruction(*typeInstruction);

    const auto shapeInstruction = tensorInstruction.m_Operands[3].m_InstructionPtr;
    const std::vector<uint32_t> shape = GetTensorShapeFromInstruction(*shapeInstruction);

    return tosa::Tensor{type, shape};
}

tosa::Attribute GetAttributeFromInstruction(const spirv::Instruction& attributeInstruction)
{
    if (attributeInstruction.GetOpCode() == spv::Op::OpConstantTrue ||
        attributeInstruction.GetOpCode() == spv::Op::OpConstantFalse)
    {
        const auto value = (attributeInstruction.GetOpCode() == spv::Op::OpConstantTrue) ? 1 : 0;
        return tosa::Attribute{{value}, tosa::DataType::bool_t, {1}};
    }
    else if (attributeInstruction.GetOpCode() == spv::Op::OpConstant)
    {
        const auto dataType = GetDataTypeFromInstruction(*attributeInstruction.m_Operands[0].m_InstructionPtr);
        std::vector<uint32_t> values;
        values.push_back(attributeInstruction.m_Operands[2].m_LiteralWord);

        if (dataType == tosa::DataType::int48_t)
        {
            values.push_back(attributeInstruction.m_Operands[3].m_LiteralWord);
        }

        return tosa::Attribute{values, dataType, {1}};
    }
    else if (attributeInstruction.GetOpCode() == spv::Op::OpConstantNull ||
             attributeInstruction.GetOpCode() == spv::Op::OpConstantComposite ||
             attributeInstruction.GetOpCode() == spv::Op::OpConstantCompositeReplicateEXT)
    {
        const auto tensor = GetTensorFromInstruction(*attributeInstruction.m_Operands[0].m_InstructionPtr);
        std::vector<uint32_t> data;

        if (attributeInstruction.GetOpCode() == spv::Op::OpConstantNull)
        {
            data.insert(data.end(), static_cast<size_t>(tensor.GetNumElements()), 0);
        }
        else if (attributeInstruction.GetOpCode() == spv::Op::OpConstantCompositeReplicateEXT)
        {
            const auto valueInstruction = attributeInstruction.m_Operands[2].m_InstructionPtr;
            data.insert(data.end(),
                        static_cast<size_t>(tensor.GetNumElements()),
                        valueInstruction->m_Operands[2].m_LiteralWord);
        }
        else
        {
            for (size_t idx = 2; idx < attributeInstruction.m_Operands.size(); ++idx)
            {
                const auto valueInstruction = attributeInstruction.m_Operands[idx].m_InstructionPtr;
                data.push_back(valueInstruction->m_Operands[2].m_LiteralWord);
                if (tensor.GetDataType() == tosa::DataType::int48_t)
                {
                    data.push_back(valueInstruction->m_Operands[3].m_LiteralWord);
                }
            }
        }

        return tosa::Attribute{data, tensor.GetDataType(), tensor.GetTensorShape()};
    }
    else
    {
        std::string errText =
            "GetAttributeFromInstruction: unsupported opcode: " + OpToString(attributeInstruction.GetOpCode());
        throw std::invalid_argument{errText};
    }
}

TosaOperator GetTosaOperator(const spirv::Instruction& instruction)
{
    if (instruction.GetOpCode() != spv::Op::OpExtInst)
    {
        throw std::invalid_argument{"GetTosaOperator: Invalid TOSA Operator Instruction: incorrect opcode"};
    }
    if (instruction.m_Operands.size() < 4)
    {
        throw std::invalid_argument{"GetTosaOperator: Invalid TOSA Operator Instruction: incorrect operand count"};
    }

    TosaOperator tosaOp;

    tosaOp.op = GetOperatorEnum(static_cast<TOSAInstructions>(instruction.m_Operands[3].m_LiteralWord));

    const tosa::OperatorDefinition& tosaOpDefinition = tosa::GetOperatorDefinition(tosaOp.op);

    // Processing output
    if (instruction.m_Operands[0].m_Type != spirv::INSTRUCTION_POINTER)
    {
        throw std::invalid_argument{"GetTosaOperator: Invalid output tensor in Instruction operands"};
    }
    const auto outputInstruction = instruction.m_Operands[0].m_InstructionPtr;
    if (outputInstruction->GetOpCode() == spv::Op::OpTypeStruct)
    {
        for (size_t idx = 1; idx < outputInstruction->m_Operands.size(); ++idx)
        {
            const auto& outputTensor = GetTensorFromInstruction(*outputInstruction->m_Operands[idx].m_InstructionPtr);
            tosaOp.outputs.push_back(
                TosaOutput{outputTensor, instruction.m_Operands[1].m_LiteralWord, static_cast<uint8_t>(idx - 1)});
        }
    }
    else
    {
        const auto& outputTensor = GetTensorFromInstruction(*outputInstruction);
        tosaOp.outputs.push_back(TosaOutput{outputTensor, instruction.m_Operands[1].m_LiteralWord, 0});
    }
    //  Checking outputs are correct
    if (tosaOp.outputs.size() != tosaOpDefinition.m_OutputSize)
    {
        std::string errText = "GetTosaOperator: Invalid output size for operator ";
        errText += tosa::GetOperatorName(tosaOp.op);
        throw std::invalid_argument{errText};
    }

    // Processing input and attributes
    if ((instruction.m_Operands.size() - 4) < (tosaOpDefinition.m_InputSize + tosaOpDefinition.m_AttributeSize))
    {
        throw std::invalid_argument{"GetTosaOperator: Invalid input/attribute operand count"};
    }
    size_t lastAttributeIdx = 0;
    for (size_t idx = 4; idx < instruction.m_Operands.size(); ++idx)
    {
        const auto currentInst = instruction.m_Operands[idx].m_InstructionPtr;

        if (lastAttributeIdx < tosaOpDefinition.m_AttributeSize)
        {
            try
            {
                tosaOp.attributes.push_back(GetAttributeFromInstruction(*currentInst));
            }
            catch (std::invalid_argument& error)
            {
                std::string errText = "GetTosaOperator: Operand parsing error: ";
                errText += error.what();
                throw std::invalid_argument{errText};
            }
            ++lastAttributeIdx;
        }
        else
        {
            if (currentInst->GetOpCode() == spv::Op::OpGraphInputARM ||
                currentInst->GetOpCode() == spv::Op::OpExtInst ||
                currentInst->GetOpCode() == spv::Op::OpCompositeExtract)
            {
                // OpGraphInputARM Input Tensor format: OpTypeTensorARM, RES_ID, OpConstant (input index)
                const auto& inputTensor = GetTensorFromInstruction(*currentInst->m_Operands[0].m_InstructionPtr);
                uint32_t tensorId = currentInst->m_Operands[1].m_LiteralWord;
                uint8_t tensorIdx = 0;
                if (currentInst->GetOpCode() == spv::Op::OpCompositeExtract)
                {
                    tensorId = currentInst->m_Operands[2].m_InstructionPtr->m_Operands[1].m_LiteralWord;
                    tensorIdx = currentInst->m_Operands[3].m_LiteralWord;
                }
                uint32_t bindingId = 0;
                if (currentInst->GetOpCode() == spv::Op::OpGraphInputARM)
                {
                    bindingId = currentInst->m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord;
                }
                tosaOp.inputs.push_back(TosaInput{inputTensor, TosaInputType::Dynamic, tensorId, tensorIdx, bindingId});
            }
            else if (currentInst->GetOpCode() == spv::Op::OpGraphConstantARM)
            {
                const auto& graphConstTensor = GetTensorFromInstruction(*currentInst->m_Operands[0].m_InstructionPtr);
                tosaOp.inputs.push_back(TosaInput{graphConstTensor,
                                                  TosaInputType::GraphConstant,
                                                  currentInst->m_Operands[1].m_LiteralWord,
                                                  0,
                                                  currentInst->m_Operands[2].m_LiteralWord});
            }
            else
            {
                try
                {
                    const auto& tensorConstAttribute = GetAttributeFromInstruction(*currentInst);
                    tosaOp.inputs.push_back(TosaInput{tensorConstAttribute,
                                                      TosaInputType::TensorConstant,
                                                      currentInst->m_Operands[1].m_LiteralWord,
                                                      0});
                }
                catch (std::invalid_argument& error)
                {
                    std::string errText = "GetTosaOperator: Tensor Constant parsing error:";
                    errText += error.what();
                    throw std::invalid_argument{errText};
                }
            }
        }
    }
    return tosaOp;
}

} // namespace tosaregen
