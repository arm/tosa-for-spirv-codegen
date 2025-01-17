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

TEST(TOSA2SPIRV_PARSER, Gather)
{
    // Create Tensors
    std::string valuesName = "values";
    std::string indicesName = "indices";
    std::string outputName = "output";

    std::vector<int32_t> valuesShape = {1,1,1,1};
    std::vector<int32_t> indicesShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* valuesTensor = new TosaSerializationTensor(valuesName, valuesShape, DType::DType_INT8, {});
    auto* indicesTensor = new TosaSerializationTensor(indicesName, indicesShape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_GATHER,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { valuesName, indicesName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("gather",
                                      "main",
                                      { op },
                                      { valuesTensor, indicesTensor, outputTensor },
                                      { valuesName, indicesName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "GATHER", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "GATHER", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "GATHER", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Scatter)
{
    // Create Tensors
    std::string values_inName = "values_in";
    std::string indicesName = "indices";
    std::string inputName = "input";
    std::string values_outName = "values_out";

    std::vector<int32_t> values_inShape = {1,1,1,1};
    std::vector<int32_t> indicesShape = {1,1,1,1};
    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> values_outShape = {1,1,1,1};

    auto* values_inTensor = new TosaSerializationTensor(values_inName, values_inShape, DType::DType_INT8, {});
    auto* indicesTensor = new TosaSerializationTensor(indicesName, indicesShape, DType::DType_INT32, {});
    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* values_outTensor = new TosaSerializationTensor(values_outName, values_outShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_SCATTER,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { values_inName, indicesName, inputName },
                                                  { values_outName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("scatter",
                                      "main",
                                      { op },
                                      { values_inTensor, indicesTensor, inputTensor, values_outTensor },
                                      { values_inName, indicesName, inputName },
                                      { values_outName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "SCATTER", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "SCATTER", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "SCATTER", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "SCATTER", outputStr);
}

