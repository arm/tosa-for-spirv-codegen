//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <TosaSerializationParser.hpp>
#include <ParserUtils.hpp>
#include <iostream>
#include <variant>


namespace tosa2spirv::parsers
{

using namespace tosa;
using namespace graphbuilder;

TosaSerializationParser::TosaSerializationParser(TosaSerializationHandler* handler, std::string& blockName)
{
    m_Block = handler->GetMainRegion()->GetBlockByName(blockName);
    if (!m_Block)
    {
        throw std::invalid_argument("TosaSerializationParser: Block was not found within TosaSerializationHandler.");
    }
}

void TosaSerializationParser::AddGraphConstantToMap(const Tensor& tensor,
                                                    const GraphConstant& data,
                                                    const std::string& tensorName)
{
    graphbuilder::GraphConstant graphConstant = m_Module->AddGraphConstant(tensorName, tensor);
    m_GraphConstants.try_emplace(graphConstant.GetGraphConstantId(), data);
    m_OpNameResIdMap[tensorName] = tensor.GetResId();
}

static bool IsTensorConstant(TosaSerializationBasicBlock* block, std::string_view tensorName)
{
    for (const auto& op : block->GetOperators())
    {
        for (const auto& outputTensorName : op->GetOutputTensorNames())
        {
            if (outputTensorName == tensorName)
            {
                return op->GetOp() == tosa::Op_CONST;
            }
        }
    }
    return false;
}

Tensor TosaSerializationParser::InitializeInputTensor(const TosaSerializationTensor* inputTosaTensor,
                                                      const std::string& inputName,
                                                      const Graph* graph)
{
    // Check for constant tensor
    if (IsTensorConstant(m_Block, inputName))
    {
        TosaSerializationTensor* constTosaTensor = m_Block->GetTensorByName(inputName);

        auto resId = graph->GetNextResId();
        // This will be replaced by an AddGraphConstant helper function
        auto constTensor = Tensor(GetDataTypeFromDType(constTosaTensor->GetDtype()),
                                  ConvertTensorShape(constTosaTensor->GetShape()),
                                  resId);

        AddGraphConstantToMap(constTensor, constTosaTensor, inputName);
        return constTensor;
    }

    // Check if the input of the Layer is the input of the Block
    std::unordered_map<std::string, uint32_t>::iterator nameBindingPair;
    if ((nameBindingPair = m_InputLayerNameMap.find(inputName)) != m_InputLayerNameMap.end())
    {
         return Tensor::CreateInput(GetDataTypeFromDType(inputTosaTensor->GetDtype()),
                                    ConvertTensorShape(inputTosaTensor->GetShape()),
                                    m_Module,
                                    *nameBindingPair);
    }

    // Use map to find the correct ResId to connect previous layer
    return  {GetDataTypeFromDType(inputTosaTensor->GetDtype()),
             ConvertTensorShape((inputTosaTensor->GetShape())),
             m_OpNameResIdMap.at(inputName)};
}

Tensor TosaSerializationParser::InitializeOutputTensor(tosa::TosaSerializationTensor* outputTosaTensor,
                                                                     const std::string& outputName)
{
    // Check if the output of the Layer is the output of the Block
    std::unordered_map<std::string, uint32_t>::iterator nameBindingPair;
    if ((nameBindingPair = m_OutputLayerNameMap.find(outputName)) != m_OutputLayerNameMap.end())
    {
        // Mark the IsGraphOutput as true
        return Tensor::CreateOutput(GetDataTypeFromDType(outputTosaTensor->GetDtype()),
                                    ConvertTensorShape(outputTosaTensor->GetShape()),
                                    m_Module,
                                    *nameBindingPair);
    }
    return Tensor{GetDataTypeFromDType(outputTosaTensor->GetDtype()), ConvertTensorShape(outputTosaTensor->GetShape())};
}

void TosaSerializationParser::OptimizeIdentityOp()
{
    int opCounter = 0, identityCounter = 0;

    for (const auto& op : m_Block->GetOperators())
    {
        opCounter++;

        if (op->GetOp() == Op_IDENTITY)
        {
            identityCounter++;

            std::vector<std::string>& inputNames = op->GetInputTensorNames();
            CHECK_INPUT(inputNames, 1);
            std::string inputName = inputNames[0];

            std::vector<std::string>& outputNames = op->GetOutputTensorNames();
            CHECK_OUTPUT(outputNames, 1);
            std::string outputName = outputNames[0];

            // Graph input/output should not be connected directly with another Graph output with all Identity Ops
            if (m_OutputLayerNameMap.find(outputName) != m_OutputLayerNameMap.end())
            {
                std::string identityBlockInputName = inputName;

                // Skip Identity operators and get the mapped tensorName from the most previous Non-Identity Op.
                // Check if the input was an output tensor from previous Identity operator, Continue to map the output
                // of the Identity to its input until the inputName is not an output from Identity.
                while (m_IdentityOutputNameMap.find(identityBlockInputName) != m_IdentityOutputNameMap.end())
                {
                    // If either of the intermediate input/output tensor of Identity is found as graph input/output,
                    // throw an exception as the graph input/output are connected via Identity Ops.
                    if (m_OutputLayerNameMap.find(identityBlockInputName) != m_OutputLayerNameMap.end() ||
                        m_InputLayerNameMap.find(identityBlockInputName) != m_InputLayerNameMap.end())
                    {
                        throw std::invalid_argument("TosaSerializationParser: "
                            "Graph input/output should not be connected directly with Identity Op!");
                    }
                    identityBlockInputName = m_IdentityOutputNameMap[identityBlockInputName];
                }

                // Check when no 'tracing up' happens or for the last input/output tensor left from the while loop.
                if (m_OutputLayerNameMap.find(identityBlockInputName) != m_OutputLayerNameMap.end() ||
                    m_InputLayerNameMap.find(identityBlockInputName) != m_InputLayerNameMap.end())
                {
                    throw std::invalid_argument("TosaSerializationParser: "
                        "Graph input/output should not be connected directly with Identity Op!");
                }
            }

            // Map inputName to outputName in order to skip the Identity operator
            m_IdentityInputNameMap[inputName] = outputName;
            // Map outputName to inputName in order to skip the Identity operator only when it is not a graph output
            if (m_OutputLayerNameMap.find(outputName) == m_OutputLayerNameMap.end())
            {
                m_IdentityOutputNameMap[outputName] = inputName;
            }
        }
    }

    // Cannot parse a model with only Identity Ops
    if (identityCounter == opCounter)
    {
        throw std::invalid_argument("TosaSerializationParser: Block contains only Identity operators!");
    }
}

std::string TosaSerializationParser::GetInputName(std::string inputName)
{
    // Skip Identity operators and get the mapped tensorName from the most previous Non-Identity Op.
    // Check if the input was an output tensor from previous Identity operator
    // Continue to map the output of the Identity to its input until the inputName is not an output from Identity.
    while (m_IdentityOutputNameMap.find(inputName) != m_IdentityOutputNameMap.end())
    {
        inputName = m_IdentityOutputNameMap[inputName];
    }
    return inputName;
}

std::string TosaSerializationParser::GetOutputName(std::string outputName)
{
    // Skip Identity operators and get the mapped tensorName from the closest following Non-Identity Op.
    // Check if the output was an input tensor from the following Identity operator.
    // Continue to map the input of the Identity to its output until the inputName is not an output from Identity.
    std::string tensorName = outputName;
    while (m_IdentityInputNameMap.find(tensorName) != m_IdentityInputNameMap.end())
    {
        tensorName = m_IdentityInputNameMap[tensorName];

        // Any 'tracing up' step should not go over the graph output.
        // If an intermediate tensor between two Identity Ops is a graph output, the graph output should not be skipped
        // The Graph Output will be kept and replace the Identity Op to connect previous and following layers, then
        // the Graph Output has been removed from the IdentityOutputNameMap to prevent tracing back to previous layers
        // i.e. Clamp -> Identity -> (Graph Output) -> Identity -> Conv2d will become
        // Clamp -> (Graph Output) -> Conv2d, where Conv2d' input name will only be traced back to the 'Graph Output'.
        // instead of finding Clamp's original outputName.
        if (m_OutputLayerNameMap.find(tensorName) != m_OutputLayerNameMap.end())
        {
            return tensorName;
        }
    }

    return outputName;
}
std::vector<uint32_t> TosaSerializationParser::GenerateSPIRV(std::string graphName)
{
    auto graph = AddGraph(graphName);

    // Loop through the blocks inputs, to add inputs to graph
    for (const auto& inputName : m_Block->GetInputs())
    {
        // Save the input name and add a DescriptorSetBinding
        m_InputLayerNameMap[inputName] = m_DescSetBindingIdx++;
    }

    // Loop through the blocks outputs, to add outputs to graph
    std::vector<std::string> outputNames;
    for (const auto& outputName : m_Block->GetOutputs())
    {
        // Save the output name and add a DescriptorSetBinding
        m_OutputLayerNameMap[outputName] = m_DescSetBindingIdx++;
        outputNames.emplace_back(outputName);
    }

    // Handle the Identity operator and skip it when parsing other operators
    OptimizeIdentityOp();

    // Loop through all operators and start to build up the SPIRV graph
    for (const auto& op : m_Block->GetOperators())
    {
        ParseOp(op, graph.get());
    }

    // Override the order of graph outputs, so as to follow TOSA block outputs order
    std::vector<ResId> outputIds;
    for (const auto& outputName : outputNames)
    {
        outputIds.emplace_back(m_OpNameResIdMap[outputName]);
    }

    graph->SetGraphOutputs(outputIds);

    auto binary = m_Module->Write();

    return binary;
}

void TosaSerializationParser::ParseOp(TosaSerializationOperator* op, Graph* graph)
{
    Op flatbufferEnum = op->GetOp();
    if (flatbufferEnum == Op_IDENTITY)
    {
        return;
    }
    OperatorEnum operatorEnum;
    if (flatbufferEnum != Op_CONST)
    {
        operatorEnum = OperatorEnumMap(flatbufferEnum);
    }

    std::vector<std::string>& inputNames = op->GetInputTensorNames();
    std::vector<Tensor> inputTensors;
    std::vector<Tensor> outputTensors;

    for (const auto& inputName : inputNames)
    {
        auto name = GetInputName(inputName);
        TosaSerializationTensor* inputTosaTensor = m_Block->GetTensorByName(name);
        inputTensors.push_back(InitializeInputTensor(inputTosaTensor, name, graph));
    }

    std::vector<std::string>& outputNames = op->GetOutputTensorNames();
    for (const auto& outputName : outputNames)
    {
        auto name = GetOutputName(outputName);
        TosaSerializationTensor* outputTosaTensor = m_Block->GetTensorByName(name);
        outputTensors.push_back(InitializeOutputTensor(outputTosaTensor, name));
    }

    std::vector<Tensor> attributeTensors;
    switch (op->GetAttributeType())
    {
        case Attribute_NONE:
            break;
        case Attribute_AxisAttribute:
        {
            TosaAxisAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.axis()));
            break;
        }
        case Attribute_PoolAttribute:
        {
            TosaPoolAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.kernel()));
            attributeTensors.push_back(CreateTensor(attribute.stride()));
            attributeTensors.push_back(CreateTensor(attribute.pad()));

            if (operatorEnum == OperatorEnum::MaxPool2d)
            {
                break;
            }

            attributeTensors.push_back(CreateTensor((ConvertAvgPoolAccType(attribute))));
            attributeTensors.push_back(CreateTensor(attribute.input_zp(), inputTensors[0].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.output_zp(), outputTensors[0].GetDataType()));
            break;
        }
        case Attribute_ConvAttribute:
        {
            TosaConvAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.pad()));
            attributeTensors.push_back(CreateTensor(attribute.stride()));
            attributeTensors.push_back(CreateTensor(attribute.dilation()));
            attributeTensors.push_back(CreateTensor(attribute.input_zp(), inputTensors[0].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.weight_zp(), inputTensors[1].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.local_bound()));
            break;
        }
        case Attribute_TransposeConvAttribute:
        {
            TosaTransposeConvAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.out_pad()));
            attributeTensors.push_back(CreateTensor(attribute.stride()));
            attributeTensors.push_back(CreateTensor(attribute.output_shape()));
            attributeTensors.push_back(CreateTensor(attribute.input_zp(), inputTensors[0].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.weight_zp(), inputTensors[1].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.local_bound()));
            break;
        }
        case Attribute_PadAttribute:
        {
            TosaPadAttribute attribute = op->GetAttribute();

            TosaSerializationTensor* inputTosaTensor = m_Block->GetTensorByName(inputNames[0]);
            auto resId = graph->GetNextResId();
            auto paddingTensor = Tensor(DataType::int32_t,
                                        {static_cast<unsigned int>(inputTosaTensor->GetShape().size()), 2u},
                                        resId);

            AddGraphConstantToMap(paddingTensor,
                                  GraphConstant().emplace<std::vector<int32_t>>(attribute.padding()), "padding");
            attributeTensors.push_back(paddingTensor);
            attributeTensors.push_back(CreateTensor(attribute.pad_const_int(), inputTensors[0].GetDataType()));
            break;
        }
        case Attribute_ReshapeAttribute:
        {
            TosaReshapeAttribute attribute = op->GetAttribute();
            const auto reshapeShapeTensorValues = ConvertTensorShape(attribute.new_shape());
            attributeTensors.push_back(CreateTensor(reshapeShapeTensorValues));
            break;
        }
        case Attribute_SliceAttribute:
        {
            TosaSliceAttribute attribute = op->GetAttribute();
            const auto sliceStartTensorValues = ConvertTensorShape(attribute.start());
            auto startTensor = Tensor::CreateAttribute(DataType::int32_t,
            std::vector{static_cast<unsigned int>(attribute.size().size())},
            std::vector(sliceStartTensorValues));

            const auto sliceSizeTensorValues = ConvertTensorShape(attribute.size());
            auto sizeTensor = Tensor::CreateAttribute(DataType::int32_t,
            std::vector{static_cast<unsigned int>(attribute.size().size())},
            std::vector(sliceSizeTensorValues));

            attributeTensors.push_back(startTensor);
            attributeTensors.push_back(sizeTensor);
            break;
        }
        case Attribute_TileAttribute:
        {
            TosaTileAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.multiples()));
            break;
        }
        case Attribute_ResizeAttribute:
        {
            TosaResizeAttribute attribute = op->GetAttribute();

            const auto scaleTensorValues = Tensor::ConvertInt16tToUint32t(attribute.scale(), false);
            attributeTensors.push_back(CreateTensor(scaleTensorValues));
            const auto offsetTensorValues = Tensor::ConvertInt16tToUint32t(attribute.offset(), false);
            attributeTensors.push_back(CreateTensor(offsetTensorValues));
            const auto borderTensorValues = Tensor::ConvertInt16tToUint32t(attribute.border(), false);
            attributeTensors.push_back(CreateTensor(borderTensorValues));

            uint32_t mode = attribute.mode() == 1 ? 0 : 1;
            std::vector modeVector = {mode};
            attributeTensors.push_back(CreateTensor({modeVector}));
            break;
        }
        case Attribute_ClampAttribute:
        {
            TosaClampAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.min_int(), inputTensors[0].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.max_int(), inputTensors[0].GetDataType()));
            break;
        }
        case Attribute_RescaleAttribute:
        {
            TosaRescaleAttribute attribute = op->GetAttribute();

            auto resId = graph->GetNextResId();
            auto multiplierTensor = Tensor(DataType::int32_t,
                                           std::vector{static_cast<unsigned int>(attribute.multiplier().size())},
                                           resId);
            AddGraphConstantToMap(multiplierTensor,
                                  GraphConstant().emplace<std::vector<int32_t>>(attribute.multiplier()),
                                  "multiplier");

            resId = graph->GetNextResId();
            auto shiftTensor = Tensor(DataType::int8_t,
                                      std::vector{static_cast<unsigned int>(attribute.shift().size())},
                                      resId);

            std::vector<int8_t> int8Vector;
            std::vector<int32_t> int32Vector = attribute.shift();
            int8Vector.reserve(int32Vector.size());
            std::transform(int32Vector.begin(), int32Vector.end(), std::back_inserter(int8Vector),
                        [](int32_t value) { return static_cast<int8_t>(value); });
            AddGraphConstantToMap(shiftTensor, GraphConstant().emplace<std::vector<int8_t>>(int8Vector), "shift");

            attributeTensors.push_back(CreateTensor(attribute.input_zp(), inputTensors[0].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.output_zp(), outputTensors[0].GetDataType()));
            attributeTensors.push_back(multiplierTensor);
            attributeTensors.push_back(shiftTensor);
            attributeTensors.push_back(CreateTensor(attribute.scale32()));
            attributeTensors.push_back(CreateTensor(attribute.double_round()));
            attributeTensors.push_back(CreateTensor(attribute.per_channel()));
            attributeTensors.push_back(CreateTensor(attribute.input_unsigned()));
            attributeTensors.push_back(CreateTensor(attribute.output_unsigned()));
            break;
        }
        case Attribute_MulAttribute:
        {
            TosaMulAttribute attribute = op->GetAttribute();

            std::vector<int32_t> int32Vector = {attribute.shift()};
            std::vector<int8_t> int8Vector;
            int8Vector.reserve(int32Vector.size());

            std::transform(int32Vector.begin(), int32Vector.end(), std::back_inserter(int8Vector),
            [](int32_t value) { return static_cast<int8_t>(value); });
            auto shiftTensor = Tensor::CreateAttribute(
                DataType::int8_t,
                std::vector<unsigned int>{1},
                Tensor::ConvertInt8tToUint32t(int8Vector));
            attributeTensors.push_back(shiftTensor);

            break;
        }
        case Attribute_ArithmeticRightShiftAttribute:
        {
            TosaArithmeticRightShiftAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.round()));
            break;
        }
        case Attribute_TransposeAttribute:
        {
            TosaTransposeAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.perms()));
            break;
        }
        case Attribute_TableAttribute:
        {
            TosaTableAttribute attribute = op->GetAttribute();

            auto resId = graph->GetNextResId();
            DataType datatype = attribute.table().size() == 513 ? DataType::int16_t : DataType::int8_t;
            auto tableTensor = Tensor(datatype,
                                      std::vector{static_cast<unsigned int>(attribute.table().size())},
                                      resId);
            GraphConstant table;
            // if int8 we need to convert from int16 given by attribute
            if (inputTensors[0].GetDataType() == DataType::int8_t)
            {
                std::vector<int8_t> int8Vector;
                std::vector<int16_t> int16Vector = attribute.table();
                int8Vector.reserve(int16Vector.size());
                std::transform(int16Vector.begin(), int16Vector.end(), std::back_inserter(int8Vector),
                    [](int16_t value) { return static_cast<int8_t>(value); });
                table.emplace<std::vector<int8_t>>(int8Vector);
            }
            else
            {
                table.emplace<std::vector<int16_t>>(attribute.table());
            }
            AddGraphConstantToMap(tableTensor, table, "table");
            attributeTensors.push_back(tableTensor);
            break;
        }
        case Attribute_MatMulAttribute:
        {
            TosaMatMulAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.a_zp(), inputTensors[0].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.b_zp(), inputTensors[1].GetDataType()));
            break;
        }
        case Attribute_FullyConnectedAttribute:
        {
            TosaFullyConnectedAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.input_zp(), inputTensors[0].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.weight_zp(), inputTensors[1].GetDataType()));
            break;
        }
        case Attribute_NegateAttribute:
        {
            TosaNegateAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.input1_zp(), inputTensors[0].GetDataType()));
            attributeTensors.push_back(CreateTensor(attribute.output_zp(), inputTensors[0].GetDataType()));
            break;
        }
        case Attribute_RFFTAttribute:
        {
            break;
        }
        case Attribute_FFTAttribute:
        {
            TosaFFTAttribute attribute = op->GetAttribute();
            attributeTensors.push_back(CreateTensor(attribute.inverse()));
            attributeTensors.push_back(CreateTensor(attribute.local_bound()));
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

    if (op->GetOp() != Op_CONST)
    {
        auto resIds = graph->AddOperator(operatorEnum, inputTensors, outputTensors, attributeTensors);
        auto resId = resIds.begin();
        for (const auto& outputName : outputNames)
        {
            auto name = GetOutputName(outputName);
            m_OpNameResIdMap.try_emplace(name, *resId);
            ++resId;
        }
    }
}

}  // namespace graphbuilder