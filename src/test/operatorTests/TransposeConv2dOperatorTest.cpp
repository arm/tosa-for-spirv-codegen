//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by Spirv2tosaTool for TOSA 1.0

#include <OpTestUtils.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

using namespace tosa2spirv::tosa;

// TEST HASH 787836048650573220
TEST(TOSA2SPIRV_OPERATOR, TransposeConv2dOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::TransposeConv2d;
const std::vector<Tensor> inputs {{DataType::uint8_t, {1, 18, 19, 16}}};
const std::vector<Tensor> graphConstants {{DataType::uint8_t, {3, 2, 2, 16}}};
const std::vector<Attribute> tensorConstants {{std::initializer_list<uint32_t>{17, 4294967269, 4294967288}, DataType::uint32_t, {3}}, {std::initializer_list<uint32_t>{0}, DataType::uint8_t, {1}}, {std::initializer_list<uint32_t>{249}, DataType::uint8_t, {1}}};
const std::vector<Tensor> outputs {{DataType::uint32_t, {1, 38, 42, 3}}};
const std::vector<Attribute> attributes {{std::initializer_list<uint32_t>{0, 2, 0, 4}, DataType::uint32_t, {4}}, {std::initializer_list<uint32_t>{2, 2}, DataType::uint32_t, {2}}, {std::initializer_list<uint32_t>{1}, DataType::uint32_t, {1}}, {std::initializer_list<uint32_t>{0}, DataType::bool_t, {1}}};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0], 0);
const auto& graphConstInput1 = graph.AddGraphConstant(graphConstants[0]);
const auto& tensorConstInput1 = graph.AddTensorConstant(tensorConstants[0]);
const auto& tensorConstInput2 = graph.AddTensorConstant(tensorConstants[1]);
const auto& tensorConstInput3 = graph.AddTensorConstant(tensorConstants[2]);

const auto& attribute1 = attributes[0];
const auto& attribute2 = attributes[1];
const auto& attribute3 = attributes[2];
const auto& attribute4 = attributes[3];

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddTransposeConv2dOperator(input1, graphConstInput1, tensorConstInput1, tensorConstInput2, tensorConstInput3, attribute1, attribute2, attribute3, attribute4, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, graphConstants, tensorConstants, outputs, attributes);
}

