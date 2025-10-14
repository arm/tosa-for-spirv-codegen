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

// TEST HASH 4396211930894833507
TEST(tosa_for_spirv_codegen_OPERATOR, PadOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Pad;
const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::uint8_t, {1, 112, 112, 64}}, {std::initializer_list<uint32_t>{0, 0, 1, 0, 1, 0, 0, 0}, DataType::uint32_t, {8}}, {std::initializer_list<uint32_t>{252}, DataType::uint8_t, {1}}};
const std::vector<Tensor> outputs {{DataType::uint8_t, {1, 113, 113, 64}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tfsc::spirv::Module> module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0].GetTensor(), 0);
const auto& input2 = graph.AddTensorConstant(inputs[1]);
const auto& input3 = graph.AddTensorConstant(inputs[2]);

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddPadOperator(input1, input2, input3, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, outputs, attributes);
}

