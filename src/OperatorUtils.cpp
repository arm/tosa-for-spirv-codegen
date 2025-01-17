//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "OperatorUtils.hpp"

#include <algorithm>
#include <Operator.hpp>
#include <OperatorDefinitions.hpp>
#include <Writer.hpp>
#include <spirv-tools/libspirv.h>


namespace tosa2spirv::graphbuilder {


void AnnotateTensor(Tensor& tensor, const TensorDefinition& info)
{
    tensor.SetCategory(info.m_Category);
    tensor.SetMetadata(info.m_Metadata);
}

void ValidateAndAnnotateOp(Operator& op)
{
    OperatorDefinition expected = GetOperatorDefinition(op.m_OperatorEnum);
    auto operatorTensorCount = op.m_InputTensors.size() + op.m_OutputTensors.size() + op.m_AttributeTensors.size();
    bool hasTensorList = std::any_of(expected.begin(), expected.end(),
                             [](TensorDefinition tensorDef) { return tensorDef.m_Category == Category::TensorList; });
    //  We can't verify the number of operands of a TensorList
    if (!hasTensorList)
    {
        if (op.m_OperatorEnum != OperatorEnum::Concat && expected.size() != operatorTensorCount)
        {
            throw std::invalid_argument("Invalid operand size for operator:");
        }
    }

    auto expectedIT = expected.begin();

    for (auto& input: op.m_InputTensors)
    {
        AnnotateTensor(input, *expectedIT);
        if (expectedIT->m_Category != Category::TensorList)
        {
            ++expectedIT;
        }
    }

    // TensorLists are homogenous so we only have one operator definition for them
    if (expectedIT->m_Category == Category::TensorList)
    {
        ++expectedIT;
    }

    for (auto& attribute: op.m_AttributeTensors)
    {
        AnnotateTensor(attribute, *expectedIT);
        ++expectedIT;
    }

    for (auto& output: op.m_OutputTensors)
    {
        AnnotateTensor(output, *expectedIT);
        ++expectedIT;
    }
}

void SetBindings(const Tensor& tensor, uint32_t pointerTypeId, spirvwriter::Writer& writer)
{
    // If tensor has a DescriptorSet Binding, handle it
    if (tensor.GetDescSetBinding())
    {
        // If tensor has a DescriptorSet Binding and it hasnt had a pointer assigned do so.
        if (!tensor.GetDescSetBinding()->GetAssignedPointerId())
        {
            // User has not defined the DescriptorSet dataType and Shape then infer.
            if (!tensor.GetDescSetBinding()->GetDataType().has_value() ||
                !tensor.GetDescSetBinding()->GetTensorShape().has_value())
            {
                tensor.GetDescSetBinding()->SetDataType(tensor.GetDataType());
                tensor.GetDescSetBinding()->SetTensorShape(tensor.GetTensorShape());
            }
            // Check for dataType or Shape mismatch
            else if (tensor.GetDescSetBinding()->GetDataType() != tensor.GetDataType() &&
                     tensor.GetDescSetBinding()->GetTensorShape() != tensor.GetTensorShape())
            {
                throw std::invalid_argument(
                        "Layer::SetBindings: Tensors has an assigned DescriptorSet Bindings with mismatching Shape/DataType");
            }

            // Assign the pointer type to the DescSet binding
            auto descSetBinding = tensor.GetDescSetBinding();
            descSetBinding->SetAssignedPointerId(pointerTypeId);

            // Then add a descriptor for the DescriptorSet Binding now that we know it's type
            writer.WriteDecoration(spv::Decoration::DecorationDescriptorSet,
                                    descSetBinding->GetDescriptorSet(),
                                    pointerTypeId);
            writer.WriteDecoration(spv::Decoration::DecorationBinding,
                                    descSetBinding->GetBinding(),
                                    pointerTypeId);

        }
        else
        {
            // if the pointer type is already assigned to DescriptorSet Binding then check it's the same
            // if not throw
            if (tensor.GetDescSetBinding()->GetAssignedPointerId() != pointerTypeId)
            {
                throw std::invalid_argument(
                        "Layer::SetBindings: Tensors with matching DescriptorSet Bindings have different pointer types.");
            }
        }
    }
    else
    {
        throw std::invalid_argument(
                "Layer::SetBindings: Tensor used as input/output to Module has no DescriptorSet Binding Assigned.");
    }
}

/// Helper function to handle the Graph input for Layer::Write() method
spirvwriter::Operand HandleGraphInput(spirvwriter::Writer& writer,
                                      const ResId& layerId,
                                      GraphInfo& graphInfo,
                                      const Tensor& inputTensor,
                                      spirvwriter::InstructionList& layerInstList,
                                      const std::string& prefix)
{
    using namespace spirvwriter;

    if (!inputTensor.GetDescSetBinding())
    {
        throw std::invalid_argument(
                "Layer::SetBindings: Tensor used as input/output to Module has no DescriptorSet Binding Assigned.");
    }
    // Create the Pointer Type and get an operand containing its ID.
    // if no pointer assigned create one and assign
    if (inputTensor.GetDescSetBinding()->GetAssignedPointerId() == 0)
    {
        auto inputTensorTypeOp = writer.WriteTensorInstruction(inputTensor);
        auto inputPointerOp    = writer.AddPointerInstruction(layerInstList, inputTensor);
        auto inputPointerTypeId = inputPointerOp.GetNumId();

        // If tensor has a DescriptorSet Binding and it hasnt had a pointer assigned do so.
        SetBindings(inputTensor, inputPointerTypeId, writer);

        // Create a Graph Input using the input Tensor Type
        auto inputIndexOp = writer.WriteGraphInputOutputIndexConstant(graphInfo.ReturnAndIncrementIndex());
        std::vector<Operand> graphInputOps{inputTensorTypeOp, inputIndexOp};

        // Create a GraphInput
        Instruction graphInputInst(spv::Op::OpGraphInputARM, writer.GetNextId(), graphInputOps);
        // Add it to a list of inputs that will be written to binary when graph definition begins
        graphInfo.m_GraphInputList.push_back(graphInputInst);

        // Add the graph input as an operand to the Layers OpExtInst instruction
        Operand graphInputIdOp(OperandType::NUMBER_ID, graphInputInst.GetResultId().Get());

        // Assign the GraphInputId to the DescSetBinding, so it can be reused again if needed
        inputTensor.GetDescSetBinding()->SetAssignedGraphInputId(graphInputIdOp.GetNumId());

        // Layer ID as an Operand
        Operand inputOp(OperandType::NUMBER_ID, layerId.Get());

        // Add the ResId of the input type for use later in OpTypeGraphARM instruction during Graph::Write
        graphInfo.m_GraphInputsInfo.emplace_back(inputPointerOp, inputTensorTypeOp, inputOp);

        return graphInputIdOp;
    }
    else
    {
        // If pointer has already been assigned reuse the GraphInput for the layers inputs
        Operand graphInputIdOp(OperandType::NUMBER_ID, inputTensor.GetDescSetBinding()->GetAssignedGraphInputId());
        return graphInputIdOp;
    }
}

/// Helper function to handle the Graph output for Layer:Write() method
void HandleGraphOutput(spirvwriter::Writer& writer,
                       const ResId& layerId,
                       GraphInfo& graphInfo,
                       const Tensor& outputTensor,
                       spirvwriter::Operand& outputTensorTypeOp,
                       spirvwriter::InstructionList& layerInstList,
                       const std::string& prefix)
{
    using namespace spirvwriter;

    // Create the Pointer Type and get an operand containing its ID.
    auto outputPointerOp = writer.AddPointerInstruction(layerInstList, outputTensor);

    // If tensor has a DescriptorSet Binding and it hasnt had a pointer assigned then do so.
    SetBindings(outputTensor, outputPointerOp.GetNumId(), writer);

    // Layer ID as an operand
    Operand outputOp(OperandType::NUMBER_ID, layerId.Get());

    // Add the ResId of the output type
    graphInfo.m_GraphOutputsInfo.emplace_back(outputPointerOp, outputTensorTypeOp, outputOp);
}

spirvwriter::Operand HandleAttribute(spirvwriter::Writer& writer, const Tensor& outputTensor)
{
    if (outputTensor.GetResId().Get() != 0)
    {
        return {spirvwriter::OperandType::NUMBER_ID, outputTensor.GetResId().Get()};
    }
    if ((outputTensor.GetCategory() == Category::Scalar ||
        outputTensor.GetCategory() == Category::Enum) &&
        outputTensor.GetMetadata() != Metadata::Shape)
    {
        return writer.WriteConstantInstruction(outputTensor);
    }
    return writer.WriteTensorCompoundInstruction(outputTensor);
}

spirvwriter::Operand AddLayerInputInstruction(spirvwriter::Writer& writer,
                                              GraphInfo& graphInfo,
                                              const Tensor& inputTensor,
                                              spirvwriter::InstructionList& layerInstList,
                                              const std::string& prefix,
                                              ResId layerId)
{
    using namespace spirvwriter;
    if (inputTensor.GetResId().Get() == 0)
    {
        return HandleGraphInput(writer,
                                layerId,
                                graphInfo,
                                inputTensor,
                                layerInstList,
                                prefix);
    }

    // If the ResId of the input tensor is not 0 then it's coming from a previously initialized layer
    return {spirvwriter::OperandType::NUMBER_ID, inputTensor.GetResId().Get()};
}

TOSAInstructions GetTOSAInstructions(OperatorEnum operatorType)
{
    switch (operatorType)
    {
        // See tosa2spirv/python/source_generator.py and README
        // THIS SECTION IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
        case OperatorEnum::Argmax:  return TOSAARGMAX;
        case OperatorEnum::AvgPool2d:  return TOSAAVG_POOL2D;
        case OperatorEnum::Conv2d:  return TOSACONV2D;
        case OperatorEnum::Conv3d:  return TOSACONV3D;
        case OperatorEnum::DepthwiseConv2d:  return TOSADEPTHWISE_CONV2D;
        case OperatorEnum::Fft2d:  return TOSAFFT2D;
        case OperatorEnum::FullyConnected:  return TOSAFULLY_CONNECTED;
        case OperatorEnum::Matmul:  return TOSAMATMUL;
        case OperatorEnum::MaxPool2d:  return TOSAMAX_POOL2D;
        case OperatorEnum::Rfft2d:  return TOSARFFT2D;
        case OperatorEnum::TransposeConv2d:  return TOSATRANSPOSE_CONV2D;
        case OperatorEnum::Clamp:  return TOSACLAMP;
        case OperatorEnum::Erf:  return TOSAERF;
        case OperatorEnum::Sigmoid:  return TOSASIGMOID;
        case OperatorEnum::Tanh:  return TOSATANH;
        case OperatorEnum::Add:  return TOSAADD;
        case OperatorEnum::ArithmeticRightShift:  return TOSAARITHMETIC_RIGHT_SHIFT;
        case OperatorEnum::BitwiseAnd:  return TOSABITWISE_AND;
        case OperatorEnum::BitwiseOr:  return TOSABITWISE_OR;
        case OperatorEnum::BitwiseXor:  return TOSABITWISE_XOR;
        case OperatorEnum::Intdiv:  return TOSAINTDIV;
        case OperatorEnum::LogicalAnd:  return TOSALOGICAL_AND;
        case OperatorEnum::LogicalLeftShift:  return TOSALOGICAL_LEFT_SHIFT;
        case OperatorEnum::LogicalRightShift:  return TOSALOGICAL_RIGHT_SHIFT;
        case OperatorEnum::LogicalOr:  return TOSALOGICAL_OR;
        case OperatorEnum::LogicalXor:  return TOSALOGICAL_XOR;
        case OperatorEnum::Maximum:  return TOSAMAXIMUM;
        case OperatorEnum::Minimum:  return TOSAMINIMUM;
        case OperatorEnum::Mul:  return TOSAMUL;
        case OperatorEnum::Pow:  return TOSAPOW;
        case OperatorEnum::Sub:  return TOSASUB;
        case OperatorEnum::Table:  return TOSATABLE;
        case OperatorEnum::Abs:  return TOSAABS;
        case OperatorEnum::BitwiseNot:  return TOSABITWISE_NOT;
        case OperatorEnum::Ceil:  return TOSACEIL;
        case OperatorEnum::Clz:  return TOSACLZ;
        case OperatorEnum::Exp:  return TOSAEXP;
        case OperatorEnum::Floor:  return TOSAFLOOR;
        case OperatorEnum::Log:  return TOSALOG;
        case OperatorEnum::LogicalNot:  return TOSALOGICAL_NOT;
        case OperatorEnum::Negate:  return TOSANEGATE;
        case OperatorEnum::Reciprocal:  return TOSARECIPROCAL;
        case OperatorEnum::Rsqrt:  return TOSARSQRT;
        case OperatorEnum::Select:  return TOSASELECT;
        case OperatorEnum::Equal:  return TOSAEQUAL;
        case OperatorEnum::Greater:  return TOSAGREATER;
        case OperatorEnum::GreaterEqual:  return TOSAGREATER_EQUAL;
        case OperatorEnum::ReduceAll:  return TOSAREDUCE_ALL;
        case OperatorEnum::ReduceAny:  return TOSAREDUCE_ANY;
        case OperatorEnum::ReduceMax:  return TOSAREDUCE_MAX;
        case OperatorEnum::ReduceMin:  return TOSAREDUCE_MIN;
        case OperatorEnum::ReduceProduct:  return TOSAREDUCE_PRODUCT;
        case OperatorEnum::ReduceSum:  return TOSAREDUCE_SUM;
        case OperatorEnum::Concat:  return TOSACONCAT;
        case OperatorEnum::Pad:  return TOSAPAD;
        case OperatorEnum::Reshape:  return TOSARESHAPE;
        case OperatorEnum::Reverse:  return TOSAREVERSE;
        case OperatorEnum::Slice:  return TOSASLICE;
        case OperatorEnum::Tile:  return TOSATILE;
        case OperatorEnum::Transpose:  return TOSATRANSPOSE;
        case OperatorEnum::Gather:  return TOSAGATHER;
        case OperatorEnum::Scatter:  return TOSASCATTER;
        case OperatorEnum::Resize:  return TOSARESIZE;
        case OperatorEnum::Cast:  return TOSACAST;
        case OperatorEnum::Rescale:  return TOSARESCALE;
        default: throw std::invalid_argument("Invalid OperatorEnum");
    }

    //                           ==================  Generated Code End ==================
}
}// namespace graphbuilder::tosa2spirv
