//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
// See tosa2spirv/python/code_generator.py and README

#include <AssemblyUtils.hpp>
#include <OpTestUtils.hpp>
#include <TosaSerializationParser.hpp>

#include <gtest/gtest.h>

using namespace tosa;
using namespace tosa2spirv::parsers;
TEST(TOSA2SPIRV_PARSER, Select)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string input3Name = "input3";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> input3Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_BOOL, {});
    auto* input3Tensor = new TosaSerializationTensor(input3Name, input3Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_SELECT,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  {input1Name, input2Name, input3Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("select",
                                      "main",
                                      {op},
                                      {input1Tensor, input2Tensor, input3Tensor, outputTensor},
                                      {input1Name, input2Name, input3Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "SELECT", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "SELECT", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "SELECT", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "SELECT", outputStr);
}
