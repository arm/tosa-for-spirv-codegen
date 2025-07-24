//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by Spirv2tosaTool for TOSA 1.0

#include <OpTestUtils.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

using namespace tosa2spirv::tosa;

// TEST HASH 9025290910311207549
TEST(TOSA2SPIRV_OPERATOR, GreaterEqualOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::GreaterEqual;
const std::vector<Tensor> inputs {{DataType::uint32_t, {1, 720, 1280, 16}}};
const std::vector<Tensor> graphConstants {{DataType::uint32_t, {1, 1, 1, 1}}};
const std::vector<Attribute> tensorConstants {};
const std::vector<Tensor> outputs {{DataType::bool_t, {1, 720, 1280, 16}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0], 0);
const auto& graphConstInput1 = graph.AddGraphConstant(graphConstants[0]);


const auto& output1 = outputs[0];
const auto& graphRes = graph.AddGreaterEqualOperator(input1, graphConstInput1, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, graphConstants, tensorConstants, outputs, attributes);
}

