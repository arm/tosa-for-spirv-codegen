//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Instruction.hpp>

#include <gtest/gtest.h>

using namespace tfsc::spirv;

// Test Operand Type and Word Size
TEST(OperandTests, TypeAndWordSize)
{
    Operand intOperand(1234);
    EXPECT_EQ(intOperand.m_Type, LITERAL_WORD);
    EXPECT_EQ(intOperand.WordSize(), 1);

    Operand residOperand(1234, RES_ID);
    EXPECT_EQ(residOperand.m_Type, RES_ID);

    Operand strOperand("Test");
    EXPECT_EQ(strOperand.m_Type, LITERAL_STRING);
    EXPECT_EQ(strOperand.WordSize(), 2); // "Test" (4 chars + 1 null)

    Operand emptyStrOperand("");
    EXPECT_EQ(emptyStrOperand.WordSize(), 1); // "" (1 null char)

    Operand longStrOperand("LongerString");
    EXPECT_EQ(longStrOperand.WordSize(), 4); // "LongerString" (13 chars + 1 null)

    Instruction instruction{spv::OpTypeGraphARM, {Operand{0u}, Operand{0u}}};
    Operand instrOperand(&instruction);
    EXPECT_EQ(instrOperand.m_Type, INSTRUCTION_POINTER);
    EXPECT_EQ(Operand().m_Type, UNINITIALIZED);
}

// Test Equality, Inequality, and Less Than Operators
TEST(OperandTests, Operators)
{
    Operand intOperand1(1234), intOperand2(1234), intOperand3(5678);
    EXPECT_EQ(intOperand1, intOperand2);
    EXPECT_NE(intOperand1, intOperand3);
    EXPECT_LT(intOperand1, intOperand3);

    Operand resOperand1(1234, RES_ID), resOperand2(1234, RES_ID), resOperand3(5678, RES_ID);
    EXPECT_EQ(resOperand1, resOperand2);
    EXPECT_NE(resOperand1, resOperand3);
    EXPECT_LT(resOperand1, resOperand3);

    Operand strOperand1("Equal"), strOperand2("Equal"), strOperand3("NotEqual");
    EXPECT_EQ(strOperand1, strOperand2);
    EXPECT_NE(strOperand1, strOperand3);
    EXPECT_NE(strOperand1, strOperand3);
    EXPECT_LT(strOperand1, strOperand3);

    Instruction instruction1{spv::OpTypeGraphARM, {Operand{0u}, Operand{0u}}};
    Instruction instruction2{spv::OpTypeGraphARM, {Operand{1}, Operand{1}}};

    Operand instrOperand1(&instruction1), instrOperand2(&instruction1), instrOperand3(&instruction2);
    EXPECT_EQ(instrOperand1, instrOperand2);
    EXPECT_NE(instrOperand1, instrOperand3);
    EXPECT_LT(instrOperand1, instrOperand3);
}

// Test Copy Constructor & Copy Assignment
TEST(OperandTests, CopyBehavior)
{
    Instruction instruction{spv::OpTypeGraphARM, {Operand{0u}, Operand{0u}}};

    // Test Copy Constructor
    Operand intOriginal(123);
    Operand copyInt(intOriginal);
    EXPECT_EQ(intOriginal, copyInt);

    Operand resOriginal(123, RES_ID);
    Operand copyRes(resOriginal);
    EXPECT_EQ(resOriginal, copyRes);

    Operand insOriginal(&instruction);
    Operand copyIns(insOriginal);
    EXPECT_EQ(insOriginal, copyIns);

    Operand strOriginal("Test");
    Operand copyStr(strOriginal);
    EXPECT_EQ(*copyStr.m_LiteralStr, "Test");
    EXPECT_NE(strOriginal.m_LiteralStr, copyStr.m_LiteralStr);

    // Test Copy Assignment
    Operand intA(42);
    Operand intB = intA;
    EXPECT_EQ(intA, intB);

    Operand resA(42, RES_ID);
    Operand resB = resA;
    EXPECT_EQ(resA, resB);

    Operand insA(&instruction);
    Operand insB = insA;
    EXPECT_EQ(insA, insB);

    Operand strA("Copy");
    Operand strB = strA;
    EXPECT_EQ(*strA.m_LiteralStr, *strB.m_LiteralStr);
    EXPECT_NE(strA.m_LiteralStr, strB.m_LiteralStr);

    // Test Self Assignment
    Operand selfAssign("SelfTest");
    selfAssign = selfAssign;
    EXPECT_EQ(*selfAssign.m_LiteralStr, "SelfTest");
}

// Test Move Constructor & Move Assignment
TEST(OperandTests, MoveBehavior)
{
    Operand intOriginal(123);
    Operand movedInt(std::move(intOriginal));
    EXPECT_EQ(movedInt.m_LiteralWord, 123);
    // Defaults to 0 after move
    EXPECT_EQ(intOriginal.m_LiteralWord, 0);

    Operand resOriginal(123, RES_ID);
    Operand movedRes(std::move(resOriginal));
    EXPECT_EQ(movedRes.m_LiteralWord, 123);
    EXPECT_EQ(resOriginal.m_LiteralWord, 0);

    Operand strOriginal("MoveTest");
    Operand movedStr(std::move(strOriginal));
    EXPECT_EQ(*movedStr.m_LiteralStr, "MoveTest");
    EXPECT_EQ(strOriginal.m_LiteralStr, nullptr);

    Instruction instruction{spv::OpTypeGraphARM, {Operand{0u}, Operand{0u}}};
    Operand insOriginal(&instruction);
    Operand movedIns(std::move(insOriginal));
    EXPECT_EQ(movedIns.m_InstructionPtr, &instruction);
    EXPECT_EQ(insOriginal.m_InstructionPtr, nullptr);

    // Move Assignment
    Operand strOperand1("Move1"), strOperand2("Move2");
    strOperand2 = std::move(strOperand1);
    EXPECT_EQ(*strOperand2.m_LiteralStr, "Move1");
    EXPECT_EQ(strOperand1.m_LiteralStr, nullptr);

    Operand intOperand1(100), intOperand2(200);
    intOperand2 = std::move(intOperand1);
    EXPECT_EQ(intOperand2.m_LiteralWord, 100);
    EXPECT_EQ(intOperand1.m_LiteralWord, 0);

    Operand insOperand1(&instruction), insOperand2("Move2");
    insOperand2 = std::move(insOperand1);
    EXPECT_EQ(insOperand2.m_InstructionPtr, &instruction);
    EXPECT_EQ(insOperand1.m_InstructionPtr, nullptr);
}

TEST(OperandTests, Destructor)
{
    Operand* strOperand = new Operand("Temporary");
    delete strOperand;
    SUCCEED();
}

TEST(OperandTests, DefaultConstructor)
{
    Operand defaultOperand;
    EXPECT_EQ(defaultOperand.m_Type, UNINITIALIZED);
    EXPECT_EQ(defaultOperand.m_LiteralWord, 0);
}