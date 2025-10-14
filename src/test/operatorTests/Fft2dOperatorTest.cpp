    //
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by tosa-regen for TOSA 1.0

#include <OpTestUtils.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <gtest/gtest.h>

using namespace tfsc::tosa;

// TEST HASH 13494171558245959358
TEST(TOSA_FOR_SPIRV_CODEGEN_OPERATOR, Fft2dOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Fft2d;
const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::float32_t, {1, 16, 8}}, {std::initializer_list<uint32_t>{}, DataType::float32_t, {1, 16, 8}}};
const std::vector<Tensor> outputs {{DataType::float32_t, {1, 16, 8}}, {DataType::float32_t, {1, 16, 8}}};
const std::vector<Attribute> attributes {{std::initializer_list<uint32_t>{1}, DataType::bool_t, {1}}, {std::initializer_list<uint32_t>{0}, DataType::bool_t, {1}}};

// Adding operator using Graph API
std::shared_ptr<tfsc::spirv::Module> module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddGraphConstant(inputs[0].GetTensor());
const auto& input2 = graph.AddInput(inputs[1].GetTensor(), 1);

const auto& attribute1 = attributes[0];
const auto& attribute2 = attributes[1];

const auto& output1 = outputs[0];
const auto& output2 = outputs[1];
const auto& graphRes = graph.AddFft2dOperator(input1, input2, attribute1, attribute2, output1, output2);
graph.AddOutput(graphRes[0], 0);
graph.AddOutput(graphRes[1], 1);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, outputs, attributes);
}

