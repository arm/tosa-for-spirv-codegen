//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
// See tosa2spirv/python/frontend_generator.py and README

#include <TosaSerializationParser.hpp>
#include <Writer.hpp>
#include <TestUtils.hpp>
#include <OpTestUtils.hpp>

#include <gtest/gtest.h>

using namespace tosa;
using namespace tosa2spirv;
using namespace parsers;

TEST(TOSA2SPIRV_PARSER, Cast)
{
    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_CAST,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("cast",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "CAST", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "CAST", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Rescale)
{
    int8_t input_zp = 1;
    int8_t output_zp = 1;
    std::vector<int32_t> multiplier = {1};
    std::vector<int32_t> shift = {1};
    bool scale32 = true;
    bool double_round = true;
    bool per_channel = true;
    bool input_unsigned = true;
    bool output_unsigned = true;
    // Create Attribute
    TosaRescaleAttribute attribute(input_zp, output_zp, multiplier, shift, scale32, double_round, per_channel, input_unsigned, output_unsigned);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                  Attribute::Attribute_RescaleAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("rescale",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "RESCALE", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "RESCALE", outputStr);
    spirvwriter::CheckConstant(DataType::int8_t, "RESCALE", outputStr, 1, 0);
    spirvwriter::CheckConstant(DataType::int8_t, "RESCALE", outputStr, 1, 1);
    spirvwriter::CheckGraphConstant({ 1 }, DataType::int32_t, "RESCALE", outputStr);
    spirvwriter::CheckGraphConstant({ 1 }, DataType::int32_t, "RESCALE", outputStr);
    spirvwriter::CheckBoolConstant(DataType::bool_t, "RESCALE", outputStr, true, 4);
    spirvwriter::CheckBoolConstant(DataType::bool_t, "RESCALE", outputStr, true, 5);
    spirvwriter::CheckBoolConstant(DataType::bool_t, "RESCALE", outputStr, true, 6);
    spirvwriter::CheckBoolConstant(DataType::bool_t, "RESCALE", outputStr, true, 7);
    spirvwriter::CheckBoolConstant(DataType::bool_t, "RESCALE", outputStr, true, 8);
}

