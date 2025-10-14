//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by tosa-regen for TOSA 1.0

#include <OpTestUtils.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <gtest/gtest.h>

using namespace tfsc::tosa;
using namespace testutils;

// TEST HASH 16491096087689761570
TEST(TOSA_FOR_SPIRV_CODEGEN_OPERATOR, DepthwiseConv2dOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::DepthwiseConv2d;
const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::uint8_t, {1, 48, 72, 120}}, {std::initializer_list<uint32_t>{}, DataType::uint8_t, {5, 5, 120, 1}}, {std::initializer_list<uint32_t>{}, DataType::uint32_t, {120}}, {std::initializer_list<uint32_t>{128}, DataType::uint8_t, {1}}, {std::initializer_list<uint32_t>{0}, DataType::uint8_t, {1}}};
const std::vector<Tensor> outputs {{DataType::uint32_t, {1, 48, 72, 120}}};
const std::vector<Attribute> attributes {{std::initializer_list<uint32_t>{2, 2, 2, 2}, DataType::uint32_t, {4}}, {std::initializer_list<uint32_t>{1, 1}, DataType::uint32_t, {2}}, {std::initializer_list<uint32_t>{1, 1}, DataType::uint32_t, {2}}, {std::initializer_list<uint32_t>{1}, DataType::uint32_t, {1}}, {std::initializer_list<uint32_t>{0}, DataType::bool_t, {1}}};

// Adding operator using Graph API
std::shared_ptr<tfsc::spirv::Module> module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0].GetTensor(), 0);
const auto& input2 = graph.AddGraphConstant(inputs[1].GetTensor());
const auto& input3 = graph.AddGraphConstant(inputs[2].GetTensor());
const auto& input4 = graph.AddTensorConstant(inputs[3]);
const auto& input5 = graph.AddTensorConstant(inputs[4]);

const auto& attribute1 = attributes[0];
const auto& attribute2 = attributes[1];
const auto& attribute3 = attributes[2];
const auto& attribute4 = attributes[3];
const auto& attribute5 = attributes[4];

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddDepthwiseConv2dOperator(input1, input2, input3, input4, input5, attribute1, attribute2, attribute3, attribute4, attribute5, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, outputs, attributes);
}

