//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <TosaSerializationParser.hpp>
#include <ParserUtils.hpp>

namespace tosa2spirv::parsers
{

using namespace ::tosa;
using namespace tosa2spirv::tosa;

TosaSerializationParser::TosaSerializationParser(TosaSerializationHandler* handler, const std::string& blockName)
{
    m_Block = handler->GetMainRegion()->GetBlockByName(blockName);
    if (!m_Block)
    {
        throw std::invalid_argument("TosaSerializationParser: Block was not found within TosaSerializationHandler.");
    }
}

bool IsConstOp(const TosaSerializationOperator* op)
{
    return op->GetOp() == Op_CONST || op->GetOp() == Op_CONST_SHAPE;
}

#ifndef NDEBUG
bool IsTensorConstant(TosaSerializationBasicBlock* block, const std::string_view tensorName)
{
    for (const auto& op : block->GetOperators())
    {
        for (const auto& outputTensorName : op->GetOutputTensorNames())
        {
            if (outputTensorName == tensorName)
            {
                return IsConstOp(op.get());
            }
        }
    }
    return false;
}
#endif  // NDEBUG

ResId TosaSerializationParser::AddExternalConstant(Graph& graph,
                                                   const Tensor& constTensor,
                                                   const ConstantData& data,
                                                   const std::string& constName)
{
    ResId constantResId = graph.AddGraphConstant(constTensor);
    m_ConstantData.emplace_back(data);
    m_OpNameResIdMap.try_emplace(constName, constantResId);
    return constantResId;
}

ResId TosaSerializationParser::GetInputTensorResId(Graph& graph, const std::string& inputName)
{
    const auto it = m_OpNameResIdMap.find(inputName);
    if (it == m_OpNameResIdMap.end())
    {
        throw std::runtime_error("TosaSerializationParser: input: " + inputName + " not found.");
    }
    return it->second;
}

void TosaSerializationParser::ParseConstantOp(TosaSerializationOperator* op, Graph& graph)
{
    assert(op->GetOutputTensorNames().size() == 1 &&
           "ParseConstantOp: constant op must have exactly one output tensor");

    for (const auto& tensorName : op->GetOutputTensorNames())
    {
        assert(m_OpNameResIdMap.find(tensorName) == m_OpNameResIdMap.end() &&
               "ParseConstantOp: tensor name already exists in m_OpNameResIdMap");

        ParseConstantTensor(tensorName, graph);
    }
}

ResId TosaSerializationParser::ParseConstantTensor(const std::string& tensorName, Graph& graph)
{
    assert(IsTensorConstant(m_Block, tensorName) && "ParseConstantTensor called on a non-constant tensor");

    TosaSerializationTensor* constTosaTensor = m_Block->GetTensorByName(tensorName);
    if (!constTosaTensor)
    {
        throw std::runtime_error("ParseConstantTensor: m_Block->GetTensorByName() returned null for: " + tensorName);
    }

    const Tensor constTensor(
        GetDataTypeFromDType(constTosaTensor->GetDtype()),
        ConvertTensorShape(constTosaTensor->GetShape()));

    constexpr auto internalConstantLimit = 16u;
    if (constTensor.GetTensorShape().size() > 1 || constTensor.GetNumElements() > internalConstantLimit)
    {
        if (constTensor.GetDataType() == DataType::int48_t)
        {
            std::vector<int64_t> vec;
            const auto err = TosaSerializationHandler::ConvertU8toI48(constTosaTensor->GetData(), constTensor.GetNumElements(), vec);
            if (err != TOSA_OK)
            {
                throw std::runtime_error("Failed to convert int48_t tensor: " + tensorName);
            }
            return AddExternalConstant(graph, constTensor, vec, tensorName);
        }
        if (constTensor.GetDataType() == DataType::int4_t)
        {
            std::vector<int8_t> vec;
            const auto err = TosaSerializationHandler::ConvertU8toI4(constTosaTensor->GetData(), constTensor.GetNumElements(), vec);
            if (err != TOSA_OK)
            {
                throw std::runtime_error("Failed to convert int4_t tensor: " + tensorName);
            }
            return AddExternalConstant(graph, constTensor, vec, tensorName);
        }
        const ConstantData constantData{constTosaTensor->GetData()};
        return AddExternalConstant(graph, constTensor, constantData, tensorName);
    }

    ResId constantResId;
    if (constTensor.GetDataType() == DataType::int48_t)
    {
        std::vector<int64_t> vec;
        const auto err = TosaSerializationHandler::ConvertU8toI48(constTosaTensor->GetData(), constTensor.GetNumElements(), vec);
        if (err != TOSA_OK)
        {
            throw std::runtime_error("Failed to convert int48_t tensor: " + tensorName);
        }
        constantResId = graph.AddTensorConstant(Attribute{vec, DataType::int48_t, constTensor.GetTensorShape()});
    }
    else if (constTensor.GetDataType() == DataType::int4_t)
    {
        std::vector<int8_t> vec;
        const auto err = TosaSerializationHandler::ConvertU8toI4(constTosaTensor->GetData(), constTensor.GetNumElements(), vec);
        if (err != TOSA_OK)
        {
            throw std::runtime_error("Failed to convert int4_t tensor: " + tensorName);
        }
        constantResId = graph.AddTensorConstant(Attribute{vec, constTensor.GetDataType(), constTensor.GetTensorShape()});
    }
    else if (constTensor.GetDataType() == DataType::bool_t)
    {
        std::vector<bool> vec;
        const auto err = TosaSerializationHandler::ConvertU8toBool(constTosaTensor->GetData(),
                                                                   constTensor.GetNumElements(), vec);
        if (err != TOSA_OK)
        {
            throw std::runtime_error("Failed to convert bool_t tensor: " + tensorName);
        }
        const Attribute constantAttribute{vec, DataType::bool_t, constTensor.GetTensorShape()};
        constantResId = graph.AddTensorConstant(constantAttribute);
    }
    else
    {
        const std::vector<uint32_t> vec = ConvertToUint32(constTosaTensor->GetData(), constTensor);
        constantResId = graph.AddTensorConstant(Attribute{vec, constTensor.GetDataType(), constTensor.GetTensorShape()});
    }

    m_OpNameResIdMap.try_emplace(tensorName, constantResId);
    return constantResId;
}


std::vector<uint32_t> TosaSerializationParser::GenerateSPIRV(std::string graphName)
{
    const auto module = GenerateSPIRVModule(graphName);
    return WriteToBinary(module);
}

std::shared_ptr<spirv::Module> TosaSerializationParser::GenerateSPIRVModule(const std::string graphName)
{
    auto module = CreateModule(m_Version);
    auto graph = Graph(module, graphName);

    unsigned int bindingId = 0;

    // 1. Handle Inputs
    for (const auto& inputName : m_Block->GetInputs())
    {
        const TosaSerializationTensor* inputTosaTensor = m_Block->GetTensorByName(inputName);
        Tensor inputTensor{GetDataTypeFromDType(inputTosaTensor->GetDtype()),
                           ConvertTensorShape(inputTosaTensor->GetShape())};

        const auto inputResId = graph.AddInput(inputTensor, bindingId++);
        m_OpNameResIdMap.try_emplace(inputName, inputResId);
    }

    // 2. Constants pass — parses all constant operators
    for (const auto& op : m_Block->GetOperators())
    {
        if (IsConstOp(op.get()))
        {
            ParseConstantOp(op.get(), graph);
        }
    }

    // 3. Main operator parsing
    for (const auto& op : m_Block->GetOperators())
    {
        if (!IsConstOp(op.get()))
        {
            ParseOperator(op.get(), graph);
        }
    }

    // 4. Add Outputs to graph
    for (const auto& outputName : m_Block->GetOutputs())
    {
        auto it = m_OpNameResIdMap.find(outputName);
        if (it != m_OpNameResIdMap.end())
        {
            graph.AddOutput(it->second, bindingId++);
        }
        else
        {
            throw std::runtime_error("Output name " + outputName + " not found in m_OpNameResIdMap.");
        }
    }

    graph.FinalizeGraph();

    return module;
}

void TosaSerializationParser::ParseOperator(TosaSerializationOperator* op, Graph& graph)
{
    Op flatbufferEnum = op->GetOp();

    if (flatbufferEnum == Op_CONST)
    {
        return;
    }
    if (flatbufferEnum == Op_CONST_SHAPE)
    {
        return;
    }

    std::vector<std::string>& inputNames = op->GetInputTensorNames();
    std::vector<std::string>& outputNames = op->GetOutputTensorNames();

    if (flatbufferEnum == Op_IDENTITY)
    {
        auto resId = m_OpNameResIdMap.at(inputNames[0]);
        m_OpNameResIdMap.try_emplace(outputNames[0], resId);
        return;
    }

    std::vector<ResId> inputTensors;
    std::vector<DataType> inputDataTypes;
    std::vector<Tensor> outputTensors;
    OperatorEnum operatorEnum = OperatorEnumMap(flatbufferEnum);

    for (const auto& inputName : inputNames)
    {
        inputTensors.push_back(GetInputTensorResId(graph, inputName));
        const auto inputDataType = m_Block->GetTensorByName(inputName)->GetDtype();
        inputDataTypes.push_back(GetDataTypeFromDType(inputDataType));
    }

    for (const auto& outputName : outputNames)
    {
        const TosaSerializationTensor* outputTosaTensor = m_Block->GetTensorByName(outputName);
        if (!outputTosaTensor)
        {
            throw std::runtime_error("ParseOperator: m_Block->GetTensorByName(outputName) returned null.");
        }
        outputTensors.emplace_back(GetDataTypeFromDType(outputTosaTensor->GetDtype()),
                                   ConvertTensorShape(outputTosaTensor->GetShape()));
    }

    auto DTypeToInt = [](DType dtype) -> int {
        switch (dtype)
        {
            case DType::DType_INT32: return 1;
            case DType::DType_FP16: return 2;
            case DType::DType_FP32: return 3;
            case DType::DType_INT48: return 4;
            default:
                throw std::runtime_error("Acc_type: Unsupported DType: " + std::to_string(static_cast<int>(dtype)));
        }
    };

    std::vector<tosa2spirv::tosa::Attribute> attributes;
    auto attr = op->GetAttributeType();
    switch (attr)
    {
        case Attribute_NONE:
        case Attribute_MatMulAttribute:
        case Attribute_ErfAttribute:
        case Attribute_SigmoidAttribute:
        case Attribute_TanhAttribute:
        case Attribute_AddAttribute:
        case Attribute_BitwiseAndAttribute:
        case Attribute_BitwiseOrAttribute:
        case Attribute_BitwiseXorAttribute:
        case Attribute_IntDivAttribute:
        case Attribute_LogicalAndAttribute:
        case Attribute_LogicalLeftShiftAttribute:
        case Attribute_LogicalRightShiftAttribute:
        case Attribute_LogicalOrAttribute:
        case Attribute_LogicalXorAttribute:
        case Attribute_MulAttribute:
        case Attribute_PowAttribute:
        case Attribute_SubAttribute:
        case Attribute_TableAttribute:
        case Attribute_AbsAttribute:
        case Attribute_BitwiseNotAttribute:
        case Attribute_CeilAttribute:
        case Attribute_ClzAttribute:
        case Attribute_CosAttribute:
        case Attribute_ExpAttribute:
        case Attribute_FloorAttribute:
        case Attribute_LogAttribute:
        case Attribute_LogicalNotAttribute:
        case Attribute_NegateAttribute:
        case Attribute_ReciprocalAttribute:
        case Attribute_RsqrtAttribute:
        case Attribute_SinAttribute:
        case Attribute_SelectAttribute:
        case Attribute_EqualAttribute:
        case Attribute_GreaterAttribute:
        case Attribute_GreaterEqualAttribute:
        case Attribute_PadAttribute:
        case Attribute_ReshapeAttribute:
        case Attribute_SliceAttribute:
        case Attribute_TileAttribute:
        case Attribute_GatherAttribute:
        case Attribute_ScatterAttribute:
        case Attribute_CastAttribute:
        case Attribute_ConstAttribute:
        case Attribute_IdentityAttribute:
        case Attribute_VariableAttribute:
        case Attribute_VariableWriteAttribute:
        case Attribute_VariableReadAttribute:
        case Attribute_ConstShapeAttribute: break;
        case Attribute_ClampAttribute:
        {
            auto dataType = inputDataTypes[0];

            auto uint8ToFloat32 = [](const std::vector<unsigned char>& bytes) {
                if (bytes.size() < 4)
                {
                    throw std::runtime_error("Not enough bytes to convert to float32");
                }
                float value;
                std::memcpy(&value, bytes.data(), sizeof(float));
                return value;
            };
            auto uint8ToFloat16 = [](const std::vector<unsigned char>& bytes) {
                if (bytes.size() < 2)
                {
                    throw std::runtime_error("Not enough bytes to convert to float16");
                }
                float value;
                std::memcpy(&value, bytes.data(), sizeof(float));
                return value;
            };

            auto uint8ToInt16 = [](const std::vector<unsigned char>& bytes) {
                if (bytes.size() < 2)
                {
                    throw std::runtime_error("Not enough bytes to convert to int16");
                }
                int16_t value;
                std::memcpy(&value, bytes.data(), sizeof(int16_t));
                return value;
            };

            TosaClampAttribute attribute = op->GetAttribute();

            auto minVal = attribute.min_val();
            auto maxVal = attribute.max_val();
            switch (dataType)
            {
                case tosa::DataType::int16_t:
                {
                    attributes.emplace_back(uint8ToInt16(minVal), dataType);
                    attributes.emplace_back(uint8ToInt16(maxVal), dataType);
                    break;
                }
                case tosa::DataType::float32_t:
                {
                    attributes.emplace_back(uint8ToFloat32(minVal), dataType);
                    attributes.emplace_back(uint8ToFloat32(maxVal), dataType);
                    break;
                }
                case tosa::DataType::float16_t:
                case tosa::DataType::bfloat16_t:
                {
                    attributes.emplace_back(uint8ToFloat16(minVal), dataType);
                    attributes.emplace_back(uint8ToFloat16(maxVal), dataType);
                    break;
                }
                case tosa::DataType::int8_t:
                {
                    attributes.emplace_back(static_cast<int8_t>(minVal[0]), dataType);
                    attributes.emplace_back(static_cast<int8_t>(maxVal[0]), dataType);
                    break;
                }
                default: throw std::runtime_error("Unsupported data type for CLAMP attributes.");
            }

            attributes.emplace_back(static_cast<int32_t>(attribute.nan_mode()));
            break;
        }
        case Attribute_ReduceAllAttribute:
        {
            TosaReduceAllAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            break;
        }
        case Attribute_ReduceAnyAttribute:
        {
            TosaReduceAnyAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            break;
        }
        case Attribute_ReduceMaxAttribute:
        {
            TosaReduceMaxAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            attributes.emplace_back(static_cast<int32_t>(attribute.nan_mode()));
            break;
        }
        case Attribute_ReduceMinAttribute:
        {
            TosaReduceMinAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            attributes.emplace_back(static_cast<int32_t>(attribute.nan_mode()));
            break;
        }
        case Attribute_ReduceProductAttribute:
        {
            TosaReduceProductAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            break;
        }
        case Attribute_ReduceSumAttribute:
        {
            TosaReduceSumAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            break;
        }
        case Attribute_ReverseAttribute:
        {
            TosaReverseAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            break;
        }
        case Attribute_ArgMaxAttribute:
        {
            TosaArgMaxAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            attributes.emplace_back(static_cast<int32_t>(attribute.nan_mode()));
            break;
        }
        case Attribute_AvgPool2dAttribute:
        {
            TosaAvgPool2dAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.kernel());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(attribute.pad());
            attributes.emplace_back(DTypeToInt(attribute.acc_type()));
            break;
        }
        case Attribute_MaxPool2dAttribute:
        {
            TosaMaxPool2dAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.kernel());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(attribute.pad());
            attributes.emplace_back(static_cast<int32_t>(attribute.nan_mode()));
            break;
        }
        case Attribute_MinimumAttribute:
        {
            TosaMinimumAttribute attribute = op->GetAttribute();
            attributes.emplace_back(static_cast<int32_t>(attribute.nan_mode()));
            break;
        }
        case Attribute_MaximumAttribute:
        {
            TosaMaximumAttribute attribute = op->GetAttribute();
            attributes.emplace_back(static_cast<int32_t>(attribute.nan_mode()));
            break;
        }
        case Attribute_ConcatAttribute:
        {
            TosaConcatAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            break;
        }
        case Attribute_FFT2dAttribute:
        {
            TosaFFT2dAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.inverse());
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_RFFT2dAttribute:
        {
            TosaFFT2dAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_Conv2dAttribute:
        {
            TosaConv2dAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.pad());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(attribute.dilation());
            attributes.emplace_back(DTypeToInt(attribute.acc_type()));
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_Conv3dAttribute:
        {
            TosaConv3dAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.pad());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(attribute.dilation());
            attributes.emplace_back(DTypeToInt(attribute.acc_type()));
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_TransposeConv2dAttribute:
        {
            TosaTransposeConv2dAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.out_pad());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(DTypeToInt(attribute.acc_type()));
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_DepthwiseConv2dAttribute:
        {
            TosaDepthwiseConv2dAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.pad());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(attribute.dilation());

            attributes.emplace_back(DTypeToInt(attribute.acc_type()));
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_ArithmeticRightShiftAttribute:
        {
            TosaArithmeticRightShiftAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.round());
            break;
        }
        case Attribute_RescaleAttribute:
        {
            TosaRescaleAttribute attribute = op->GetAttribute();

            attributes.emplace_back(attribute.scale32());
            attributes.emplace_back(static_cast<int32_t>(attribute.rounding_mode()));
            attributes.emplace_back(attribute.per_channel());
            attributes.emplace_back(attribute.input_unsigned());
            attributes.emplace_back(attribute.output_unsigned());
            break;
        }
        case Attribute_ResizeAttribute:
        {
            TosaResizeAttribute attribute = op->GetAttribute();
            attributes.emplace_back(static_cast<int32_t>(attribute.mode()));
            break;
        }
        case Attribute_TransposeAttribute:
        {
            TosaTransposeAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.perms());
            break;
        }
        default:
        {
            throw std::runtime_error("Unsupported attribute type: " + std::to_string(static_cast<int>(attr)));
        }
    }

    auto resIds = graph.AddOperator(operatorEnum, inputTensors, outputTensors, attributes);
    auto resId = resIds.begin();
    auto outputTensor = outputTensors.begin();
    for (const auto& outputName : outputNames)
    {
        m_OpNameResIdMap.try_emplace(outputName, *resId);
        ++resId;
        ++outputTensor;
    }
}

}  // namespace tosa2spirv::parsers