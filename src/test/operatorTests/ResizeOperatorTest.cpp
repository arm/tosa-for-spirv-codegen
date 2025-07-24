//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by Spirv2tosaTool for TOSA 1.0

#include <OpTestUtils.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

using namespace tosa2spirv::tosa;

// TEST HASH 17146890655786874055
TEST(TOSA2SPIRV_OPERATOR, ResizeOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Resize;
const std::vector<Tensor> inputs {{DataType::uint8_t, {1, 135, 240, 32}}};
const std::vector<Tensor> graphConstants {};
const std::vector<Attribute> tensorConstants {{std::initializer_list<uint32_t>{4, 2, 4, 2}, DataType::uint32_t, {4}}, {std::initializer_list<uint32_t>{4294967295, 4294967295}, DataType::uint32_t, {2}}, {std::initializer_list<uint32_t>{1, 1}, DataType::uint32_t, {2}}};
const std::vector<Tensor> outputs {{DataType::uint8_t, {1, 270, 480, 32}}};
const std::vector<Attribute> attributes {{std::initializer_list<uint32_t>{1}, DataType::uint32_t, {1}}};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0], 0);
const auto& tensorConstInput1 = graph.AddTensorConstant(tensorConstants[0]);
const auto& tensorConstInput2 = graph.AddTensorConstant(tensorConstants[1]);
const auto& tensorConstInput3 = graph.AddTensorConstant(tensorConstants[2]);

const auto& attribute1 = attributes[0];

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddResizeOperator(input1, tensorConstInput1, tensorConstInput2, tensorConstInput3, attribute1, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, graphConstants, tensorConstants, outputs, attributes);
}

