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

// TEST HASH 8537222349719328075
TEST(TOSA2SPIRV_OPERATOR, ClampOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Clamp;
const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::uint8_t, {1, 1, 1, 24}}};
const std::vector<Tensor> outputs {{DataType::uint8_t, {1, 1, 1, 24}}};
const std::vector<Attribute> attributes {{std::initializer_list<uint32_t>{0}, DataType::uint8_t, {1}}, {std::initializer_list<uint32_t>{127}, DataType::uint8_t, {1}}, {std::initializer_list<uint32_t>{1}, DataType::uint32_t, {1}}};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0].GetTensor(), 0);

const auto& attribute1 = attributes[0];
const auto& attribute2 = attributes[1];
const auto& attribute3 = attributes[2];

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddClampOperator(input1, attribute1, attribute2, attribute3, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, outputs, attributes);
}

