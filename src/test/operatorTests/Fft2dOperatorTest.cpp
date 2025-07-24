//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by Spirv2tosaTool for TOSA 1.0

#include <OpTestUtils.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

using namespace tosa2spirv::tosa;

// TEST HASH 11684528478320844845
TEST(TOSA2SPIRV_OPERATOR, Fft2dOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Fft2d;
const std::vector<Tensor> inputs {{DataType::float32_t, {1, 16, 8}}};
const std::vector<Tensor> graphConstants {{DataType::float32_t, {1, 16, 8}}};
const std::vector<Attribute> tensorConstants {};
const std::vector<Tensor> outputs {{DataType::float32_t, {1, 16, 8}}, {DataType::float32_t, {1, 16, 8}}};
const std::vector<Attribute> attributes {{std::initializer_list<uint32_t>{1}, DataType::bool_t, {1}}, {std::initializer_list<uint32_t>{0}, DataType::bool_t, {1}}};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0], 0);
const auto& graphConstInput1 = graph.AddGraphConstant(graphConstants[0]);

const auto& attribute1 = attributes[0];
const auto& attribute2 = attributes[1];

const auto& output1 = outputs[0];
const auto& output2 = outputs[1];
const auto& graphRes = graph.AddFft2dOperator(input1, graphConstInput1, attribute1, attribute2, output1, output2);
graph.AddOutput(graphRes[0], 0);
graph.AddOutput(graphRes[1], 1);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, graphConstants, tensorConstants, outputs, attributes);
}

