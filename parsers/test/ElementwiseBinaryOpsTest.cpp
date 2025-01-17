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

TEST(TOSA2SPIRV_PARSER, Add)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_ADD,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("add",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "ADD", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "ADD", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "ADD", outputStr);
}

TEST(TOSA2SPIRV_PARSER, ArithmeticRightShift)
{
    bool round = true;
    // Create Attribute
    TosaArithmeticRightShiftAttribute attribute(round);

    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_ARITHMETIC_RIGHT_SHIFT,
                                                  Attribute::Attribute_ArithmeticRightShiftAttribute,
                                                  &attribute,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("arithmetic_right_shift",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "ARITHMETIC_RIGHT_SHIFT", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "ARITHMETIC_RIGHT_SHIFT", outputStr);
    spirvwriter::CheckBoolConstant(DataType::bool_t, "ARITHMETIC_RIGHT_SHIFT", outputStr, true, 0);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "ARITHMETIC_RIGHT_SHIFT", outputStr);
}

TEST(TOSA2SPIRV_PARSER, BitwiseAnd)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_BITWISE_AND,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("bitwise_and",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_AND", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_AND", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_AND", outputStr);
}

TEST(TOSA2SPIRV_PARSER, BitwiseOr)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_BITWISE_OR,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("bitwise_or",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_OR", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_OR", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_OR", outputStr);
}

TEST(TOSA2SPIRV_PARSER, BitwiseXor)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_BITWISE_XOR,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("bitwise_xor",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_XOR", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_XOR", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "BITWISE_XOR", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Intdiv)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_INTDIV,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("intdiv",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "INTDIV", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "INTDIV", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "INTDIV", outputStr);
}

TEST(TOSA2SPIRV_PARSER, LogicalAnd)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_LOGICAL_AND,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_and",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_AND", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_AND", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_AND", outputStr);
}

TEST(TOSA2SPIRV_PARSER, LogicalLeftShift)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_LOGICAL_LEFT_SHIFT,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_left_shift",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "LOGICAL_LEFT_SHIFT", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "LOGICAL_LEFT_SHIFT", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "LOGICAL_LEFT_SHIFT", outputStr);
}

TEST(TOSA2SPIRV_PARSER, LogicalRightShift)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_LOGICAL_RIGHT_SHIFT,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_right_shift",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "LOGICAL_RIGHT_SHIFT", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "LOGICAL_RIGHT_SHIFT", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "LOGICAL_RIGHT_SHIFT", outputStr);
}

TEST(TOSA2SPIRV_PARSER, LogicalOr)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_LOGICAL_OR,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_or",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_OR", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_OR", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_OR", outputStr);
}

TEST(TOSA2SPIRV_PARSER, LogicalXor)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_LOGICAL_XOR,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_xor",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_XOR", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_XOR", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "LOGICAL_XOR", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Maximum)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_MAXIMUM,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("maximum",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "MAXIMUM", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "MAXIMUM", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "MAXIMUM", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Minimum)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_MINIMUM,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("minimum",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "MINIMUM", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "MINIMUM", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "MINIMUM", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Mul)
{
    int8_t shift = 1;
    // Create Attribute
    TosaMulAttribute attribute(shift);

    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT8, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_MUL,
                                                  Attribute::Attribute_MulAttribute,
                                                  &attribute,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("mul",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "MUL", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "MUL", outputStr);
    spirvwriter::CheckConstant(DataType::int8_t, "MUL", outputStr, 1, 0);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "MUL", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Pow)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_FP16, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_FP16, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_FP16, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_POW,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("pow",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "POW", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::float16_t, "POW", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::float16_t, "POW", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Sub)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1,1,1,1};
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_SUB,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { input1Name, input2Name },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("sub",
                                      "main",
                                      { op },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "SUB", outputStr);
    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "SUB", outputStr);
    spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "SUB", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Table)
{
    std::vector<int16_t> table = {1, 1, 1, 1};
    // Create Attribute
    TosaTableAttribute attribute(table);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_TABLE,
                                                  Attribute::Attribute_TableAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("table",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    spirvwriter::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "TABLE", outputStr);
        spirvwriter::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "TABLE", outputStr);
}

