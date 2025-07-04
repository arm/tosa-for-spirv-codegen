//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <AssemblyUtils.hpp>
#include <ModuleComparator.hpp>
#include <spirvmodels/Add.hpp>

#include <spirvmodels/Conv2d.hpp>
#include <spirvmodels/MaxPool.hpp>
#include <spirvmodels/Rescale.hpp>

#include <Graph.hpp>
#include <gtest/gtest.h>
#include <tosa2spirv.hpp>

namespace testutils
{
using namespace tosa2spirv;
using namespace tosa2spirv::tosa;

TEST(TESTUTILS, TestReader)
{
    const auto module0 = LoadSPIRVDisassembly(spirvmodels::SimpleMaxpool2d);
    const auto binary0 = WriteToBinary(module0);
    EXPECT_EQ(binary0.size(), 201);
}

void RoundTripModel(const std::string& model)
{
    const auto module0 = LoadSPIRVDisassembly(model);
    EXPECT_FALSE(module0->GetSpirvGraph().empty());
    const auto binary0 = WriteToBinary(module0);
    EXPECT_FALSE(binary0.empty());
    const std::string disassembly0 = DisassembleSPIRV(binary0, true);
    EXPECT_FALSE(disassembly0.empty());

    const auto module1 = LoadSPIRVDisassembly(disassembly0);
    const auto diff = CompareModules(module0, module1);
    EXPECT_TRUE(diff.empty());
}

TEST(TESTUTILS, RoundTripModels)
{
    RoundTripModel(spirvmodels::SimpleMaxpool2d);
    RoundTripModel(spirvmodels::SimpleMaxpool2dGenerator);
    RoundTripModel(spirvmodels::AddZeroDimOutputTensor);
    RoundTripModel(spirvmodels::SimpleConv2d);
    RoundTripModel(spirvmodels::Conv2DRescaleConv2D);
    RoundTripModel(spirvmodels::Conv2DRescaleConv2DDualOutput);
    RoundTripModel(spirvmodels::simpleRescale);
}

TEST(TESTUTILS, Idempotence)
{
    const auto reference = LoadSPIRVDisassembly(spirvmodels::SimpleMaxpool2d);
    auto module = LoadSPIRVDisassembly(spirvmodels::SimpleMaxpool2d);

    for (int i = 0; i < 10; i++)
    {
        const auto binary = WriteToBinary(module);
        const auto modelDisassembly = DisassembleSPIRV(binary, true);
        module = LoadSPIRVDisassembly(modelDisassembly);
        const auto diff = CompareModules(module, reference);
        EXPECT_TRUE(diff.empty());
    }
}

} // namespace testutils
