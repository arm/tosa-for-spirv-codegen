
//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
//
#include <Instruction.hpp>
#include <ProblemTree.hpp>
#include <gtest/gtest.h>

using namespace tfsc::spirv;
using namespace testutils;

// Test InstructionPair equality and ordering
TEST(ProblemTreeTests, InstructionPairComparison)
{
    Instruction lhs(spv::OpExtInst, {Operand{1}, Operand{2}});
    Instruction rhs(spv::OpExtInst, {Operand{3}, Operand{4}});

    InstructionPair p1(&lhs, &rhs);
    InstructionPair p2(&lhs, &rhs);

    EXPECT_FALSE(p1 < p2);
}

// Test ProblemTree equality and ordering
TEST(ProblemTreeTests, ProblemTreeEqualityAndOrdering)
{
    Instruction lhs(spv::OpExtInst, {Operand{100}});
    Instruction rhs(spv::OpExtInst, {Operand{200}});

    ProblemTree pt1(&lhs, &rhs, 1, true);
    ProblemTree pt2(&lhs, &rhs, 1, true);

    EXPECT_TRUE(pt1 == pt2);
    EXPECT_FALSE(pt1 < pt2);
}

// CanonicalizeProblems groups correctly and deduplicates
TEST(ProblemTreeTests, CanonicalizeProblemsDeduplication)
{
    Instruction lhs(spv::OpExtInst, {Operand{10}});
    Instruction rhs(spv::OpExtInst, {Operand{20}});

    ProblemTree pt(&lhs, &rhs, 1, true);
    std::vector<ProblemTree> trees{pt, pt};

    auto grouped = CanonicalizeProblems(trees);
    ASSERT_EQ(grouped.size(), 1);

    const auto& [problemGroup, treeSet] = *grouped.begin();
    EXPECT_EQ(problemGroup.size(), 1);
    EXPECT_EQ(treeSet.size(), 0);
}

// Ensure CollectResIds extracts RES_IDs correctly
TEST(ProblemTreeTests, CollectResIdsExtracts)
{
    Instruction lhs(spv::OpExtInst, {Operand{123, RES_ID}});
    Instruction rhs(spv::OpExtInst, {Operand{456, RES_ID}});

    ProblemTree pt(&lhs, &rhs, 0, false);

    std::vector<std::string> lhsIds;
    std::vector<std::string> rhsIds;
    CollectResIds(pt, lhsIds, rhsIds);

    ASSERT_EQ(lhsIds.size(), 1);
    ASSERT_EQ(rhsIds.size(), 1);
    EXPECT_NE(lhsIds[0], rhsIds[0]);
}

// Ensure printing functions execute and return non-empty strings
TEST(ProblemTreeTests, PrintFunctionsReturnStrings)
{
    Instruction lhs(spv::OpExtInst, {Operand{1, RES_ID}});
    Instruction rhs(spv::OpExtInst, {Operand{2, RES_ID}});
    ProblemTree pt(&lhs, &rhs, 2, true);

    HighlightCtx ctx;
    std::string lhsStr = PrintProblemTreeLhs(pt);
    std::string rhsStr = PrintProblemTreeRhs(pt);
    std::string lineStr = PrintInstructionPair(pt.m_Root, pt.m_Depth, pt.m_IsAProblem, ctx);

    EXPECT_FALSE(lhsStr.empty());
    EXPECT_FALSE(rhsStr.empty());
    EXPECT_FALSE(lineStr.empty());
}
