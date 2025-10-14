//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <TosaOperator.hpp>

namespace tosaregen
{
using namespace tfsc;

bool operator<(const tosa::Tensor& lhs, const tosa::Tensor& rhs)
{
    if (lhs.GetDataType() != rhs.GetDataType())
    {
        return lhs.GetDataType() < rhs.GetDataType();
    }
    if (lhs.GetNumElements() != rhs.GetNumElements())
    {
        return lhs.GetNumElements() < rhs.GetNumElements();
    }
    if (lhs.GetTensorShape().size() != rhs.GetTensorShape().size())
    {
        return lhs.GetTensorShape().size() < rhs.GetTensorShape().size();
    }
    for (size_t idx = 0; idx < lhs.GetTensorShape().size(); ++idx)
    {
        if (lhs.GetTensorShape()[idx] != rhs.GetTensorShape()[idx])
        {
            return lhs.GetTensorShape()[idx] < rhs.GetTensorShape()[idx];
        }
    }
    return false;
}

bool operator<(const tosa::Attribute& lhs, const tosa::Attribute& rhs)
{
    if (lhs.GetTensor() != rhs.GetTensor())
    {
        return lhs.GetTensor() < rhs.GetTensor();
    }
    if (lhs.GetData().size() != rhs.GetData().size())
    {
        return lhs.GetData().size() < rhs.GetData().size();
    }
    for (size_t idx = 0; idx < lhs.GetData().size(); ++idx)
    {
        if (lhs.GetData()[idx] != rhs.GetData()[idx])
        {
            return lhs.GetData()[idx] < rhs.GetData()[idx];
        }
    }
    if (lhs.GetResId() != rhs.GetResId())
    {
        return lhs.GetResId() < rhs.GetResId();
    }
    return false;
}

std::string GetTensorString(const tosa::Tensor& tensor)
{
    std::string result;
    result += "{DataType::" + GetDataTypeName(tensor.GetDataType()) + ", {";
    for (size_t dimIdx = 0; dimIdx < tensor.GetTensorShape().size(); ++dimIdx)
    {
        const auto dim = tensor.GetTensorShape()[dimIdx];
        result += std::to_string(dim);
        if (dimIdx < tensor.GetTensorShape().size() - 1)
        {
            result += ", ";
        }
    }
    result += "}}";
    return result;
}

std::string GetAttributeString(const tosa::Attribute& attribute)
{
    std::string result;
    result += "{std::initializer_list<uint32_t>{";
    // Print data initializer list
    for (size_t dataIdx = 0; dataIdx < attribute.GetData().size(); ++dataIdx)
    {
        const auto value = attribute.GetData()[dataIdx];
        result += std::to_string(value);
        if (dataIdx < attribute.GetData().size() - 1)
        {
            result += ", ";
        }
    }
    // Print data type
    result += "}, DataType::" + GetDataTypeName(attribute.GetTensor().GetDataType()) + ", {";
    // Print shape initializer list
    for (size_t dimIdx = 0; dimIdx < attribute.GetTensor().GetTensorShape().size(); ++dimIdx)
    {
        const auto dim = attribute.GetTensor().GetTensorShape()[dimIdx];
        result += std::to_string(dim);
        if (dimIdx < attribute.GetTensor().GetTensorShape().size() - 1)
        {
            result += ", ";
        }
    }
    result += "}}";
    return result;
}

bool operator==(const TosaOperator& lhs, const TosaOperator& rhs)
{
    return lhs.op == rhs.op && lhs.inputs == rhs.inputs && lhs.outputs == rhs.outputs &&
           lhs.attributes == rhs.attributes;
}
bool operator!=(const TosaOperator& lhs, const TosaOperator& rhs) { return !(lhs == rhs); }

bool CompareTosaOperatorData(const TosaOperator& lhs, const TosaOperator& rhs)
{
    if (lhs.op != rhs.op)
    {
        return false;
    }
    if (lhs.inputs.size() != rhs.inputs.size() || lhs.outputs.size() != rhs.outputs.size() ||
        lhs.attributes.size() != rhs.attributes.size())
    {
        return false;
    }
    for (size_t idx = 0; idx < lhs.inputs.size(); ++idx)
    {
        const auto& lhsInput = lhs.inputs[idx];
        const auto& rhsInput = rhs.inputs[idx];
        if (lhsInput.GetAttribute() != rhsInput.GetAttribute() || lhsInput.type != rhsInput.type ||
            lhsInput.bindingId != rhsInput.bindingId)
        {
            return false;
        }
    }
    for (size_t idx = 0; idx < lhs.outputs.size(); ++idx)
    {
        if (lhs.outputs[idx].GetTensor() != rhs.outputs[idx].GetTensor())
        {
            return false;
        }
    }
    for (size_t idx = 0; idx < lhs.attributes.size(); ++idx)
    {
        if (lhs.attributes[idx] != rhs.attributes[idx])
        {
            return false;
        }
    }
    return true;
}

bool operator<(const TosaInput& lhs, const TosaInput& rhs)
{
    if (lhs.GetAttribute() != rhs.GetAttribute())
    {
        return lhs.GetAttribute() < rhs.GetAttribute();
    }
    if (lhs.type != rhs.type)
    {
        return lhs.type < rhs.type;
    }
    if (lhs.id != rhs.id)
    {
        return lhs.id < rhs.id;
    }
    if (lhs.idx != rhs.idx)
    {
        return lhs.idx < rhs.idx;
    }
    if (lhs.bindingId != rhs.bindingId)
    {
        return lhs.bindingId < rhs.bindingId;
    }
    return false;
}
bool operator<=(const TosaInput& lhs, const TosaInput& rhs) { return (lhs < rhs) || (lhs == rhs); }
bool operator>(const TosaInput& lhs, const TosaInput& rhs) { return !(lhs <= rhs); }
bool operator>=(const TosaInput& lhs, const TosaInput& rhs) { return (lhs > rhs) || (lhs == rhs); }
bool operator==(const TosaInput& lhs, const TosaInput& rhs) { return !(lhs < rhs) && !(rhs < lhs); }
bool operator!=(const TosaInput& lhs, const TosaInput& rhs) { return !(lhs == rhs); }

bool operator<(const TosaOutput& lhs, const TosaOutput& rhs)
{
    if (lhs.GetTensor() != rhs.GetTensor())
    {
        return lhs.GetTensor() < rhs.GetTensor();
    }
    if (lhs.id != rhs.id)
    {
        return lhs.id < rhs.id;
    }
    if (lhs.idx != rhs.idx)
    {
        return lhs.idx < rhs.idx;
    }
    return false;
}
bool operator<=(const TosaOutput& lhs, const TosaOutput& rhs) { return (lhs < rhs) || (lhs == rhs); }
bool operator>(const TosaOutput& lhs, const TosaOutput& rhs) { return !(lhs <= rhs); }
bool operator>=(const TosaOutput& lhs, const TosaOutput& rhs) { return (lhs > rhs) || (lhs == rhs); }
bool operator==(const TosaOutput& lhs, const TosaOutput& rhs) { return !(lhs < rhs) && !(rhs < lhs); }
bool operator!=(const TosaOutput& lhs, const TosaOutput& rhs) { return !(lhs == rhs); }

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
        if (lhs.inputs[idx] != rhs.inputs[idx])
        {
            return lhs.inputs[idx] < rhs.inputs[idx];
        }
    }
    for (size_t idx = 0; idx < lhs.outputs.size(); ++idx)
    {
        if (lhs.outputs[idx] != rhs.outputs[idx])
        {
            return lhs.outputs[idx] < rhs.outputs[idx];
        }
    }
    for (size_t idx = 0; idx < lhs.attributes.size(); ++idx)
    {
        if (lhs.attributes[idx] != rhs.attributes[idx])
        {
            return lhs.attributes[idx] != rhs.attributes[idx];
        }
    }
    return false;
};

