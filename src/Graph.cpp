//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Graph.hpp>
#include <OperatorDefinitions.hpp>
#include <SPIRVDefinitions.hpp>

#include <algorithm>

namespace tfsc::tosa
{

using namespace spirv;
using namespace spv;
using namespace tfsc::spirv;

Graph::Graph(std::shared_ptr<Module> module, std::string name)
    : m_Name(std::move(name))
    , m_Module(std::move(module))
{
}

ResId Graph::AddInput(const Tensor& input, const unsigned int bindingId)
{
    const auto inputId = CreateConstant(m_Inputs.size(), DataType::uint32_t, *m_Module);

    auto tensor = CreateTensor(input, *m_Module);
    auto uniformConstantPtr =
        m_Module->EmplaceInstruction(OpTypePointer, {RESID, Operand{StorageClassUniformConstant}, tensor});

    const auto opVariable =
        m_Module->EmplaceInstructionNonUnique(OpVariable,
                                              {uniformConstantPtr, RESID, Operand{StorageClassUniformConstant}});

    m_Module->EmplaceInstruction(OpDecorate, {opVariable, Operand{DecorationDescriptorSet}, Operand{0u}});
    m_Module->EmplaceInstruction(OpDecorate, {opVariable, Operand{DecorationBinding}, Operand{bindingId}});

    m_Inputs.emplace_back(opVariable.m_InstructionPtr);
    return m_Module->EmplaceInstruction(OpGraphInputARM, {tensor, RESID, inputId}).m_InstructionPtr;
}

void Graph::AddOutput(const ResId outputTensor, const unsigned int bindingId)
{
    std::vector<Operand> outputsWithIds;
    const auto tensor = outputTensor->m_Operands[0];
    const auto outputIdOperand = CreateConstant(m_Outputs.size(), DataType::int32_t, *m_Module);

    m_Module->EmplaceInstruction(OpGraphSetOutputARM, {Operand{outputTensor}, outputIdOperand});
    auto uniformConstantPtr =
        m_Module->EmplaceInstruction(OpTypePointer, {RESID, Operand{StorageClassUniformConstant}, tensor});

    const auto opVariable =
        m_Module->EmplaceInstructionNonUnique(OpVariable,
                                              {uniformConstantPtr, RESID, Operand{StorageClassUniformConstant}});

    m_Module->EmplaceInstruction(OpDecorate, {opVariable, Operand{DecorationDescriptorSet}, Operand{0u}});
    m_Module->EmplaceInstruction(OpDecorate, {opVariable, Operand{DecorationBinding}, Operand{bindingId}});

    m_Outputs.emplace_back(opVariable.m_InstructionPtr);
}

ResId Graph::AddGraphConstant(const Tensor& tensor)
{
    const auto tensorOperand = CreateTensor(tensor, *m_Module);
    return m_Module->EmplaceInstruction(OpGraphConstantARM, {tensorOperand, RESID, Operand{m_GraphConstantId++}})
        .m_InstructionPtr;
}

ResId Graph::AddExternalGraphConstant(const Tensor& tensor)
{
    const auto tensorOperand = CreateTensor(tensor, *m_Module);
    return m_Module->EmplaceInstruction(OpGraphConstantARM, {tensorOperand, RESID, Operand{m_GraphConstantId++}})
        .m_InstructionPtr;
}

ResId Graph::AddTensorConstant(const Attribute& attribute)
{
    return CreateAttribute(attribute, *m_Module).m_InstructionPtr;
}

// Forward declaration of the static helper function ChainConcat.
static Operand ChainConcat(Module& spirvModule,
                           const std::vector<Operand>& operands,
                           const std::vector<unsigned int>& oShape,
                           const uint16_t& maxOperands);

std::vector<ResId> Graph::AddOperator(const OperatorEnum operatorType,
                                      const std::vector<ResId>& inputs,
                                      const std::vector<Tensor>& outputs,
                                      const std::vector<Attribute>& attributes) const
{
    std::vector<Operand> operands;
    operands.reserve(inputs.size() + outputs.size() + attributes.size() + 3);

    std::vector<Operand> postOperands;
    std::vector<Operand> outputTensorTypeOpVec;

    if (outputs.size() == 2)
    {
        outputTensorTypeOpVec.emplace_back(RESID);
        outputTensorTypeOpVec.emplace_back(CreateTensor(outputs[0], *m_Module));
        outputTensorTypeOpVec.emplace_back(CreateTensor(outputs[1], *m_Module));

        const auto outputTypeStructOp = m_Module->EmplaceInstruction(OpTypeStruct, outputTensorTypeOpVec);
        operands.push_back(outputTypeStructOp);
    }
    else
    {
        operands.emplace_back(CreateTensor(outputs[0], *m_Module));
    }

    operands.emplace_back(RESID);
    const auto tosaVersion = Operand{&(*m_Module->GetInstructionsOfType(OpExtInstImport).first)};
    operands.push_back(tosaVersion);
    const Operand operatorOperand(GetTOSAInstructions(operatorType));
    operands.push_back(operatorOperand);

    auto expectedIt = GetOperatorDefinition(operatorType).m_AttributeDefinitions;
    for (const auto& attribute : attributes)
    {
        if (expectedIt->m_Metadata == Metadata::Shape)
        {
            if (expectedIt->m_Category == Category::GraphConstant)
            {
                postOperands.emplace_back(attribute.GetResId());
            }
            else
            {
                postOperands.push_back(CreateAttribute(attribute, *m_Module));
            }
            expectedIt++;
            continue;
        }

        if (expectedIt->m_Category == Category::Scalar || expectedIt->m_Category == Category::Enum)
        {
            const auto dataType = attribute.GetTensor().GetDataType();
            if (dataType == DataType::int48_t)
            {
                operands.push_back(CreateConstantDouble(attribute.GetData()[0], attribute.GetData()[0], *m_Module));
            }
            else
            {
                operands.push_back(CreateConstant(attribute.GetData()[0], dataType, *m_Module));
            }
        }
        else if (expectedIt->m_Category == Category::GraphConstant)
        {
            operands.emplace_back(attribute.GetResId());
        }
        else
        {
            operands.push_back(CreateAttribute(attribute, *m_Module));
        }
        expectedIt++;
    }

    for (const auto& input : inputs)
    {
        operands.emplace_back(input);
    }

    operands.insert(std::end(operands), std::begin(postOperands), std::end(postOperands));

    Operand operatorResId;
    constexpr uint16_t maxSpirvOperands = 250;
    if (operatorType == OperatorEnum::Concat && operands.size() > maxSpirvOperands)
    {
        operatorResId = ChainConcat(*m_Module, operands, outputs[0].GetTensorShape(), maxSpirvOperands);
    }
    else
    {
        operatorResId = m_Module->EmplaceInstruction(OpExtInst, {operands});
    }

    if (outputs.size() == 2)
    {
        auto output0CompositeExtractInst0 =
            m_Module->EmplaceInstruction(OpCompositeExtract,
                                         {outputTensorTypeOpVec[1], RESID, operatorResId, Operand{0u}});
        auto output0CompositeExtractInst1 =
            m_Module->EmplaceInstruction(OpCompositeExtract,
                                         {outputTensorTypeOpVec[2], RESID, operatorResId, Operand{1u}});

        return {output0CompositeExtractInst0.m_InstructionPtr, output0CompositeExtractInst1.m_InstructionPtr};
    }

    return {operatorResId.m_InstructionPtr};
}

void Graph::FinalizeGraph()
{
    const auto IOSize = m_Inputs.size() + m_Outputs.size();

    std::vector<Operand> graphTypeOperands(2 + IOSize);
    std::vector<Operand> entryPointOperands(2 + IOSize);

    auto entryPointOperandsIOBegin = entryPointOperands.begin() + 2;
    auto graphTypeOperandsIOBegin = graphTypeOperands.begin() + 2;

    for (const auto& input : m_Inputs)
    {
        *entryPointOperandsIOBegin++ = Operand{input};
        const auto tensor = input->m_Operands[0].m_InstructionPtr->m_Operands[2];
        *graphTypeOperandsIOBegin++ = tensor;
    }
    for (const auto& output : m_Outputs)
    {
        *entryPointOperandsIOBegin++ = Operand{output};
        const auto tensor = output->m_Operands[0].m_InstructionPtr->m_Operands[2];
        *graphTypeOperandsIOBegin++ = tensor;
    }

    graphTypeOperands[0] = RESID;
    graphTypeOperands[1] = Operand{static_cast<uint32_t>(m_Inputs.size())};
    auto graphTypeInstruction = m_Module->EmplaceInstruction(OpTypeGraphARM, graphTypeOperands);
    const auto graphArm = m_Module->EmplaceInstruction(OpGraphARM, {graphTypeInstruction, RESID});

    entryPointOperands[0] = graphArm;
    entryPointOperands[1] = Operand{m_Name};

    m_Module->EmplaceInstruction(OpGraphEntryPointARM, entryPointOperands);
    m_Module->EmplaceInstruction(OpGraphEndARM, {});
    // Graph is complete no more instructions should be added.
    m_Module.reset();
}

/// Function made to handle Concats with > 255 Operands which is the limit.
static Operand ChainConcat(Module& spirvModule,
                           const std::vector<Operand>& operands,
                           const std::vector<unsigned int>& oShape,
                           const uint16_t& maxOperands)
{
    std::vector<unsigned int> shape = oShape;

    const Operand& version = operands[2];
    const Operand& opEnum = operands[3];
    const Operand& concatAxis = operands[4];

    // Store final output shape
    uint16_t outputAxisShape = shape.back();

    std::vector<Operand> concatInputs(operands.begin() + 4, operands.end());
    auto it = concatInputs.begin();
    auto end = concatInputs.end();

    shape.back() = maxOperands;
    Operand previousConcat;
    uint16_t isFirst = 1;

    // Following chunks so we can chain Concats together.
    for (int16_t i = -1; i < (outputAxisShape / maxOperands); ++i)
    {
        // First iteration needs a size of 2 more for the previous concat + concatAxis
        // If this is the first iteration value will be 251 otherwise 249.
        unsigned int allowed = (maxOperands - 1) + isFirst * 2;
        unsigned int count = std::min(allowed, static_cast<unsigned int>(end - it));

        shape.back() += count * (1 - isFirst);
        Tensor chunkTensor = Tensor(DataType::uint8_t, shape);
        Operand chunkType = CreateTensor(chunkTensor, spirvModule);

        // If not first iteration then concat the axis + previous concat
        std::vector<Operand> chunk{chunkType,
                                   RESID,
                                   version,
                                   opEnum,
                                   isFirst ? Operand{} : concatAxis,
                                   isFirst ? Operand{} : previousConcat};

        chunk.insert(chunk.end(), it, it + count);
        it += count;

        previousConcat = spirvModule.EmplaceInstruction(OpExtInst, chunk);
        isFirst = 0;
    }
    return previousConcat;
}

} // namespace tfsc::tosa
