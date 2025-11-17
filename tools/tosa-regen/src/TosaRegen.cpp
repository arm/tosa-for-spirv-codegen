//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <TosaRegen.hpp>

namespace tosaregen
{
using namespace tfsc;
using namespace ::tosa;

DType GetTosaSerializationType(tfsc::tosa::DataType type)
{
    switch (type)
    {
        case tfsc::tosa::DataType::int4_t: return DType::DType_INT4;
        case tfsc::tosa::DataType::uint8_t:
        case tfsc::tosa::DataType::int8_t: return DType::DType_INT8;
        case tfsc::tosa::DataType::uint16_t:
        case tfsc::tosa::DataType::int16_t: return DType::DType_INT16;
        case tfsc::tosa::DataType::uint32_t:
        case tfsc::tosa::DataType::int32_t: return DType::DType_INT32;
        case tfsc::tosa::DataType::int48_t: return DType::DType_INT48;
        case tfsc::tosa::DataType::float16_t: return DType::DType_FP16;
        case tfsc::tosa::DataType::float32_t: return DType::DType_FP32;
        case tfsc::tosa::DataType::bfloat16_t: return DType::DType_BF16;
        case tfsc::tosa::DataType::bool_t: return DType::DType_BOOL;

        // Unsupported types
        case tfsc::tosa::DataType::int64_t:
        case tfsc::tosa::DataType::null_t:
        default: return DType::DType_UNKNOWN;
    }
}

DType GetTosaAccType(uint32_t idx)
{
    switch (idx)
    {
        case 1: return DType::DType_INT32;
        case 2: return DType::DType_FP16;
        case 3: return DType::DType_FP32;
        case 4: return DType::DType_INT48;
    }
    return DType::DType_UNKNOWN;
}

std::unique_ptr<TosaAttributeBase> GetTosaAttribute(tfsc::tosa::OperatorEnum op,
                                                    std::vector<tfsc::tosa::Attribute> attributes)
{
    switch (op)
    {
            // GET TOSA ATTRIBUTE CODE GENERATION BEGIN
        case tfsc::tosa::OperatorEnum::ArgMax:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            NanPropagationMode nan_mode = static_cast<NanPropagationMode>(attributes[1].GetData()[0]);
            return std::make_unique<TosaArgMaxAttribute>(axis, nan_mode);
        }
        case tfsc::tosa::OperatorEnum::AvgPool2d:
        {
            std::vector<int32_t> kernel = {attributes[0].GetData().begin(), attributes[0].GetData().end()};
            std::vector<int32_t> stride = {attributes[1].GetData().begin(), attributes[1].GetData().end()};
            std::vector<int32_t> pad = {attributes[2].GetData().begin(), attributes[2].GetData().end()};
            DType acc_type = GetTosaAccType(attributes[3].GetData()[0]);
            return std::make_unique<TosaAvgPool2dAttribute>(kernel, stride, pad, acc_type);
        }
        case tfsc::tosa::OperatorEnum::Conv2d:
        {
            std::vector<int32_t> pad = {attributes[0].GetData().begin(), attributes[0].GetData().end()};
            std::vector<int32_t> stride = {attributes[1].GetData().begin(), attributes[1].GetData().end()};
            std::vector<int32_t> dilation = {attributes[2].GetData().begin(), attributes[2].GetData().end()};
            bool local_bound = static_cast<bool>(attributes[4].GetData()[0]);
            DType acc_type = GetTosaAccType(attributes[3].GetData()[0]);
            return std::make_unique<TosaConv2dAttribute>(pad, stride, dilation, local_bound, acc_type);
        }
        case tfsc::tosa::OperatorEnum::Conv3d:
        {
            std::vector<int32_t> pad = {attributes[0].GetData().begin(), attributes[0].GetData().end()};
            std::vector<int32_t> stride = {attributes[1].GetData().begin(), attributes[1].GetData().end()};
            std::vector<int32_t> dilation = {attributes[2].GetData().begin(), attributes[2].GetData().end()};
            bool local_bound = static_cast<bool>(attributes[4].GetData()[0]);
            DType acc_type = GetTosaAccType(attributes[3].GetData()[0]);
            return std::make_unique<TosaConv3dAttribute>(pad, stride, dilation, local_bound, acc_type);
        }
        case tfsc::tosa::OperatorEnum::DepthwiseConv2d:
        {
            std::vector<int32_t> pad = {attributes[0].GetData().begin(), attributes[0].GetData().end()};
            std::vector<int32_t> stride = {attributes[1].GetData().begin(), attributes[1].GetData().end()};
            std::vector<int32_t> dilation = {attributes[2].GetData().begin(), attributes[2].GetData().end()};
            bool local_bound = static_cast<bool>(attributes[4].GetData()[0]);
            DType acc_type = GetTosaAccType(attributes[3].GetData()[0]);
            return std::make_unique<TosaDepthwiseConv2dAttribute>(pad, stride, dilation, local_bound, acc_type);
        }
        case tfsc::tosa::OperatorEnum::Fft2d:
        {
            bool inverse = static_cast<bool>(attributes[0].GetData()[0]);
            bool local_bound = static_cast<bool>(attributes[1].GetData()[0]);
            return std::make_unique<TosaFFT2dAttribute>(inverse, local_bound);
        }
        case tfsc::tosa::OperatorEnum::Matmul: return std::make_unique<::tosa::TosaMatMulAttribute>();
        case tfsc::tosa::OperatorEnum::MaxPool2d:
        {
            std::vector<int32_t> kernel = {attributes[0].GetData().begin(), attributes[0].GetData().end()};
            std::vector<int32_t> stride = {attributes[1].GetData().begin(), attributes[1].GetData().end()};
            std::vector<int32_t> pad = {attributes[2].GetData().begin(), attributes[2].GetData().end()};
            NanPropagationMode nan_mode = static_cast<NanPropagationMode>(attributes[3].GetData()[0]);
            return std::make_unique<TosaMaxPool2dAttribute>(kernel, stride, pad, nan_mode);
        }
        case tfsc::tosa::OperatorEnum::Rfft2d:
        {
            bool local_bound = static_cast<bool>(attributes[0].GetData()[0]);
            return std::make_unique<TosaRFFT2dAttribute>(local_bound);
        }
        case tfsc::tosa::OperatorEnum::TransposeConv2d:
        {
            std::vector<int32_t> out_pad = {attributes[0].GetData().begin(), attributes[0].GetData().end()};
            std::vector<int32_t> stride = {attributes[1].GetData().begin(), attributes[1].GetData().end()};
            bool local_bound = static_cast<bool>(attributes[3].GetData()[0]);
            DType acc_type = GetTosaAccType(attributes[2].GetData()[0]);
            return std::make_unique<TosaTransposeConv2dAttribute>(out_pad, stride, local_bound, acc_type);
        }
        case tfsc::tosa::OperatorEnum::Clamp:
        {
            std::vector<uint8_t> min_val = {attributes[0].GetData().begin(), attributes[0].GetData().end()};
            std::vector<uint8_t> max_val = {attributes[1].GetData().begin(), attributes[1].GetData().end()};
            NanPropagationMode nan_mode = static_cast<NanPropagationMode>(attributes[2].GetData()[0]);
            return std::make_unique<TosaClampAttribute>(min_val, max_val, nan_mode);
        }
        case tfsc::tosa::OperatorEnum::Erf: return std::make_unique<::tosa::TosaErfAttribute>();
        case tfsc::tosa::OperatorEnum::Sigmoid: return std::make_unique<::tosa::TosaSigmoidAttribute>();
        case tfsc::tosa::OperatorEnum::Tanh: return std::make_unique<::tosa::TosaTanhAttribute>();
        case tfsc::tosa::OperatorEnum::Add: return std::make_unique<::tosa::TosaAddAttribute>();
        case tfsc::tosa::OperatorEnum::ArithmeticRightShift:
        {
            bool round = static_cast<bool>(attributes[0].GetData()[0]);
            return std::make_unique<TosaArithmeticRightShiftAttribute>(round);
        }
        case tfsc::tosa::OperatorEnum::BitwiseAnd: return std::make_unique<::tosa::TosaBitwiseAndAttribute>();
        case tfsc::tosa::OperatorEnum::BitwiseOr: return std::make_unique<::tosa::TosaBitwiseOrAttribute>();
        case tfsc::tosa::OperatorEnum::BitwiseXor: return std::make_unique<::tosa::TosaBitwiseXorAttribute>();
        case tfsc::tosa::OperatorEnum::IntDiv: return std::make_unique<::tosa::TosaIntDivAttribute>();
        case tfsc::tosa::OperatorEnum::LogicalAnd: return std::make_unique<::tosa::TosaLogicalAndAttribute>();
        case tfsc::tosa::OperatorEnum::LogicalLeftShift:
            return std::make_unique<::tosa::TosaLogicalLeftShiftAttribute>();
        case tfsc::tosa::OperatorEnum::LogicalRightShift:
            return std::make_unique<::tosa::TosaLogicalRightShiftAttribute>();
        case tfsc::tosa::OperatorEnum::LogicalOr: return std::make_unique<::tosa::TosaLogicalOrAttribute>();
        case tfsc::tosa::OperatorEnum::LogicalXor: return std::make_unique<::tosa::TosaLogicalXorAttribute>();
        case tfsc::tosa::OperatorEnum::Maximum:
        {
            NanPropagationMode nan_mode = static_cast<NanPropagationMode>(attributes[0].GetData()[0]);
            return std::make_unique<TosaMaximumAttribute>(nan_mode);
        }
        case tfsc::tosa::OperatorEnum::Minimum:
        {
            NanPropagationMode nan_mode = static_cast<NanPropagationMode>(attributes[0].GetData()[0]);
            return std::make_unique<TosaMinimumAttribute>(nan_mode);
        }
        case tfsc::tosa::OperatorEnum::Mul: return std::make_unique<::tosa::TosaMulAttribute>();
        case tfsc::tosa::OperatorEnum::Pow: return std::make_unique<::tosa::TosaPowAttribute>();
        case tfsc::tosa::OperatorEnum::Sub: return std::make_unique<::tosa::TosaSubAttribute>();
        case tfsc::tosa::OperatorEnum::Table: return std::make_unique<::tosa::TosaTableAttribute>();
        case tfsc::tosa::OperatorEnum::Abs: return std::make_unique<::tosa::TosaAbsAttribute>();
        case tfsc::tosa::OperatorEnum::BitwiseNot: return std::make_unique<::tosa::TosaBitwiseNotAttribute>();
        case tfsc::tosa::OperatorEnum::Ceil: return std::make_unique<::tosa::TosaCeilAttribute>();
        case tfsc::tosa::OperatorEnum::Clz: return std::make_unique<::tosa::TosaClzAttribute>();
        case tfsc::tosa::OperatorEnum::Cos: return std::make_unique<::tosa::TosaCosAttribute>();
        case tfsc::tosa::OperatorEnum::Exp: return std::make_unique<::tosa::TosaExpAttribute>();
        case tfsc::tosa::OperatorEnum::Floor: return std::make_unique<::tosa::TosaFloorAttribute>();
        case tfsc::tosa::OperatorEnum::Log: return std::make_unique<::tosa::TosaLogAttribute>();
        case tfsc::tosa::OperatorEnum::LogicalNot: return std::make_unique<::tosa::TosaLogicalNotAttribute>();
        case tfsc::tosa::OperatorEnum::Negate: return std::make_unique<::tosa::TosaNegateAttribute>();
        case tfsc::tosa::OperatorEnum::Reciprocal: return std::make_unique<::tosa::TosaReciprocalAttribute>();
        case tfsc::tosa::OperatorEnum::Rsqrt: return std::make_unique<::tosa::TosaRsqrtAttribute>();
        case tfsc::tosa::OperatorEnum::Sin: return std::make_unique<::tosa::TosaSinAttribute>();
        case tfsc::tosa::OperatorEnum::Select: return std::make_unique<::tosa::TosaSelectAttribute>();
        case tfsc::tosa::OperatorEnum::Equal: return std::make_unique<::tosa::TosaEqualAttribute>();
        case tfsc::tosa::OperatorEnum::Greater: return std::make_unique<::tosa::TosaGreaterAttribute>();
        case tfsc::tosa::OperatorEnum::GreaterEqual: return std::make_unique<::tosa::TosaGreaterEqualAttribute>();
        case tfsc::tosa::OperatorEnum::ReduceAll:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            return std::make_unique<TosaReduceAllAttribute>(axis);
        }
        case tfsc::tosa::OperatorEnum::ReduceAny:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            return std::make_unique<TosaReduceAnyAttribute>(axis);
        }
        case tfsc::tosa::OperatorEnum::ReduceMax:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            NanPropagationMode nan_mode = static_cast<NanPropagationMode>(attributes[1].GetData()[0]);
            return std::make_unique<TosaReduceMaxAttribute>(axis, nan_mode);
        }
        case tfsc::tosa::OperatorEnum::ReduceMin:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            NanPropagationMode nan_mode = static_cast<NanPropagationMode>(attributes[1].GetData()[0]);
            return std::make_unique<TosaReduceMinAttribute>(axis, nan_mode);
        }
        case tfsc::tosa::OperatorEnum::ReduceProduct:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            return std::make_unique<TosaReduceProductAttribute>(axis);
        }
        case tfsc::tosa::OperatorEnum::ReduceSum:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            return std::make_unique<TosaReduceSumAttribute>(axis);
        }
        case tfsc::tosa::OperatorEnum::Concat:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            return std::make_unique<TosaConcatAttribute>(axis);
        }
        case tfsc::tosa::OperatorEnum::Pad: return std::make_unique<::tosa::TosaPadAttribute>();
        case tfsc::tosa::OperatorEnum::Reshape: return std::make_unique<::tosa::TosaReshapeAttribute>();
        case tfsc::tosa::OperatorEnum::Reverse:
        {
            int32_t axis = static_cast<int32_t>(attributes[0].GetData()[0]);
            return std::make_unique<TosaReverseAttribute>(axis);
        }
        case tfsc::tosa::OperatorEnum::Slice: return std::make_unique<::tosa::TosaSliceAttribute>();
        case tfsc::tosa::OperatorEnum::Tile: return std::make_unique<::tosa::TosaTileAttribute>();
        case tfsc::tosa::OperatorEnum::Transpose:
        {
            std::vector<int32_t> perms = {attributes[0].GetData().begin(), attributes[0].GetData().end()};
            return std::make_unique<TosaTransposeAttribute>(perms);
        }
        case tfsc::tosa::OperatorEnum::Gather: return std::make_unique<::tosa::TosaGatherAttribute>();
        case tfsc::tosa::OperatorEnum::Scatter: return std::make_unique<::tosa::TosaScatterAttribute>();
        case tfsc::tosa::OperatorEnum::Resize:
        {
            ResizeMode mode = static_cast<ResizeMode>(attributes[0].GetData()[0]);
            return std::make_unique<TosaResizeAttribute>(mode);
        }
        case tfsc::tosa::OperatorEnum::Cast: return std::make_unique<::tosa::TosaCastAttribute>();
        case tfsc::tosa::OperatorEnum::Rescale:
        {
            bool scale32 = static_cast<bool>(attributes[0].GetData()[0]);
            RoundingMode rounding_mode = static_cast<RoundingMode>(attributes[1].GetData()[0]);
            bool per_channel = static_cast<bool>(attributes[2].GetData()[0]);
            bool input_unsigned = static_cast<bool>(attributes[3].GetData()[0]);
            bool output_unsigned = static_cast<bool>(attributes[4].GetData()[0]);
            return std::make_unique<TosaRescaleAttribute>(scale32,
                                                          rounding_mode,
                                                          per_channel,
                                                          input_unsigned,
                                                          output_unsigned);
        }
        // GET TOSA ATTRIBUTE CODE GENERATION END
        default: return std::make_unique<TosaNoneAttribute>();
    }
}

