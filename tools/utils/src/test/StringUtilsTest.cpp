
//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#include "spirv/unified1/spirv.hpp"
#include <StringUtils.hpp>

#include <Graph.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <gtest/gtest.h>
#include <spirvmodels/Conv2d.hpp>
#include <spirvmodels/MaxPool.hpp>

namespace testutils
{

TEST(TestUtils, ResIdToString_ValidResId)
{
    Instruction instr;
    Operand op;
    op.m_Type = RES_ID;
    op.m_LiteralWord = 42;
    instr.m_Operands.push_back(op);

    EXPECT_EQ(testutils::ResIdToString(instr), "%42");
}

TEST(TestUtils, ResIdToString_NoResId)
{
    Instruction instr;
    Operand op;
    op.m_Type = LITERAL_WORD;
    op.m_LiteralWord = 10;
    instr.m_Operands.push_back(op);

    EXPECT_EQ(testutils::ResIdToString(instr), "");
}

TEST(TestUtils, OperandToString_InstructionPointer)
{
    Instruction instr;
    Operand nested;
    nested.m_Type = RES_ID;
    nested.m_LiteralWord = 7;
    instr.m_Operands.push_back(nested);

    Operand op;
    op.m_Type = INSTRUCTION_POINTER;
    op.m_InstructionPtr = &instr;

    EXPECT_EQ(testutils::OperandToString(op), "%7");
}

TEST(TestUtils, OperandToString_LiteralWord)
{
    Operand op(123);
    EXPECT_EQ(testutils::OperandToString(op), "123");
}

TEST(TestUtils, OperandToString_LiteralString)
{
    Operand op{"hello"};
    EXPECT_EQ(testutils::OperandToString(op), "hello");
}

TEST(TestUtils, OperandToString_InvalidType)
{
    Operand op;
    op.m_Type = static_cast<OperandType>(999);
    EXPECT_THROW(testutils::OperandToString(op), std::runtime_error);
}

TEST(TestUtils, InstructionToString_WithOperands)
{
    Instruction instr;
    instr.m_Opcode = spv::OpExtInst;
    instr.m_Operands.push_back(Operand{1, RES_ID});
    instr.m_Operands.push_back(Operand{42, LITERAL_WORD});

    EXPECT_EQ(testutils::InstructionToString(instr), "%1 = OpExtInst  42");
}

TEST(TestUtils, GetSubString_NormalCase)
{
    std::string str = "foo{bar}baz";
    EXPECT_EQ(testutils::GetSubString(str, "{", "}"), "bar");
}

TEST(TestUtils, GetSubString_NoLower)
{
    std::string str = "foobar";
    EXPECT_EQ(testutils::GetSubString(str, "[", "]"), "");
}

TEST(TestUtils, GetSubString_UpperBeforeLower)
{
    std::string str = "]foobar[";
    EXPECT_EQ(testutils::GetSubString(str, "[", "]"), "");
}

TEST(TestUtils, GetVisibleLength_AnsiStripped)
{
    std::string str = "\033[38;5;196mRedText\033[0m";
    EXPECT_EQ(testutils::GetVisibleLength(str), 7);
}

TEST(TestUtils, GetVisibleLength_OnlyAnsi)
{
    std::string str = "\033[38;5;196m";
    EXPECT_EQ(testutils::GetVisibleLength(str), 0);
}

TEST(TestUtils, HighlightString_SingleOccurrence)
{
    std::string str = "This is %1";
    testutils::HighlightString(str, "%1", "[", "]");
    EXPECT_EQ(str, "This is [%1]");
}

TEST(TestUtils, HighlightString_NotWordBoundary)
{
    std::string str = "This is %10foo";
    testutils::HighlightString(str, "%10", "[", "]");
    EXPECT_EQ(str, "This is %10foo"); // no change
}

TEST(TestUtils, HighlightResIds_Basic)
{
    std::string inst = "%1 = OpAdd %1 %2";
    std::string highlighted = testutils::HighlightResIds(inst, "%1", "[", "]");
    EXPECT_EQ(highlighted, "[%1] = OpAdd [%1] %2");
}

TEST(TestUtils, GetNextColour_Rotates)
{
    std::string_view c1 = testutils::GetNextColour();
    for (int i = 0; i < 23; ++i)
    {
        testutils::GetNextColour();
    }
    EXPECT_EQ(c1, testutils::GetNextColour()); // should cycle back
}

TEST(TestUtils, Trim_Whitespace)
{
    EXPECT_EQ(testutils::Trim("  hello  "), "hello");
    EXPECT_EQ(testutils::Trim(" \n\r\t "), "");
}

TEST(TestUtils, ConcatStringLines_Basic)
{
    std::string lhs = "line1\nline2\n";
    std::string rhs = "rline1\nrline2\n";
    std::string result = testutils::ConcatStringLines(lhs, rhs, 4);
    EXPECT_NE(result.find("line1"), std::string::npos);
    EXPECT_NE(result.find("rline1"), std::string::npos);
}

} // namespace testutils
