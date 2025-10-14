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

// TEST HASH 12425848590834726304
TEST(TOSA_FOR_SPIRV_CODEGEN_OPERATOR, LogicalAndOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::LogicalAnd;
const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}, {std::initializer_list<uint32_t>{}, DataType::bool_t, {4, 6, 8, 10}}};
const std::vector<Tensor> outputs {{DataType::bool_t, {4, 6, 8, 10}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tfsc::spirv::Module> module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0].GetTensor(), 0);
const auto& input2 = graph.AddInput(inputs[1].GetTensor(), 1);

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddLogicalAndOperator(input1, input2, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, outputs, attributes);
}

