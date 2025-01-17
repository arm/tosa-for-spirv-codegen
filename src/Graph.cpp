//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Graph.hpp>
#include <Writer.hpp>
#include <OperatorDefinitions.hpp>
#include "OperatorUtils.hpp"

#include <unordered_set>

namespace tosa2spirv
{

namespace graphbuilder
{

std::vector<ResId> Graph::AddOperator(OperatorEnum operatorType,
                                      const std::vector<Tensor>& inputs,
                                      const std::vector<Tensor>& outputs,
                                      const std::vector<Tensor> attributes)
{
    std::vector<ResId> resIds;
    for (int i = 0; i < outputs.size(); ++i)
    {
       resIds.push_back(m_IdGenerator.GetNextId());
    }
    m_Operators.emplace_back(operatorType, resIds, inputs, outputs, attributes);

    ValidateAndAnnotateOp(m_Operators.back());
    return resIds;
}

void WriteOperatorTwoOutputs(const Operator& op, spirvwriter::Writer& writer, const ResId& tosaId, GraphInfo& graphInfo)
{
    using namespace spirvwriter;

    std::vector<Operand> operands;
    std::vector<Operand> postOperands;
    InstructionList layerInstList;
    auto operatorId = writer.GetNextId();
    auto outputOperand0 = writer.WriteTensorInstruction(op.m_OutputTensors[0]);
    auto outputOperand1 = writer.WriteTensorInstruction(op.m_OutputTensors[1]);
    std::vector outputTensorTypeOpVec{outputOperand0, outputOperand1};

    auto output0CompositeExtractInst = writer.WriteCompositeExtractInstruction(operatorId,
                                                                               op.m_ResIds[0],
                                                                               outputOperand0,
                                                                               0);
    auto output0CompositeExtractId = output0CompositeExtractInst.GetResultId().Get();
    auto output1CompositeExtractInst = writer.WriteCompositeExtractInstruction(operatorId,
                                                                               op.m_ResIds[1],
                                                                               outputOperand1,
                                                                               1);
    auto output1CompositeExtractId = output1CompositeExtractInst.GetResultId().Get();
    auto outputTypeStructOp = writer.WriteTypeStructInstruction(outputTensorTypeOpVec);

    ResId operatorResId = outputTypeStructOp.GetNumId();
    operands.push_back(outputTypeStructOp);

    // Create an operand with the ID of the Extended Instruction Set Import of TOSA
    Operand tosaOp(OperandType::NUMBER_ID, tosaId.Get());
    // Create an operand containing the TOSA Instruction for the given operator from TOSA.h
    TOSAInstructions tosaEnum = GetTOSAInstructions(op.m_OperatorEnum);
    Operand operatorOperand(OperandType::NUMBER_ID, tosaEnum);
    operands.push_back(tosaOp);
    operands.push_back(operatorOperand);

    for (const auto& attributeTensor : op.m_AttributeTensors)
    {
        if (attributeTensor.GetMetadata() == Metadata::Shape)
        {
            postOperands.push_back(HandleAttribute(writer, attributeTensor));
        }
        else
        {
            operands.push_back(HandleAttribute(writer, attributeTensor));
        }
    }

    std::string name = GetOperatorName(op.m_OperatorEnum) + "_" + std::to_string(op.m_ResIds[0].Get());
    for (const auto& inputTensor : op.m_InputTensors)
    {
        operands.push_back(AddLayerInputInstruction(writer, graphInfo, inputTensor, layerInstList, name, op.m_ResIds[0]));
    }

    operands.insert(operands.end(), postOperands.begin(), postOperands.end());

    if (op.m_OutputTensors[0].IsGraphOutput())
    {
        HandleGraphOutput(writer,
                          op.m_ResIds[0],
                          graphInfo,
                          op.m_OutputTensors[0],
                          outputOperand0,
                          layerInstList,
                          name);
    }
    if (op.m_OutputTensors[1].IsGraphOutput())
    {
        HandleGraphOutput(writer,
                          op.m_ResIds[1],
                          graphInfo,
                          op.m_OutputTensors[1],
                          outputOperand1,
                          layerInstList,
                          name);
    }

    // Add the OpExtInst instruction using the ID from Layers member variables as well as the rest of the Ops.
    graphInfo.m_LayerList.push_back(std::move(Instruction(OpExtInst, operatorId, operands)));
    graphInfo.m_LayerList.push_back(std::move(output0CompositeExtractInst));
    graphInfo.m_LayerList.push_back(std::move(output1CompositeExtractInst));
    // Add the OpExtInst instruction using the ID from Layers member variables as well as the rest of the Ops.
    // Write to binary
    writer.WriteBinary(layerInstList);
}

void WriteOperator(const Operator& op, spirvwriter::Writer& writer, const ResId& tosaId, GraphInfo& graphInfo)
{
    using namespace spirvwriter;

    std::vector<Operand> operands;
    std::vector<Operand> postOperands;
    InstructionList layerInstList;

    ResId operatorResId = op.m_ResIds[0];
    auto outputOperand = writer.WriteTensorInstruction(op.m_OutputTensors[0]);
    operands.push_back(outputOperand);

    // Create an operand with the ID of the Extended Instruction Set Import of TOSA
    Operand tosaOp(OperandType::NUMBER_ID, tosaId.Get());
    // Create an operand containing the TOSA Instruction for the given operator from TOSA.h
    TOSAInstructions tosaEnum = GetTOSAInstructions(op.m_OperatorEnum);
    Operand operatorOperand(OperandType::NUMBER_ID, tosaEnum);
    operands.push_back(tosaOp);
    operands.push_back(operatorOperand);

    for (const auto& attributeTensor : op.m_AttributeTensors)
    {
        if (attributeTensor.GetMetadata() == Metadata::Shape)
        {
            postOperands.push_back(HandleAttribute(writer, attributeTensor));
        }
        else
        {
            operands.push_back(HandleAttribute(writer, attributeTensor));
        }
    }

    std::string name = GetOperatorName(op.m_OperatorEnum) + "_" + std::to_string(op.m_ResIds[0].Get());
    for (const auto& inputTensor : op.m_InputTensors)
    {
        operands.push_back(AddLayerInputInstruction(writer, graphInfo, inputTensor, layerInstList, name, op.m_ResIds[0]));
    }

    operands.insert(operands.end(), postOperands.begin(), postOperands.end());

    // Check if the layer is an output, if so add OpGraphOutputEXT instruction.
    if (op.m_OutputTensors[0].IsGraphOutput())
    {
        HandleGraphOutput(writer,
                          op.m_ResIds[0],
                          graphInfo,
                          op.m_OutputTensors[0],
                          operands[0],
                          layerInstList,
                          name);
    }

    // Add the OpExtInst instruction using the ID from Layers member variables as well as the rest of the Ops.
    graphInfo.m_LayerList.push_back(std::move(Instruction(OpExtInst, operatorResId, operands)));
    // Write to binary
    writer.WriteBinary(layerInstList);
}

void Graph::Write(spirvwriter::Writer& writer, const ResId& tosaId)
{
    GraphInfo graphInfo;
    for (auto& op : m_Operators)
    {
        if (op.m_OutputTensors.size() == 2)
        {
            WriteOperatorTwoOutputs(op, writer, tosaId, graphInfo);
        }
        else
        {
            WriteOperator(op, writer, tosaId, graphInfo);
        }
    }

    // Note: Graphs can have no inputs so definition can be started even if graphInputs.empty()
    // Instead they can have constants as inputs in certain tests.
    auto graphInputs = graphInfo.m_GraphInputsInfo;
    auto graphOutputs = graphInfo.m_GraphOutputsInfo;

    // Create Constants for input/output index to make sure it is written before OpGraphEntryPointARM
    int32_t maxIndex = std::max(graphInputs.size(), graphOutputs.size());
    for (int32_t index = 0; index < maxIndex; ++index)
    {
        writer.WriteGraphInputOutputIndexConstant(index);
    }

    // Add OpTypeGraphARM
    auto graphTypeOp = writer.WriteGraphTypeInstruction(graphInputs.size(), graphInputs, graphOutputs);

    // Add OpGraphEntryPointARM
    writer.WriteGraphEntryPointInstruction(m_GraphId, m_Name, graphInputs, graphOutputs);

    // Add OpGraphARM
    writer.WriteGraphInstruction(m_GraphId, graphTypeOp);

    if (!graphInputs.empty())
    {
        // Write the graphInputs now that the graph definition has begun
        writer.WriteBinary(graphInfo.m_GraphInputList);
    }

    if (!graphOutputs.empty())
    {
        spirvwriter::InstructionList outputInstList;
        writer.WriteBinary(outputInstList);
    }

    writer.WriteBinary(graphInfo.m_LayerList);

    // Reorder graph outputs
    ReorderGraphOutputs(graphOutputs);

    // Add OpGraphSetOutputARM
    writer.WriteGraphSetOutputInstruction(graphOutputs);

    // End the graph
    writer.WriteGraphEndInstruction();
}

void Graph::SetGraphOutputs(const std::vector<ResId>& graphOutputs)
{
    m_GraphOutputs = graphOutputs;
}

void Graph::ReorderGraphOutputs(std::vector<GraphIOInfo>& graphOutputs)
{
    if (!m_GraphOutputs.empty())
    {
        unsigned long n = m_GraphOutputs.size();
        if (n != graphOutputs.size())
        {
            throw std::invalid_argument("ReorderGraphOutputs(): Overridden GraphOutput ID count "
                                        "does not match number of GraphOutputs found in the graph.");
        }
        {
            std::unordered_set<uint32_t> unorderedIds, orderedIds;
            for (int i = 0; i != n; ++i)
            {
                unorderedIds.emplace(graphOutputs[i].m_Layer.GetNumId());
                orderedIds.emplace(m_GraphOutputs[i].Get());
            }
            if (unorderedIds != orderedIds)
            {
                throw std::invalid_argument("ReorderGraphOutputs(): ResId values for graph outputs do not match.");
            }
        }
        for (int i = 0; i != n; ++i)
        {
            graphOutputs[i].m_Layer = spirvwriter::Operand(spirvwriter::NUMBER_ID, m_GraphOutputs[i].Get());
        }
    }
}

//
// Data-node
//

void Graph::Accept(IVisitor &visitor) const
{
    visitor.Visit(*this);
}

} // namespace graphbuilder

} // namespace tosa2spirv