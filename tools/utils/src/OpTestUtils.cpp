//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "Tensor.hpp"
#include <OpTestUtils.hpp>

#include <ValidationUtils.hpp>

namespace testutils
{

void CheckAttributeOperand(const spirv::Instruction* currentInst, const tosa::Attribute& attribute)
{
    if (currentInst->GetOpCode() == spv::Op::OpConstant)
    {
        CheckConstant(currentInst,
                      attribute.GetTensor().GetDataType(),
                      attribute.GetData()[0],
                      (attribute.GetTensor().GetDataType() == DataType::int48_t ? attribute.GetData()[1]
                                                                                : std::optional<uint32_t>{}));
    }
    else if (currentInst->GetOpCode() == spv::Op::OpConstantTrue ||
             currentInst->GetOpCode() == spv::Op::OpConstantFalse)
    {
        EXPECT_EQ(attribute.GetTensor().GetDataType(), DataType::bool_t);
        EXPECT_EQ(attribute.GetData().size(), 1);
        if (currentInst->GetOpCode() == spv::Op::OpConstantTrue)
        {
            EXPECT_EQ(attribute.GetData()[0], 1);
        }
        else
        {
            EXPECT_EQ(attribute.GetData()[0], 0);
        }
    }
    else if (currentInst->GetOpCode() == spv::Op::OpConstantComposite ||
             currentInst->GetOpCode() == spv::Op::OpConstantCompositeReplicateEXT ||
             currentInst->GetOpCode() == spv::Op::OpConstantNull)
    {
        CheckConstantComposite(currentInst,
                               attribute.GetData(),
                               attribute.GetTensor().GetDataType(),
                               attribute.GetTensor().GetDataType());
    }
}

void CheckModule(const std::shared_ptr<spirv::Module>& module,
                 const TOSAInstructions op,
                 const std::vector<tosa::Attribute>& inputs,
                 const std::vector<tosa::Tensor>& outputs,
                 const std::vector<tosa::Attribute>& attributes)
{
    // Validating Module SPIR-V
    const auto binary = tfsc::WriteToBinary(module);
    EXPECT_FALSE(testutils::DisassembleSPIRV(binary, true) == "");

    // Validating module content
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

    size_t opIdx = 4;
    EXPECT_EQ(opInstruction->m_Operands.size() - 4, (inputs.size() + attributes.size()));
    for (const auto& attribute : attributes)
    {
        EXPECT_EQ(opInstruction->m_Operands[opIdx].m_Type, spirv::INSTRUCTION_POINTER);
        const auto& currentInst = opInstruction->m_Operands[opIdx++].m_InstructionPtr;

        CheckAttributeOperand(currentInst, attribute);
    }
    for (const auto& input : inputs)
    {
        EXPECT_EQ(opInstruction->m_Operands[opIdx].m_Type, spirv::INSTRUCTION_POINTER);
        const auto& currentInst = opInstruction->m_Operands[opIdx++].m_InstructionPtr;

        if (currentInst->GetOpCode() == spv::Op::OpGraphInputARM || currentInst->GetOpCode() == spv::Op::OpExtInst ||
            currentInst->GetOpCode() == spv::Op::OpGraphConstantARM)
        {
            EXPECT_TRUE(currentInst->m_Operands.size() > 0 &&
                        currentInst->m_Operands[0].m_Type == spirv::INSTRUCTION_POINTER);
            const auto& inputInstruction = currentInst->m_Operands[0].m_InstructionPtr;
            CheckTensorType(inputInstruction,
                            input.GetTensor().GetDataType(),
                            input.GetTensor().GetTensorShape().size(),
                            input.GetTensor().GetTensorShape());
        }
        else
        {
            CheckAttributeOperand(currentInst, input);
        }
    }
    EXPECT_EQ(opIdx, opInstruction->m_Operands.size());
}

} // namespace testutils
