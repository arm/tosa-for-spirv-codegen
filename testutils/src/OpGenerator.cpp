//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <OpGenerator.hpp>
#include <stdexcept>

#include "EnumMaps.hpp"

namespace testutils
{
using namespace tosa2spirv;

tosa::DataType GetDataTypeFromInstruction(const spirv::Instruction& typeInstruction)
{
    // Based on SPIRVDefinitions.cpp inverse of the function 'tosa2spirv::spirv::CreateDataType'
    if (typeInstruction.GetOpCode() == spv::Op::OpTypeInt)
    {
        const auto bitWidth = typeInstruction.m_Operands[1].m_LiteralWord;
        switch (bitWidth)
        {
            case 8: return tosa::DataType::uint8_t;
            case 16: return tosa::DataType::uint16_t;
            case 32: return tosa::DataType::uint32_t;
            case 64: return tosa::DataType::int48_t;
            default: throw std::invalid_argument{"GetDataTypeFromInstruction: Invalid type instruction operands"};
        }
    }
    else if (typeInstruction.GetOpCode() == spv::Op::OpTypeFloat)
    {
        const auto bitWidth = typeInstruction.m_Operands[1].m_LiteralWord;
        if (typeInstruction.m_Operands.size() == 3)
        {
            const auto fpEncoding = typeInstruction.m_Operands[2].m_LiteralWord;
            if (bitWidth == 16 && fpEncoding == 2)
            {
                return tosa::DataType::bfloat16_t;
            }
            throw std::invalid_argument{"GetDataTypeFromInstruction: Invalid float type instruction operands"};
        }
        switch (bitWidth)
        {
            case 16: return tosa::DataType::float16_t;
            case 32: return tosa::DataType::float32_t;
            default: throw std::invalid_argument{"GetDataTypeFromInstruction: Invalid type instruction operands"};
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
        return std::vector<uint32_t>{size, value};
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
            data.insert(data.end(), static_cast<size_t>(tensor.GetTensorShape()[0]), 0);
        }
        else if (attributeInstruction.GetOpCode() == spv::Op::OpConstantCompositeReplicateEXT)
        {
            const auto valueInstruction = attributeInstruction.m_Operands[2].m_InstructionPtr;
            data.insert(data.end(),
                        static_cast<size_t>(tensor.GetTensorShape()[0]),
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

std::string GetTensorString(tosa::Tensor& tensor)
{
    std::string result;
    result += "{DataType::" + GetDataTypeName(tensor.GetDataType()) + ", {";
    for (size_t dimIdx = 0; dimIdx < tensor.GetTensorShape().size(); ++dimIdx)
    {
        const auto dim = tensor.GetTensorShape()[dimIdx];
        result += std::to_string(dim);
        if (dimIdx < tensor.GetTensorShape().size() - 1)
            result += ", ";
    }
    result += "}}";
    return result;
}

std::string GetAttributeString(tosa::Attribute& attribute)
{
    std::string result;
    result += "{{";
    // Print data initializer list
    for (size_t dataIdx = 0; dataIdx < attribute.GetData().size(); ++dataIdx)
    {
        const auto value = attribute.GetData()[dataIdx];
        result += std::to_string(value);
        if (dataIdx < attribute.GetData().size() - 1)
            result += ", ";
    }
    // Print data type
    result += "}, DataType::" + GetDataTypeName(attribute.GetTensor().GetDataType()) + ", {";
    // Print shape initializer list
    for (size_t dimIdx = 0; dimIdx < attribute.GetTensor().GetTensorShape().size(); ++dimIdx)
    {
        const auto dim = attribute.GetTensor().GetTensorShape()[dimIdx];
        result += std::to_string(dim);
        if (dimIdx < attribute.GetTensor().GetTensorShape().size() - 1)
            result += ", ";
    }
    result += "}}";
    return result;
}

template <typename T> bool CompareVectors(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    std::vector<T> comparator;
    for (const T& element : lhs)
    {
        comparator.push_back(element);
    }
    for (const T& element : rhs)
    {
        for (auto iterator = comparator.begin(); iterator != comparator.end(); ++iterator)
        {
            if (*iterator == element)
            {
                comparator.erase(iterator);
                break;
            }
        }
    }

    return comparator.empty();
}

bool operator==(const TosaOperator& lhs, const TosaOperator& rhs)
{
    return (lhs.op == rhs.op && CompareVectors(lhs.inputs, rhs.inputs) && CompareVectors(lhs.outputs, rhs.outputs) &&
            CompareVectors(lhs.attributes, rhs.attributes));
}
bool operator!=(const TosaOperator& lhs, const TosaOperator& rhs) { return !(lhs == rhs); }

bool defaultOpComparator(const TosaOperator& lhs, const TosaOperator& rhs)
{
    // Checking if opcodes are different
    if (lhs.op != rhs.op)
    {
        return lhs.op < rhs.op;
    }
    // Checking if operand sizes are different
    if (lhs.inputs.size() != rhs.inputs.size())
    {
        return lhs.inputs.size() < rhs.inputs.size();
    }
    if (lhs.outputs.size() != rhs.outputs.size())
    {
        return lhs.outputs.size() < rhs.outputs.size();
    }
    if (lhs.attributes.size() != rhs.attributes.size())
    {
        return lhs.attributes.size() < rhs.attributes.size();
    }
    // Checking if operands are different
    for (size_t idx = 0; idx < lhs.inputs.size(); ++idx)
    {
        if (lhs.inputs[idx].GetNumElements() != rhs.inputs[idx].GetNumElements())
            return lhs.inputs[idx].GetNumElements() < rhs.inputs[idx].GetNumElements();
    }
    for (size_t idx = 0; idx < lhs.outputs.size(); ++idx)
    {
        if (lhs.outputs[idx].GetNumElements() != rhs.outputs[idx].GetNumElements())
            return lhs.outputs[idx].GetNumElements() < rhs.outputs[idx].GetNumElements();
    }
    for (size_t idx = 0; idx < lhs.attributes.size(); ++idx)
    {
        if (lhs.attributes[idx].GetTensor().GetNumElements() != rhs.attributes[idx].GetTensor().GetNumElements())
            return lhs.attributes[idx].GetTensor().GetNumElements() < rhs.attributes[idx].GetTensor().GetNumElements();
    }
    return false;
};

std::vector<std::string> Spirv2tosa(const std::shared_ptr<spirv::Module>& module, OperatorComparator filter)
{
    std::vector<std::string> result;
    auto operators = Spirv2operators(module, filter);

    for (auto op : operators)
    {
        std::string opString = "const OperatorEnum op = OperatorEnum::" + GetEnumName(op.op) + ";\n";
        opString += "const std::vector<Tensor> inputs {";
        for (size_t inputIdx = 0; inputIdx < op.inputs.size(); ++inputIdx)
        {
            auto input = op.inputs[inputIdx];
            opString += GetTensorString(input);
            if (inputIdx < op.inputs.size() - 1)
                opString += ", ";
        }
        opString += "};\nconst std::vector<Tensor> outputs {";
        for (size_t outputIdx = 0; outputIdx < op.outputs.size(); ++outputIdx)
        {
            auto output = op.outputs[outputIdx];
            opString += GetTensorString(output);
            if (outputIdx < op.outputs.size() - 1)
                opString += ", ";
        }
        opString += "};\nconst std::vector<Attribute> attributes {";
        for (size_t attributeIdx = 0; attributeIdx < op.attributes.size(); ++attributeIdx)
        {
            auto attribute = op.attributes[attributeIdx];
            opString += GetAttributeString(attribute);
            if (attributeIdx < op.attributes.size() - 1)
                opString += ", ";
        }
        opString += "};";
        result.push_back(opString);
    }
    return result;
}

std::vector<TosaOperator> Spirv2operators(const std::shared_ptr<spirv::Module>& module, OperatorComparator filter)
{
    auto operandInstructions = module->GetInstructionsOfType(spv::Op::OpExtInst);
    std::vector<spirv::Instruction> instructions{operandInstructions.first, operandInstructions.second};

    std::set<TosaOperator, OperatorComparator> operators{filter};
    for (auto instruction : instructions)
    {
        operators.insert(GetTosaOperator(instruction));
    }

    std::vector<TosaOperator> result{operators.begin(), operators.end()};
    return result;
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
            tosaOp.outputs.push_back(GetTensorFromInstruction(*outputInstruction->m_Operands[idx].m_InstructionPtr));
        }
    }
    else
    {
        tosaOp.outputs.push_back(GetTensorFromInstruction(*outputInstruction));
    }

    // Processing input and attributes
    for (size_t idx = 4; idx < instruction.m_Operands.size(); ++idx)
    {
        const auto currentInst = instruction.m_Operands[idx].m_InstructionPtr;
        if (currentInst->GetOpCode() == spv::Op::OpGraphInputARM ||
            currentInst->GetOpCode() == spv::Op::OpGraphConstantARM || currentInst->GetOpCode() == spv::Op::OpExtInst)
        {
            // OpGraphInputARM Input Tensor format: OpTypeTensorARM, RES_ID, OpConstant (input index)
            tosaOp.inputs.push_back(GetTensorFromInstruction(*currentInst->m_Operands[0].m_InstructionPtr));
        }
        else
        {
            try
            {
                tosaOp.attributes.push_back(GetAttributeFromInstruction(*currentInst));
            }
            catch (std::invalid_argument error)
            {
                std::string errText = "GetTosaOperator: Operand parsing error: ";
                errText += error.what();
                throw std::invalid_argument{errText};
            }
        }
    }
    return tosaOp;
}

} // namespace testutils
