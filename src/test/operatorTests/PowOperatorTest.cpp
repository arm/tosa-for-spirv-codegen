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

// TEST HASH 4524290647387079834
TEST(TOSA_FOR_SPIRV_CODEGEN_OPERATOR, PowOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Pow;
const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::float32_t, {20}}, {std::initializer_list<uint32_t>{}, DataType::float32_t, {20}}};
const std::vector<Tensor> outputs {{DataType::float32_t, {20}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tfsc::spirv::Module> module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddGraphConstant(inputs[0].GetTensor());
const auto& input2 = graph.AddInput(inputs[1].GetTensor(), 1);

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddPowOperator(input1, input2, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, outputs, attributes);
}

