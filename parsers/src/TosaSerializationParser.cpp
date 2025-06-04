//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <ParserUtils.hpp>
#include <TosaSerializationParser.hpp>

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

bool IsTensorConstant(TosaSerializationBasicBlock* block, const std::string_view tensorName)
{
    for (const auto& op : block->GetOperators())
    {
        for (const auto& outputTensorName : op->GetOutputTensorNames())
        {
            if (outputTensorName == tensorName)
            {
                return op->GetOp() == Op_CONST;
            }
        }
    }
    return false;
}

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

ResId TosaSerializationParser::InitializeInputTensor(Graph& graph, const std::string& inputName)
{
    // Attempt to resolve the input
    if (const auto input = m_OpNameResIdMap.find(inputName); input != m_OpNameResIdMap.end())
    {
        return input->second;
    }
    // If the input is not a constant and is not resolved something has gone wrong
    if (!IsTensorConstant(m_Block, inputName))
    {
        throw std::runtime_error("TosaSerializationParser: input: " + inputName + " not found.");
    }

    // Constants are created at this point to maintain order with the VulcanMLBackend
    const TosaSerializationTensor* constTosaTensor = m_Block->GetTensorByName(inputName);
    const auto constTensor =
        Tensor(GetDataTypeFromDType(constTosaTensor->GetDtype()), ConvertTensorShape(constTosaTensor->GetShape()));

    constexpr auto internalConstantLimit = 16u;
    if (constTensor.GetTensorShape().size() > 1 || constTensor.GetNumElements() > internalConstantLimit)
    {
        if (constTensor.GetDataType() == DataType::int48_t)
        {
            const std::vector<int64_t> vec = ConvertToInt64(constTosaTensor->GetData(), constTensor);
            return AddExternalConstant(graph, constTensor, vec, inputName);
        }
        if (constTensor.GetDataType() == DataType::int4_t)
        {
            const std::vector<uint8_t> vec = UnpackInt4Signed(constTosaTensor->GetData(), constTensor);
            return AddExternalConstant(graph, constTensor, vec, inputName);
        }
        const ConstantData constantData{constTosaTensor->GetData()};
        return AddExternalConstant(graph, constTensor, constantData, inputName);
    }

    ResId constantResId;
    if (constTensor.GetDataType() == DataType::int48_t)
    {
        const std::vector<int64_t> vec = ConvertToInt64(constTosaTensor->GetData(), constTensor);
        const Attribute constantAttribute{vec, DataType::int48_t, constTensor.GetTensorShape()};
        constantResId = graph.AddTensorConstant(constantAttribute);
    }
    else if (constTensor.GetDataType() == DataType::int4_t)
    {
        const std::vector<uint8_t> vec = UnpackInt4Signed(constTosaTensor->GetData(), constTensor);
        const Attribute constantAttribute{vec, constTensor.GetDataType(), constTensor.GetTensorShape()};
        constantResId = graph.AddTensorConstant(constantAttribute);
    }
    else
    {
        const std::vector<uint32_t> vec = ConvertToUint32(constTosaTensor->GetData(), constTensor);
        const Attribute constantAttribute{vec, constTensor.GetDataType(), constTensor.GetTensorShape()};
        constantResId = graph.AddTensorConstant(constantAttribute);
    }
    m_OpNameResIdMap.try_emplace(inputName, constantResId);
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
    for (const auto& inputName : m_Block->GetInputs())
    {
        const TosaSerializationTensor* inputTosaTensor = m_Block->GetTensorByName(inputName);
        Tensor inputTensor{GetDataTypeFromDType(inputTosaTensor->GetDtype()),
                           ConvertTensorShape(inputTosaTensor->GetShape())};

        const auto inputResId = graph.AddInput(inputTensor, bindingId++);
        m_OpNameResIdMap.try_emplace(inputName, inputResId);
    }

    // Loop through all operators and start to build up the SPIRV graph
    for (const auto& op : m_Block->GetOperators())
    {
        ParseOperator(op, graph);
    }

    // Now that we have parsed the graph we can resolve the resIds of the output tensors
    for (const auto& outputName : m_Block->GetOutputs())
    {
        ResId resId;
        auto it = m_OpNameResIdMap.find(outputName);
        if (it != m_OpNameResIdMap.end())
        {
            resId = it->second;
        }
        else
        {
            // Likely a disconnected tensor that we need to resolve
            resId = InitializeInputTensor(graph, outputName);
        }
        graph.AddOutput(resId, bindingId++);
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
        inputTensors.push_back(InitializeInputTensor(graph, inputName));
        const auto inputDataType = m_Block->GetTensorByName(inputName)->GetDtype();
        inputDataTypes.push_back(GetDataTypeFromDType(inputDataType));
    }

    for (const auto& outputName : outputNames)
    {
        TosaSerializationTensor const* outputTosaTensor = m_Block->GetTensorByName(outputName);
        outputTensors.emplace_back(GetDataTypeFromDType(outputTosaTensor->GetDtype()),
                                   ConvertTensorShape(outputTosaTensor->GetShape()));
    }

    std::vector<Attribute> attributes;
    switch (op->GetAttributeType())
    {
        case Attribute_NONE: break;
        case Attribute_AxisAttribute:
        {
            TosaAxisAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.axis());
            break;
        }
        case Attribute_PoolAttribute:
        {
            TosaPoolAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.kernel());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(attribute.pad());

            if (operatorEnum == OperatorEnum::MaxPool2d)
            {
                break;
            }
            attributes.emplace_back(ConvertAvgPoolAccType(attribute));
            attributes.emplace_back(attribute.input_zp(), inputDataTypes[0]);
            attributes.emplace_back(attribute.output_zp(), outputTensors[0].GetDataType());
            break;
        }
        case Attribute_ConvAttribute:
        {
            TosaConvAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.pad());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(attribute.dilation());
            attributes.emplace_back(attribute.input_zp(), inputDataTypes[0]);
            attributes.emplace_back(attribute.weight_zp(), inputDataTypes[1]);
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_TransposeConvAttribute:
        {
            TosaTransposeConvAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.out_pad());
            attributes.emplace_back(attribute.stride());
            attributes.emplace_back(attribute.output_shape());
            attributes.emplace_back(attribute.input_zp(), inputDataTypes[0]);
            attributes.emplace_back(attribute.weight_zp(), inputDataTypes[1]);
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_PadAttribute:
        {
            TosaPadAttribute attribute = op->GetAttribute();
            const auto inputName = op->GetInputTensorNames()[0];
            TosaSerializationTensor* const inputTosaTensor = m_Block->GetTensorByName(inputName);
            auto paddingTensor =
                Tensor(DataType::int32_t, {static_cast<unsigned int>(inputTosaTensor->GetShape().size()), 2u});

            ConstantData constant{attribute.padding()};
            const auto padding = AddExternalConstant(graph, paddingTensor, constant, "padding");
            attributes.emplace_back(padding);
            attributes.emplace_back(std::vector{attribute.pad_const_int()}, inputDataTypes[0]);
            break;
        }
        case Attribute_ReshapeAttribute:
        {
            TosaReshapeAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.new_shape());
            break;
        }
        case Attribute_SliceAttribute:
        {
            TosaSliceAttribute attribute = op->GetAttribute();
            const auto sliceStartTensorValues = ConvertTensorShape(attribute.start());

            attributes.emplace_back(attribute.start(), DataType::uint32_t);
            attributes.emplace_back(attribute.size(), DataType::uint32_t);
            break;
        }
        case Attribute_TileAttribute:
        {
            TosaTileAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.multiples());
            break;
        }
        case Attribute_ResizeAttribute:
        {
            TosaResizeAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.scale(), DataType::uint32_t);
            attributes.emplace_back(attribute.offset(), DataType::uint32_t);
            attributes.emplace_back(attribute.border(), DataType::uint32_t);

            uint32_t mode = attribute.mode() == 1 ? 0 : 1;
            std::vector modeVector = {mode};
            attributes.emplace_back(modeVector);
            break;
        }
        case Attribute_ClampAttribute:
        {
            TosaClampAttribute attribute = op->GetAttribute();
            attributes.emplace_back(std::vector{attribute.min_int()}, inputDataTypes[0]);
            attributes.emplace_back(std::vector{attribute.max_int()}, inputDataTypes[0]);
            break;
        }
        case Attribute_RescaleAttribute:
        {
            TosaRescaleAttribute attribute = op->GetAttribute();

            auto multiplierTensor =
                Tensor(DataType::int32_t, std::vector{static_cast<unsigned int>(attribute.multiplier().size())});
            ConstantData multiplierConstant{attribute.multiplier()};
            const auto multiplier = AddExternalConstant(graph, multiplierTensor, multiplierConstant, "multiplier");

            auto shiftTensor =
                Tensor(DataType::int8_t, std::vector{static_cast<unsigned int>(attribute.shift().size())});

            std::vector<int8_t> int8Vector;
            const std::vector<int32_t>& int32Vector = attribute.shift();
            int8Vector.reserve(int32Vector.size());
            std::transform(int32Vector.begin(),
                           int32Vector.end(),
                           std::back_inserter(int8Vector),
                           [](const int32_t value) { return static_cast<int8_t>(value); });

            ConstantData shiftConstant{std::move(int8Vector)};
            const auto shift = AddExternalConstant(graph, shiftTensor, shiftConstant, "shift");

            attributes.emplace_back(attribute.input_zp(), inputDataTypes[0]);
            attributes.emplace_back(attribute.output_zp(), outputTensors[0].GetDataType());
            attributes.emplace_back(multiplier);
            attributes.emplace_back(shift);
            attributes.emplace_back(attribute.scale32());
            attributes.emplace_back(attribute.double_round());
            attributes.emplace_back(attribute.per_channel());
            attributes.emplace_back(attribute.input_unsigned());
            attributes.emplace_back(attribute.output_unsigned());
            break;
        }
        case Attribute_MulAttribute:
        {
            TosaMulAttribute attribute = op->GetAttribute();

            std::vector<int32_t> int32Vector = {attribute.shift()};
            std::vector<int8_t> int8Vector;
            int8Vector.reserve(int32Vector.size());

            std::transform(int32Vector.begin(),
                           int32Vector.end(),
                           std::back_inserter(int8Vector),
                           [](const int32_t value) { return static_cast<int8_t>(value); });
            attributes.emplace_back(std::move(int8Vector), DataType::int8_t, Tensor::TensorShape{1u});

            break;
        }
        case Attribute_ArithmeticRightShiftAttribute:
        {
            TosaArithmeticRightShiftAttribute attribute = op->GetAttribute();
            attributes.emplace_back(std::vector{attribute.round()});
            break;
        }
        case Attribute_TransposeAttribute:
        {
            TosaTransposeAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.perms());
            break;
        }
        case Attribute_TableAttribute:
        {
            TosaTableAttribute attribute = op->GetAttribute();
            const auto inputName = op->GetInputTensorNames()[0];
            const auto inputDataType = GetDataTypeFromDType(m_Block->GetTensorByName(inputName)->GetDtype());
            DataType datatype = attribute.table().size() > 256 ? DataType::int16_t : DataType::int8_t;
            auto tableTensor = Tensor(datatype, {static_cast<unsigned int>(attribute.table().size())});
            ResId tableConstantResId;
            // if int8 we need to convert from int16 given by attribute
            if (inputDataType == DataType::int8_t)
            {
                std::vector<int8_t> int8Vector;
                const std::vector<int16_t>& int16Vector = attribute.table();
                int8Vector.reserve(int16Vector.size());
                std::transform(int16Vector.begin(),
                               int16Vector.end(),
                               std::back_inserter(int8Vector),
                               [](const int16_t value) { return static_cast<int8_t>(value); });
                const auto table = ConstantData{std::move(int8Vector)};
                tableConstantResId = AddExternalConstant(graph, tableTensor, table, "table");
            }
            else
            {
                ConstantData table{attribute.table()};
                tableConstantResId = AddExternalConstant(graph, tableTensor, table, "table");
            }
            attributes.emplace_back(tableConstantResId);
            break;
        }
        case Attribute_MatMulAttribute:
        {
            TosaMatMulAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.a_zp(), inputDataTypes[0]);
            attributes.emplace_back(attribute.b_zp(), inputDataTypes[1]);
            break;
        }
        case Attribute_FullyConnectedAttribute:
        {
            TosaFullyConnectedAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.input_zp(), inputDataTypes[0]);
            attributes.emplace_back(attribute.weight_zp(), inputDataTypes[1]);
            break;
        }
        case Attribute_NegateAttribute:
        {
            TosaNegateAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.input1_zp(), inputDataTypes[0]);
            attributes.emplace_back(attribute.output_zp(), inputDataTypes[0]);
            break;
        }
        case Attribute_RFFTAttribute:
        {
            break;
        }
        case Attribute_FFTAttribute:
        {
            TosaFFTAttribute attribute = op->GetAttribute();
            attributes.emplace_back(attribute.inverse());
            attributes.emplace_back(attribute.local_bound());
            break;
        }
        case Attribute_CondIfAttribute:
        case Attribute_CustomAttribute:
        case Attribute_WhileLoopAttribute:
        default:
        {
            throw std::runtime_error("Unsupported attribute type");
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

} // namespace tosa2spirv::parsers
