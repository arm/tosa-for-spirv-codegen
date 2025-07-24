//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <OpTestUtils.hpp>

#include <ValidationUtils.hpp>

namespace testutils
{

bool CheckModule(const std::shared_ptr<spirv::Module>& module,
                 const TOSAInstructions op,
                 const std::vector<tosa::Tensor>& inputs,
                 const std::vector<tosa::Tensor>& graphConstants,
                 const std::vector<tosa::Attribute>& tensorConstants,
                 const std::vector<tosa::Tensor>& outputs,
                 const std::vector<tosa::Attribute>& attributes)
{
    // Validating Module SPIR-V
    const auto binary = tosa2spirv::WriteToBinary(module);
    if (testutils::DisassembleSPIRV(binary, true) == "")
    {
        return false;
    }

    // Validating module content
    std::vector<tosa::Attribute> jointAttributes;
    if (!attributes.empty())
    {
        jointAttributes.insert(jointAttributes.end(), attributes.begin(), attributes.end());
    }
    jointAttributes.insert(jointAttributes.end(), tensorConstants.begin(), tensorConstants.end());

    const auto& opInstruction = module->GetInstructionsOfType(spv::Op::OpExtInst).first;

    EXPECT_TRUE(opInstruction != module->GetSpirvGraph().end() && opInstruction->m_Operands.size() >= 4);

    const auto& opCodeInstruction = opInstruction->m_Operands[3];
    EXPECT_TRUE(opCodeInstruction.m_Type == spirv::LITERAL_WORD &&
                static_cast<TOSAInstructions>(opCodeInstruction.m_LiteralWord) == op);

    EXPECT_EQ(opInstruction->m_Operands[0].m_Type, spirv::INSTRUCTION_POINTER);
    const auto& outputInstruction = opInstruction->m_Operands[0].m_InstructionPtr;

    EXPECT_TRUE(outputs.size() == 1 || outputs.size() == 2);
    if (outputs.size() == 1)
    {
        CheckTensorType(outputInstruction,
                        outputs[0].GetDataType(),
                        outputs[0].GetTensorShape().size(),
                        outputs[0].GetTensorShape());
    }
    else if (outputs.size() == 2)
    {
        EXPECT_EQ(outputInstruction->GetOpCode(), spv::Op::OpTypeStruct);
        const auto& output1 = outputInstruction->m_Operands[1];
        const auto& output2 = outputInstruction->m_Operands[2];
        EXPECT_TRUE(output1.m_Type == spirv::INSTRUCTION_POINTER && output2.m_Type == spirv::INSTRUCTION_POINTER);
        CheckTensorType(output1.m_InstructionPtr,
                        outputs[0].GetDataType(),
                        outputs[0].GetTensorShape().size(),
                        outputs[0].GetTensorShape());
        CheckTensorType(output2.m_InstructionPtr,
                        outputs[1].GetDataType(),
                        outputs[1].GetTensorShape().size(),
                        outputs[1].GetTensorShape());
    }

    size_t inputIdx = 0;
    size_t graphConstIdx = 0;
    size_t tensorConstIdx = 0;
    for (size_t opIdx = 4; opIdx < opInstruction->m_Operands.size(); ++opIdx)
    {
        EXPECT_EQ(opInstruction->m_Operands[opIdx].m_Type, spirv::INSTRUCTION_POINTER);
        const auto& currentInst = opInstruction->m_Operands[opIdx].m_InstructionPtr;

        if (currentInst->GetOpCode() == spv::Op::OpGraphInputARM || currentInst->GetOpCode() == spv::Op::OpExtInst)
        {
            const auto& inputInstruction = currentInst->m_Operands[0].m_InstructionPtr;
            const auto& currentInputTensor = inputs[inputIdx++];
            CheckTensorType(inputInstruction,
                            currentInputTensor.GetDataType(),
                            currentInputTensor.GetTensorShape().size(),
                            currentInputTensor.GetTensorShape());
        }
        else if (currentInst->GetOpCode() == spv::Op::OpGraphConstantARM)
        {
            const auto& graphConstInstruction = currentInst->m_Operands[0].m_InstructionPtr;
            const auto& currentGraphConstTensor = graphConstants[graphConstIdx++];
            CheckTensorType(graphConstInstruction,
                            currentGraphConstTensor.GetDataType(),
                            currentGraphConstTensor.GetTensorShape().size(),
                            currentGraphConstTensor.GetTensorShape());
        }
        else
        {
            const auto& currentTensorConst = jointAttributes[tensorConstIdx++];
            if (currentInst->GetOpCode() == spv::Op::OpConstant)
            {
                CheckConstant(
                    currentInst,
                    currentTensorConst.GetTensor().GetDataType(),
                    currentTensorConst.GetData()[0],
                    (currentTensorConst.GetTensor().GetDataType() == DataType::int48_t ? currentTensorConst.GetData()[1]
                                                                                       : std::optional<uint32_t>{}));
            }
            else if (currentInst->GetOpCode() == spv::Op::OpConstantTrue ||
                     currentInst->GetOpCode() == spv::Op::OpConstantFalse)
            {
                EXPECT_EQ(currentTensorConst.GetTensor().GetDataType(), DataType::bool_t);
                EXPECT_EQ(currentTensorConst.GetData().size(), 1);
                if (currentInst->GetOpCode() == spv::Op::OpConstantTrue)
                {
                    EXPECT_EQ(currentTensorConst.GetData()[0], 1);
                }
                else
                {
                    EXPECT_EQ(currentTensorConst.GetData()[0], 0);
                }
            }
            else if (currentInst->GetOpCode() == spv::Op::OpConstantComposite ||
                     currentInst->GetOpCode() == spv::Op::OpConstantCompositeReplicateEXT ||
                     currentInst->GetOpCode() == spv::Op::OpConstantNull)
            {
                CheckConstantComposite(currentInst,
                                       currentTensorConst.GetData(),
                                       currentTensorConst.GetTensor().GetDataType(),
                                       currentTensorConst.GetTensor().GetDataType());
            }
        }
    }
    return (inputIdx == inputs.size() && graphConstIdx == graphConstants.size() &&
            tensorConstIdx == jointAttributes.size());
}

bool CheckOperator(const tosa::OperatorEnum op,
                   const std::vector<tosa::Tensor>& inputs,
                   const std::vector<tosa::Tensor>& graphConstants,
                   const std::vector<tosa::Attribute>& tensorConstants,
                   const std::vector<tosa::Tensor>& outputs,
                   const std::vector<tosa::Attribute>& attributes)
{
    std::shared_ptr<spirv::Module> module = CreateModule(TOSAVersion::v1_0);
    tosa::Graph graph{module};

    std::vector<tosa::ResId> inputIds;
    for (size_t inputIdx = 0; inputIdx < inputs.size(); ++inputIdx)
    {
        const auto& inputId = graph.AddInput(inputs[inputIdx], inputIdx);
        inputIds.push_back(inputId);
    }
    for (const auto& graphConst : graphConstants)
    {
        const auto& graphConstId = graph.AddGraphConstant(graphConst);
        inputIds.push_back(graphConstId);
    }
    for (const auto& tensorConst : tensorConstants)
    {
        const auto& tensorId = graph.AddTensorConstant(tensorConst);
        inputIds.push_back(tensorId);
    }

    // Adding Operator to Module based on the given parameters
    const auto res = graph.AddOperator(op, inputIds, outputs, attributes)[0];
    graph.AddOutput(res, 0);
    graph.FinalizeGraph();

    // Running validation on generated SPIR-V Module
    const auto binary = tosa2spirv::WriteToBinary(module);
    if (testutils::DisassembleSPIRV(binary, true) == "")
    {
        return false;
    }

    // Verifying that generated SPIR-V Instruction in Graph is correct
    const auto& instruction = module->GetInstructionsOfType(spv::Op::OpExtInst).first;
    testutils::TosaOperator generatedOp = testutils::GetTosaOperator(*instruction);

    testutils::TosaOperator inputOp{op, inputs, graphConstants, tensorConstants, outputs, attributes};

    return generatedOp == inputOp;
}

} // namespace testutils