std::string OperatorToString(const TosaOperator& op)
{
    std::string opString = "const OperatorEnum op = OperatorEnum::" + GetOperatorName(op.op) + ";\n";
    opString += "const std::vector<Attribute> inputs {";
    for (size_t inputIdx = 0; inputIdx < op.inputs.size(); ++inputIdx)
    {
        const auto& input = op.inputs[inputIdx];
        opString += GetAttributeString(input.GetAttribute());
        if (inputIdx < op.inputs.size() - 1)
        {
            opString += ", ";
        }
    }
    opString += "};\nconst std::vector<Tensor> outputs {";
    for (size_t outputIdx = 0; outputIdx < op.outputs.size(); ++outputIdx)
    {
        const auto& output = op.outputs[outputIdx];
        opString += GetTensorString(output.GetTensor());
        if (outputIdx < op.outputs.size() - 1)
        {
            opString += ", ";
        }
    }
    opString += "};\nconst std::vector<Attribute> attributes {";
    for (size_t attributeIdx = 0; attributeIdx < op.attributes.size(); ++attributeIdx)
    {
        const auto& attribute = op.attributes[attributeIdx];
        opString += GetAttributeString(attribute);
        if (attributeIdx < op.attributes.size() - 1)
        {
            opString += ", ";
        }
    }
    opString += "};";
    return opString;
}

