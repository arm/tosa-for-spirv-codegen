//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by Spirv2tosaTool for TOSA 1.0

#include <OpTestUtils.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

using namespace tosa2spirv::tosa;
using namespace testutils;

// TEST HASH 7816137131507928185
TEST(TOSA2SPIRV_OPERATOR, TileOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Tile;
const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::bool_t, {79}}, {std::initializer_list<uint32_t>{3}, DataType::uint32_t, {1}}};
const std::vector<Tensor> outputs {{DataType::bool_t, {237}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0].GetTensor(), 0);
const auto& input2 = graph.AddTensorConstant(inputs[1]);

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddTileOperator(input1, input2, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, outputs, attributes);
}