std::string GetTosaSerializationTensorName(const uint32_t& tensorId,
                                           const uint8_t& tensorIdx,
                                           const std::map<uint32_t, std::string>& tensorNameMap,
                                           const std::string& tensorNamePrefix)
{
    if (tensorNameMap.find(tensorId) != tensorNameMap.end())
    {
        return tensorNameMap.at(tensorId);
    }
    else
    {
        return (tensorNamePrefix + std::to_string(tensorId) + "_" + std::to_string(tensorIdx));
    }
}

std::unique_ptr<TosaSerializationTensor>
GetTosaSerializationTensor(const tfsc::tosa::Tensor& tensor, const std::string& name, const std::vector<uint32_t>& data)
{
    std::vector<int32_t> shape;
    std::vector<int32_t> dataInt32;
    for (size_t i = 0; i < tensor.GetTensorShape().size(); ++i)
    {
        shape.push_back(static_cast<int32_t>(tensor.GetTensorShape()[i]));
    }
    for (size_t i = 0; i < data.size(); ++i)
    {
        dataInt32.push_back(static_cast<int32_t>(data[i]));
    }

    std::vector<uint8_t> dataUint8;
    TosaSerializationHandler::ConvertI32toU8(dataInt32, dataUint8);

    auto result = std::make_unique<TosaSerializationTensor>(name,
                                                            shape,
                                                            GetTosaSerializationType(tensor.GetDataType()),
                                                            dataUint8);
    return std::move(result);
}