std::string OperatorToGraphDefinition(const TosaOperator& op,
                                      std::string graphVarName,
                                      std::string inputsName,
                                      std::string outputsName,
                                      std::string attributesName)
{
    std::string operatorLine =
        "const auto& graphRes = " + graphVarName + ".Add" + tosa::GetOperatorName(op.op) + "Operator(";

    std::string opString;
    if (op.op == tosa::OperatorEnum::Concat)
    {
        operatorLine += "{";
    }
    // Adding input tensors
    for (size_t inputIdx = 0; inputIdx < op.inputs.size(); ++inputIdx)
    {
        // Adding tensor to graph
        opString += "const auto& input" + std::to_string(inputIdx + 1) + " = " + graphVarName;
        // Using separate add function based on tensor type
        switch (op.inputs[inputIdx].type)
        {
            case TosaInputType::Dynamic: opString += ".AddInput("; break;
            case TosaInputType::GraphConstant: opString += ".AddGraphConstant("; break;
            case TosaInputType::TensorConstant: opString += ".AddTensorConstant("; break;
        }
        if (inputsName != "")
        {
            opString += inputsName + "[" + std::to_string(inputIdx) + "]";
            if (op.inputs[inputIdx].type != TosaInputType::TensorConstant)
            {
                opString += ".GetTensor()";
            }
        }
        else
        {
            if (op.inputs[inputIdx].type == TosaInputType::TensorConstant)
            {
                opString += "Attribute" + GetAttributeString(op.inputs[inputIdx].GetAttribute());
            }
            else
            {
                opString += "Tensor" + GetTensorString(op.inputs[inputIdx].GetTensor());
            }
        }
        if (op.inputs[inputIdx].type == TosaInputType::Dynamic)
        {
            opString += ", " + std::to_string(inputIdx);
        }
        opString += ");\n";
        // Adding tensor name to Add*Operator
        operatorLine += "input" + std::to_string(inputIdx + 1);
        if (op.op == tosa::OperatorEnum::Concat && inputIdx + 1 == op.inputs.size())
        {
            operatorLine += "}";
        }
        operatorLine += ", ";
    }
    if (op.inputs.size() > 0)
    {
        opString += "\n";
    }

    // Adding attributes
    for (size_t attributeIdx = 0; attributeIdx < op.attributes.size(); ++attributeIdx)
    {
        // Creating attribute variable
        opString += "const auto& attribute" + std::to_string(attributeIdx + 1) + " = ";
        if (attributesName != "")
        {
            opString += attributesName + "[" + std::to_string(attributeIdx) + "];\n";
        }
        else
        {
            opString += "Attribute" + GetAttributeString(op.attributes[attributeIdx]) + ";\n";
        }
        // Adding attribute name to Add*Operator
        operatorLine += "attribute" + std::to_string(attributeIdx + 1) + ", ";
    }
    if (op.attributes.size() > 0)
    {
        opString += "\n";
    }

    // Adding outputs to the graph
    for (size_t outputIdx = 0; outputIdx < op.outputs.size(); ++outputIdx)
    {
        // Creating output variable
        opString += "const auto& output" + std::to_string(outputIdx + 1) + " = ";
        if (outputsName != "")
        {
            opString += outputsName + "[" + std::to_string(outputIdx) + "];\n";
        }
        else
        {
            opString += "Tensor" + GetTensorString(op.outputs[outputIdx].GetTensor()) + ";\n";
        }
        // Adding output name to Add*Operator
        operatorLine += "output" + std::to_string(outputIdx + 1);
        if ((outputIdx + 1) != op.outputs.size())
        {
            operatorLine += ", ";
        }
    }
    opString += operatorLine + ");\n";

    // Binding output tensors to graph outputs
    if (op.outputs.size() > 1)
    {
        for (size_t idx = 0; idx < op.outputs.size(); ++idx)
        {
            opString +=
                graphVarName + ".AddOutput(graphRes[" + std::to_string(idx) + "], " + std::to_string(idx) + ");\n";
        }
    }
    else
    {
        opString += graphVarName + ".AddOutput(graphRes, 0);\n";
    }

    opString += graphVarName + ".FinalizeGraph();";
    return opString;
}

} // namespace tosaregen
