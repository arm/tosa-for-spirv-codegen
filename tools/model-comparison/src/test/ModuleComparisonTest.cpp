//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#include <AssemblyUtils.hpp>
#include <ModuleComparator.hpp>

#include <Graph.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <gtest/gtest.h>
#include <spirvmodels/Conv2d.hpp>
#include <spirvmodels/MaxPool.hpp>

namespace testutils
{
using namespace tfsc;
using namespace tfsc::tosa;

TEST(TESTUTILS, ModuleComparator)
{
    auto module0 = CreateModule(TOSAVersion{});
    auto graph0 = Graph(module0);

    auto input0 = graph0.AddInput(Tensor(DataType::int8_t, std::vector<unsigned int>{1, 1, 1, 1}), 0);

    auto kernel = Attribute({1, 1}, DataType::int32_t);

    auto stride = Attribute({1, 1}, DataType::int32_t);

    auto pad = Attribute({1, 1, 1, 1}, DataType::int32_t);

    auto nan_mode = Attribute({1}, DataType::int32_t);

    auto outputShape = Tensor(DataType::int8_t, std::vector<unsigned int>{1, 1, 1, 1});

    const auto out0 = graph0.AddMaxPool2dOperator(input0, kernel, stride, pad, nan_mode, outputShape);

    graph0.AddOutput(out0, 0);
    graph0.FinalizeGraph();
    const auto diff0 = CompareModules(module0, module0);
    EXPECT_TRUE(diff0.empty());

    auto module1 = CreateModule(TOSAVersion{});
    auto graph1 = Graph(module1);
    const auto input1 = graph1.AddInput(Tensor(DataType::int8_t, std::vector<unsigned int>{1, 1, 1, 2}), 0);

    const auto out1 = graph1.AddMaxPool2dOperator(input1, kernel, stride, pad, nan_mode, outputShape);

    graph1.AddOutput(out1, 0);
    graph1.FinalizeGraph();

    const auto diff1 = CompareModules(module0, module1);
    EXPECT_FALSE(diff1.empty());
}

TEST(TESTUTILS, TestComparisonOfDisassembly)
{
    const auto maxPool0 = LoadSPIRVDisassembly(spirvmodels::SimpleMaxpool2d);
    EXPECT_FALSE(maxPool0->GetSpirvGraph().empty());

    const auto diff0 = CompareModules(maxPool0, maxPool0);
    EXPECT_TRUE(diff0.empty());

    const auto conv2d = LoadSPIRVDisassembly(spirvmodels::SimpleConv2d);
    EXPECT_FALSE(conv2d->GetSpirvGraph().empty());

    const auto diff1 = CompareModules(maxPool0, conv2d);
    EXPECT_FALSE(diff1.empty());

    const auto maxPool1 = LoadSPIRVDisassembly(spirvmodels::SimpleMaxpool2d);
    EXPECT_FALSE(maxPool1->GetSpirvGraph().empty());
    const auto diff2 = CompareModules(maxPool0, maxPool1);
    EXPECT_TRUE(diff2.empty());

    maxPool0->EmplaceInstruction(spv::OpConstant, {Operand{0u}});
    const auto diff4 = CompareModules(maxPool0, maxPool1);
    EXPECT_TRUE(diff4.empty());
}
} // namespace testutils