void SortTosaSerializationOperators(std::vector<std::unique_ptr<TosaSerializationOperator>>& operators,
                                    std::vector<std::string>& graphInputNames)
{
    std::unordered_map<size_t, std::set<std::string>> inDegrees;
    std::queue<size_t> readyOperators;

    // First pass to get in-degree information and initialize queue
    for (size_t idx = 0; idx < operators.size(); ++idx)
    {
        std::set<std::string> inputs;
        for (const auto& name : operators[idx]->GetInputTensorNames())
        {
            if (std::find(graphInputNames.begin(), graphInputNames.end(), name) == graphInputNames.end())
            {
                inputs.insert(name);
            }
        }
        // Pushing to ready queue or nodes to be processed based on in-degree
        if (inputs.size() == 0)
        {
            readyOperators.push(idx);
        }
        else
        {
            inDegrees[idx] = inputs;
        }
    }

    // Second pass to process and sort operators
    std::vector<size_t> sortedOperators;
    while (!readyOperators.empty())
    {
        size_t idx = readyOperators.front();
        readyOperators.pop();

        sortedOperators.push_back(idx);
        std::vector<size_t> clearedIndices;
        for (auto& element : inDegrees)
        {
            for (const auto& name : operators[idx]->GetOutputTensorNames())
            {
                if (const auto nameIndex = std::find(element.second.begin(), element.second.end(), name);
                    nameIndex != element.second.end())
                {
                    element.second.erase(nameIndex);
                }
            }
            if (element.second.size() == 0)
            {
                readyOperators.push(element.first);
                clearedIndices.push_back(element.first);
            }
        }
        for (const size_t index : clearedIndices)
        {
            inDegrees.erase(index);
        }
    }
    // Copying back sorted array
    std::vector<std::unique_ptr<TosaSerializationOperator>> sortedPtrs;
    for (const size_t idx : sortedOperators)
    {
        sortedPtrs.push_back(std::move(operators[idx]));
    }
    operators = std::move(sortedPtrs);
}

