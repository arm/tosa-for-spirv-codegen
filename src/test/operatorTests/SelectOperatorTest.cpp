//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by Spirv2tosaTool for TOSA 1.0

#include <OpTestUtils.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

using namespace tosa2spirv::tosa;

// TEST HASH 5161346066531117142
TEST(TOSA2SPIRV_OPERATOR, SelectOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Select;
const std::vector<Tensor> inputs {{DataType::bool_t, {1, 720, 1280, 16}}, {DataType::uint8_t, {1, 720, 1280, 16}}, {DataType::uint8_t, {1, 720, 1280, 16}}};
const std::vector<Tensor> graphConstants {};
const std::vector<Attribute> tensorConstants {};
const std::vector<Tensor> outputs {{DataType::uint8_t, {1, 720, 1280, 16}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0], 0);
const auto& input2 = graph.AddInput(inputs[1], 1);
const auto& input3 = graph.AddInput(inputs[2], 2);


const auto& output1 = outputs[0];
const auto& graphRes = graph.AddSelectOperator(input1, input2, input3, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, graphConstants, tensorConstants, outputs, attributes);
}

