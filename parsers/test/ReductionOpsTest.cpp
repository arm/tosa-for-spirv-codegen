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

TEST(TOSA2SPIRV_PARSER, ReduceAll)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_REDUCE_ALL,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reduce_all",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "REDUCE_ALL", outputStr);
    spirvwriter::CheckConstant(DataType::int32_t, "REDUCE_ALL", outputStr, 1, 0);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "REDUCE_ALL", outputStr);
}

TEST(TOSA2SPIRV_PARSER, ReduceAny)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_REDUCE_ANY,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reduce_any",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "REDUCE_ANY", outputStr);
    spirvwriter::CheckConstant(DataType::int32_t, "REDUCE_ANY", outputStr, 1, 0);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "REDUCE_ANY", outputStr);
}

TEST(TOSA2SPIRV_PARSER, ReduceMax)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_REDUCE_MAX,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reduce_max",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "REDUCE_MAX", outputStr);
    spirvwriter::CheckConstant(DataType::int32_t, "REDUCE_MAX", outputStr, 1, 0);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "REDUCE_MAX", outputStr);
}

TEST(TOSA2SPIRV_PARSER, ReduceMin)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_REDUCE_MIN,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reduce_min",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "REDUCE_MIN", outputStr);
    spirvwriter::CheckConstant(DataType::int32_t, "REDUCE_MIN", outputStr, 1, 0);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "REDUCE_MIN", outputStr);
}

TEST(TOSA2SPIRV_PARSER, ReduceProduct)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_REDUCE_PRODUCT,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reduce_product",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "REDUCE_PRODUCT", outputStr);
    spirvwriter::CheckConstant(DataType::int32_t, "REDUCE_PRODUCT", outputStr, 1, 0);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "REDUCE_PRODUCT", outputStr);
}

TEST(TOSA2SPIRV_PARSER, ReduceSum)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_REDUCE_SUM,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reduce_sum",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "REDUCE_SUM", outputStr);
    spirvwriter::CheckConstant(DataType::int32_t, "REDUCE_SUM", outputStr, 1, 0);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "REDUCE_SUM", outputStr);
}

