//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by Spirv2tosaTool for TOSA 1.0

#include <OpTestUtils.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

using namespace tosa2spirv::tosa;

// TEST HASH 8193335299602792620
TEST(TOSA2SPIRV_OPERATOR, MatmulOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Matmul;
const std::vector<Tensor> inputs {{DataType::uint8_t, {3, 197, 64}}, {DataType::uint8_t, {3, 64, 197}}};
const std::vector<Tensor> graphConstants {};
const std::vector<Attribute> tensorConstants {{std::initializer_list<uint32_t>{255}, DataType::uint8_t, {1}}, {std::initializer_list<uint32_t>{0}, DataType::uint8_t, {1}}};
const std::vector<Tensor> outputs {{DataType::uint32_t, {3, 197, 197}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0], 0);
const auto& input2 = graph.AddInput(inputs[1], 1);
const auto& tensorConstInput1 = graph.AddTensorConstant(tensorConstants[0]);
const auto& tensorConstInput2 = graph.AddTensorConstant(tensorConstants[1]);


const auto& output1 = outputs[0];
const auto& graphRes = graph.AddMatmulOperator(input1, input2, tensorConstInput1, tensorConstInput2, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, graphConstants, tensorConstants, outputs, attributes);
}

