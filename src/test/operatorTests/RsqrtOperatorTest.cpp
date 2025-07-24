//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// Generated automatically by Spirv2tosaTool for TOSA 1.0

#include <OpTestUtils.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

using namespace tosa2spirv::tosa;

// TEST HASH 2771883454056244998
TEST(TOSA2SPIRV_OPERATOR, RsqrtOperatorTest0)
{
// Operator Definition, separated for reuse in the test fixture
const OperatorEnum op = OperatorEnum::Rsqrt;
const std::vector<Tensor> inputs {};
const std::vector<Tensor> graphConstants {};
const std::vector<Attribute> tensorConstants {{std::initializer_list<uint32_t>{1}, DataType::float32_t, {1}}};
const std::vector<Tensor> outputs {{DataType::float32_t, {1}}};
const std::vector<Attribute> attributes {};

// Adding operator using Graph API
std::shared_ptr<tosa2spirv::spirv::Module> module = tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0);
Graph graph{module};

const auto& tensorConstInput1 = graph.AddTensorConstant(tensorConstants[0]);


const auto& output1 = outputs[0];
const auto& graphRes = graph.AddRsqrtOperator(tensorConstInput1, output1);
graph.AddOutput(graphRes, 0);
graph.FinalizeGraph();

// Validating generated SPIR-V Module
testutils::CheckModule(module, op, inputs, graphConstants, tensorConstants, outputs, attributes);
}

