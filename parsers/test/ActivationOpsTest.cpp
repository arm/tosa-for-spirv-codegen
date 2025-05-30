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
TEST(TOSA2SPIRV_PARSER, Clamp)
{
    int32_t min_val_int = 1;
    float min_val_fp = 1;
    int32_t max_val_int = 1;
    float max_val_fp = 1;
    // Create Attribute
    TosaClampAttribute attribute(min_val_int, max_val_int, min_val_fp, max_val_fp);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_CLAMP,
                                                  Attribute::Attribute_ClampAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("clamp", "main", {op}, {inputTensor, outputTensor}, {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "CLAMP", outputStr);
    testutils::CheckConstant(DataType::int8_t, "CLAMP", outputStr, 1, 0);
    testutils::CheckConstant(DataType::int8_t, "CLAMP", outputStr, 1, 1);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "CLAMP", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Erf)
{
    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op =
        new tosa::TosaSerializationOperator(Op::Op_ERF, Attribute::Attribute_NONE, nullptr, {inputName}, {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("erf", "main", {op}, {inputTensor, outputTensor}, {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "ERF", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "ERF", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Sigmoid)
{
    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_SIGMOID,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("sigmoid", "main", {op}, {inputTensor, outputTensor}, {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "SIGMOID", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "SIGMOID", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Tanh)
{
    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op =
        new tosa::TosaSerializationOperator(Op::Op_TANH, Attribute::Attribute_NONE, nullptr, {inputName}, {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("tanh", "main", {op}, {inputTensor, outputTensor}, {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "TANH", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "TANH", outputStr);
}