std::vector<std::string> Spirv2tosa(const std::shared_ptr<spirv::Module>& module, OperatorComparator filter)
{
    std::vector<std::string> result;
    auto operators = Spirv2operators(module, filter);

    for (auto op : operators)
    {
        result.push_back(OperatorToString(op));
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

void GetTosaSerializationOperator(const TosaOperator& op,
                                  std::vector<std::unique_ptr<TosaSerializationOperator>>& operators,
                                  std::map<std::string, std::unique_ptr<TosaSerializationTensor>>& tensors,
                                  std::set<std::string>& dynamicInputNames,
                                  std::set<std::string>& dynamicOutputNames,
                                  const std::vector<std::vector<uint32_t>>& graphConstants,
                                  const std::map<uint32_t, std::string>& tensorNameMap,
                                  std::string tensorNamePrefix)
{
    // Processing Tensor Inputs
    std::vector<std::string> inputNames;
    for (const auto& input : op.inputs)
    {
        std::string tensorName = GetTosaSerializationTensorName(input.id, input.idx, tensorNameMap, tensorNamePrefix);
        if (tensors.find(tensorName) == tensors.end())
        {
            std::vector<uint32_t> data;
            if (input.type == tosaregen::TosaInputType::GraphConstant && graphConstants.size() > input.bindingId)
            {
                for (const uint32_t element : graphConstants.at(input.bindingId))
                {
                    data.push_back(element);
                }
            }
            else
            {
                data = input.GetAttribute().GetData();
            }
            tensors[tensorName] = GetTosaSerializationTensor(input.GetTensor(), tensorName, data);

            if (input.type == tosaregen::TosaInputType::TensorConstant ||
                input.type == tosaregen::TosaInputType::GraphConstant)
            {
                auto constOp = std::make_unique<TosaSerializationOperator>(Op::Op_CONST,
                                                                           Attribute::Attribute_NONE,
                                                                           nullptr,
                                                                           std::vector<std::string>{},
                                                                           std::vector<std::string>{tensorName},
                                                                           TosaOpLocation{});
                operators.push_back(std::move(constOp));
            }
        }
        inputNames.push_back(tensorName);
        if (input.type == tosaregen::TosaInputType::Dynamic)
        {
            dynamicInputNames.insert(tensorName);
        }
    }
    // Processing Tensor Outputs
    std::vector<std::string> outputNames;
    for (const auto& output : op.outputs)
    {
        std::string tensorName = GetTosaSerializationTensorName(output.id, output.idx, tensorNameMap, tensorNamePrefix);
        if (tensors.find(tensorName) == tensors.end())
        {
            tensors[tensorName] = GetTosaSerializationTensor(output.GetTensor(), tensorName, {});
        }
        outputNames.push_back(tensorName);
        dynamicOutputNames.insert(tensorName);
    }
    // Processing actual operator
    const auto operatorType = static_cast<Op>(static_cast<uint32_t>(op.op) + 1);
    const auto attributeType = static_cast<Attribute>(static_cast<uint32_t>(op.op) + 1);
    const auto attributePtr = GetTosaAttribute(op.op, op.attributes);
    auto serializationOp = std::make_unique<TosaSerializationOperator>(operatorType,
                                                                       attributeType,
                                                                       attributePtr.get(),
                                                                       inputNames,
                                                                       outputNames,
                                                                       TosaOpLocation{});
    operators.push_back(std::move(serializationOp));
}

std::unique_ptr<TosaSerializationHandler>
GetTosaSerializationHandler(const std::shared_ptr<tfsc::spirv::Module>& module,
                            const std::vector<std::vector<uint32_t>>& graphConstants,
                            const std::map<uint32_t, std::string>& tensorNameMap)
{
    const auto& ops = tosaregen::Spirv2operators(module, tosaregen::defaultOpComparator);

    std::vector<std::string> graphInputNames;
    const auto& inputOps = module->GetInstructionsOfType(spv::Op::OpGraphInputARM);
    for (auto instructionIt = inputOps.first; instructionIt != inputOps.second; ++instructionIt)
    {
        const uint32_t tensorId = instructionIt->m_Operands[1].m_LiteralWord;
        graphInputNames.push_back(GetTosaSerializationTensorName(tensorId, 0, tensorNameMap, "tensor_"));
    }
    std::sort(graphInputNames.begin(), graphInputNames.end());

    std::vector<std::string> graphOutputNames;
    const auto& outputOps = module->GetInstructionsOfType(spv::Op::OpGraphSetOutputARM);
    for (auto instructionIt = outputOps.first; instructionIt != outputOps.second; ++instructionIt)
    {
        const auto& outputInstruction = instructionIt->m_Operands[0].m_InstructionPtr;
        uint32_t tensorId = 0;
        uint32_t tensorIdx = 0;
        if (outputInstruction->m_Opcode == spv::Op::OpExtInst)
        {
            tensorId = outputInstruction->m_Operands[1].m_LiteralWord;
        }
        else if (outputInstruction->m_Opcode == spv::Op::OpCompositeExtract)
        {
            tensorId = outputInstruction->m_Operands[2].m_InstructionPtr->m_Operands[1].m_LiteralWord;
            tensorIdx = outputInstruction->m_Operands[3].m_LiteralWord;
        }
        graphOutputNames.push_back(GetTosaSerializationTensorName(tensorId, tensorIdx, tensorNameMap, "tensor_"));
    }
    std::sort(graphOutputNames.begin(), graphOutputNames.end());

    std::vector<std::unique_ptr<TosaSerializationOperator>> operators;
    std::map<std::string, std::unique_ptr<TosaSerializationTensor>> tensors;
    std::set<std::string> graphInputs;
    std::set<std::string> graphOutputs;
    for (const auto& op : ops)
    {
        GetTosaSerializationOperator(op, operators, tensors, graphInputs, graphOutputs, graphConstants, tensorNameMap);
    }
    SortTosaSerializationOperators(operators, graphInputNames);

    std::vector<std::unique_ptr<TosaSerializationTensor>> graphTensors;
    for (auto it = tensors.begin(); it != tensors.end(); ++it)
    {
        graphTensors.push_back(std::move(it->second));
    }

    auto tosaBlock =
        std::make_unique<TosaSerializationBasicBlock>("main",
                                                      "main",
                                                      std::move(operators),
                                                      std::move(graphTensors),
                                                      std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                                      graphInputNames,
                                                      graphOutputNames);
    auto tosaRegion = std::make_unique<TosaSerializationRegion>("main");
    tosaRegion->GetBlocks().push_back(std::move(tosaBlock));
    auto tosaHandler = std::make_unique<TosaSerializationHandler>();
    tosaHandler->GetRegions().push_back(std::move(tosaRegion));

    return std::move(tosaHandler);
}

} // namespace tosaregen
