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

// TEST HASH 4414539027324783339
TEST(TOSA_FOR_SPIRV_CODEGEN_OPERATOR, TableOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Table;
const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::uint8_t, {1, 197, 1}}, {std::initializer_list<uint32_t>{127, 127, 127, 127, 127, 127, 127, 127, 126, 115, 106, 98, 91, 85, 80, 75, 71, 67, 63, 60, 57, 54, 52, 49, 47, 45, 43, 41, 39, 37, 35, 34, 32, 31, 30, 28, 27, 26, 25, 24, 23, 21, 20, 20, 19, 18, 17, 16, 15, 14, 14, 13, 12, 12, 11, 10, 10, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 255, 255, 254, 254, 254, 253, 253, 253, 252, 252, 252, 251, 251, 251, 250, 250, 250, 249, 249, 249, 249, 248, 248, 248, 248, 247, 247, 247, 247, 246, 246, 246, 246, 245, 245, 245, 245, 244, 244, 244, 244, 244, 243, 243, 243, 243, 243, 242, 242, 242, 242, 242, 241, 241, 241, 241, 241, 241, 240, 240, 240, 240, 240, 240, 239, 239, 239, 239, 239, 239, 239, 238, 238, 238, 238, 238, 238, 238, 237, 237, 237, 237, 237, 237, 237, 236, 236, 236, 236, 236, 236, 236, 236, 236, 235, 235, 235, 235, 235, 235, 235, 235, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 233, 233, 233, 233, 233, 233, 233, 233, 233, 233, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 228, 228, 228, 228, 228, 228, 228}, DataType::uint8_t, {256}}};
const std::vector<Tensor> outputs {{DataType::uint8_t, {1, 197, 1}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tfsc::spirv::Module> module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
Graph graph{module};

const auto& input1 = graph.AddInput(inputs[0].GetTensor(), 0);
const auto& input2 = graph.AddTensorConstant(inputs[1]);

const auto& output1 = outputs[0];
const auto& graphRes = graph.AddTableOperator(input1, input2, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, outputs, attributes);
}

