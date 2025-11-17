//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Module.hpp>
#include <gtest/gtest.h>

namespace tfsc::spirv
{

TEST(TOSA_FOR_SPIRV_CODEGEN, Instruction)
{
    // Test that we can create an Instruction object and test its methods.
    Operand operand1(spv::Capability::CapabilityVulkanMemoryModel);
    Operand operand2(spv::Capability::CapabilityShader);
    std::vector<Operand> operands{operand1, operand2};

    Instruction instruction1(spv::Op::OpCapability, operands);
    EXPECT_EQ(instruction1.GetOpCode(), 17);
    EXPECT_EQ(instruction1.m_Operands.size(), 2);
    EXPECT_EQ(instruction1.m_WordCount, 3);
}

// Test Instruction Constructor
TEST(InstructionTests, Constructor)
{
    Instruction instruction{spv::OpTypeInt, {Operand{1}, Operand{"Test"}}};

    EXPECT_EQ(instruction.GetOpCode(), spv::OpTypeInt);
    EXPECT_EQ(instruction.m_Operands.size(), 2);
    EXPECT_EQ(instruction.m_WordCount, 1 + Operand(1).WordSize() + Operand("Test").WordSize());
}

// Test Comparator - Different Opcodes
TEST(InstructionTests, CompareDifferentOpcodes)
{
    Instruction inst1{spv::OpTypeInt, {Operand{0u}}};
    Instruction inst2{spv::OpTypeFloat, {Operand{0u}}};

    InstructionComparator comparator;
    EXPECT_TRUE(comparator(inst1, inst2));
    EXPECT_FALSE(comparator(inst2, inst1));
}

// Test InstructionComparator - Same Opcode, Different Operand Sizes
TEST(InstructionTests, CompareDifferentOperandSizes)
{
    Instruction inst1{spv::OpTypeInt, {Operand{1}}};
    Instruction inst2{spv::OpTypeInt, {Operand{1}, Operand{2}}};

    InstructionComparator comparator;

    // Smaller operand count should be first
    EXPECT_TRUE(comparator(inst1, inst2));
    EXPECT_FALSE(comparator(inst2, inst1));
}

// Test InstructionComparator - Operand Comparison With ResID
TEST(InstructionTests, CompareOperandsWithResID)
{
    Instruction inst1{spv::OpTypeInt, {Operand{1}, Operand{2, RES_ID}}};
    Instruction inst2{spv::OpTypeInt, {Operand{1}, Operand{3, RES_ID}}};

    InstructionComparator comparator;
    EXPECT_FALSE(comparator(inst1, inst2));
}

// Test InstructionComparator - Instruction Pointer Comparison
TEST(InstructionTests, CompareInstructionPointers)
{
    Instruction instruction1{spv::OpTypeInt, {Operand{0u}}};
    Instruction instruction2{spv::OpTypeInt, {Operand{1}}};

    Operand instrOp1(&instruction1);
    Operand instrOp2(&instruction2);

    Instruction inst1{spv::OpTypeInt, {instrOp1}};
    Instruction inst2{spv::OpTypeInt, {instrOp2}};

    InstructionComparator comparator;
    EXPECT_EQ(comparator(inst1, inst2), instrOp1.m_InstructionPtr < instrOp2.m_InstructionPtr);
}

// Test InstructionComparator - Equal Instructions
TEST(InstructionTests, CompareEqualInstructions)
{
    Instruction inst1{spv::OpTypeInt, {Operand{1}, Operand{2}}};
    Instruction inst2{spv::OpTypeInt, {Operand{1}, Operand{2}}};

    InstructionComparator comparator;
    EXPECT_FALSE(comparator(inst1, inst2));
    EXPECT_FALSE(comparator(inst2, inst1));
}

// Test EmplaceInstruction - Different Instructions
TEST(ModuleTests, EmplaceUniqueInstruction)
{
    Module module;
    Operand op1 = module.EmplaceInstruction(spv::OpTypeInt, {Operand{1}});
    Operand op2 = module.EmplaceInstruction(spv::OpTypeFloat, {Operand{1}});

    EXPECT_NE(op1.m_InstructionPtr, nullptr);
    EXPECT_NE(op2.m_InstructionPtr, nullptr);
    EXPECT_NE(op1, op2);
}

// Test EmplaceInstruction - Handles Duplicate Instructions
TEST(ModuleTests, EmplaceDuplicateInstruction)
{
    Module module;
    Operand op1 = module.EmplaceInstruction(spv::OpTypeInt, {Operand{1}});
    Operand op2 = module.EmplaceInstruction(spv::OpTypeInt, {Operand{1}});

    EXPECT_NE(op1.m_InstructionPtr, nullptr);
    EXPECT_EQ(op1, op2);
}

/// Module Tests
// Test EmplaceInstruction - OpVariable Case
TEST(ModuleTests, EmplaceOpVariable)
{
    Module module;

    Operand op1 = module.EmplaceInstruction(spv::OpVariable, {Operand{1u}});
    Operand op2 = module.EmplaceInstruction(spv::OpVariable, {Operand{1u}});

    EXPECT_NE(op1.m_InstructionPtr, nullptr);
    EXPECT_NE(op2.m_InstructionPtr, nullptr);
    EXPECT_EQ(op1, op2);
}

// Test GetInstructionsOfType - Fetching Instructions
TEST(ModuleTests, GetInstructionsOfType)
{
    Module module;

    module.EmplaceInstruction(spv::OpTypeInt, {Operand{1}});
    module.EmplaceInstruction(spv::OpTypeInt, {Operand{2}});
    module.EmplaceInstruction(spv::OpTypeFloat, {Operand{3}});

    auto range = module.GetInstructionsOfType(spv::OpTypeInt);
    // At least one should exist
    EXPECT_NE(range.first, range.second);

    int count = std::distance(range.first, range.second);
    // Two OpTypeInt instructions should exist
    EXPECT_EQ(count, 2);
}

// Test EmplaceInstructionNonUnique - Instructions are Always Unique
TEST(ModuleTests, EmplaceInstructionNonUniqueCreatesUniqueInstructions)
{
    Module module;

    Operand op1 = module.EmplaceInstructionNonUnique(spv::OpTypeInt, {Operand{1}, RESID});
    Operand op2 = module.EmplaceInstructionNonUnique(spv::OpTypeInt, {Operand{1}, RESID});

    // Both instructions should have unique pointers
    EXPECT_NE(op1.m_InstructionPtr, nullptr);
    EXPECT_NE(op2.m_InstructionPtr, nullptr);
    EXPECT_NE(op1, op2);

    auto& inst1Operands = op1.m_InstructionPtr->m_Operands;
    auto& inst2Operands = op2.m_InstructionPtr->m_Operands;

    EXPECT_EQ(inst1Operands.size(), inst2Operands.size());
    bool hasDifferentResId = false;

    for (size_t i = 0; i < inst1Operands.size(); ++i)
    {
        if (inst1Operands[i].m_Type == RES_ID && inst2Operands[i].m_Type == RES_ID)
        {
            if (inst1Operands[i].m_LiteralWord != inst2Operands[i].m_LiteralWord)
            {
                hasDifferentResId = true;
                break;
            }
        }
    }
    EXPECT_TRUE(hasDifferentResId);
}

} // namespace tfsc::spirv