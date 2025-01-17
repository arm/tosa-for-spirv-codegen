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

TEST(TOSA2SPIRV_PARSER, Abs)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_ABS,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("abs",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "ABS", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "ABS", outputStr);
}

TEST(TOSA2SPIRV_PARSER, BitwiseNot)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_BITWISE_NOT,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("bitwise_not",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_NOT", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_NOT", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Ceil)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_CEIL,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("ceil",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "CEIL", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "CEIL", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Clz)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_CLZ,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("clz",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "CLZ", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "CLZ", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Exp)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_EXP,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("exp",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "EXP", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "EXP", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Floor)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_FLOOR,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("floor",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "FLOOR", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "FLOOR", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Log)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_LOG,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("log",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "LOG", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "LOG", outputStr);
}

TEST(TOSA2SPIRV_PARSER, LogicalNot)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_LOGICAL_NOT,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_not",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_NOT", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_NOT", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Negate)
{
    int8_t input1_zp = 1;
    int8_t output_zp = 1;
    // Create Attribute
    TosaNegateAttribute attribute(input1_zp, output_zp);

    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_NEGATE,
                                                  Attribute::Attribute_NegateAttribute,
                                                  &attribute,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("negate",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "NEGATE", outputStr);
    spirvwriter::CheckConstant(DataType::int8_t, "NEGATE", outputStr, 1, 0);
    spirvwriter::CheckConstant(DataType::int8_t, "NEGATE", outputStr, 1, 1);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "NEGATE", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Reciprocal)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_RECIPROCAL,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reciprocal",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "RECIPROCAL", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "RECIPROCAL", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Rsqrt)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_RSQRT,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("rsqrt",
                                      "main",
                                      { op },
                                      { input1Tensor, outputTensor },
                                      { input1Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "RSQRT", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "RSQRT", outputStr);
}

