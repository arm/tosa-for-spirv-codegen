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
TEST(TOSA2SPIRV_PARSER, Equal)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_EQUAL,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  {input1Name, input2Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("equal",
                                      "main",
                                      {op},
                                      {input1Tensor, input2Tensor, outputTensor},
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "EQUAL", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "EQUAL", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "EQUAL", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Greater)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_GREATER,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  {input1Name, input2Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("greater",
                                      "main",
                                      {op},
                                      {input1Tensor, input2Tensor, outputTensor},
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "GREATER", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "GREATER", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "GREATER", outputStr);
}

TEST(TOSA2SPIRV_PARSER, GreaterEqual)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_GREATER_EQUAL,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  {input1Name, input2Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("greater_equal",
                                      "main",
                                      {op},
                                      {input1Tensor, input2Tensor, outputTensor},
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "GREATER_EQUAL", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "GREATER_EQUAL", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "GREATER_EQUAL", outputStr);
}
