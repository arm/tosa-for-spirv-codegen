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
TEST(TOSA2SPIRV_PARSER, Concat)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_CONCAT,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  {input1Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("concat", "main", {op}, {input1Tensor, outputTensor}, {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "CONCAT", outputStr);
    testutils::CheckConstant(DataType::int32_t, "CONCAT", outputStr, 1, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "CONCAT", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Pad)
{
    std::vector<int32_t> padding = {1, 1, 1, 1};
    int32_t pad_const_int = 1;
    float pad_const_fp = 1;
    // Create Attribute
    TosaPadAttribute attribute(padding, pad_const_int, pad_const_fp);

    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_PAD,
                                                  Attribute::Attribute_PadAttribute,
                                                  &attribute,
                                                  {input1Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("pad", "main", {op}, {input1Tensor, outputTensor}, {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "PAD", outputStr);
    testutils::CheckBoolConstant(DataType::bool_t, "PAD", outputStr, true, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "PAD", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Reshape)
{
    std::vector<int32_t> shape = {1, 1, 1, 1};
    // Create Attribute
    TosaReshapeAttribute attribute(shape);

    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_RESHAPE,
                                                  Attribute::Attribute_ReshapeAttribute,
                                                  &attribute,
                                                  {input1Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reshape",
                                      "main",
                                      {op},
                                      {input1Tensor, outputTensor},
                                      {input1Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "RESHAPE", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "RESHAPE", outputStr, 1);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "RESHAPE", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Reverse)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_REVERSE,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reverse", "main", {op}, {inputTensor, outputTensor}, {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "REVERSE", outputStr);
    testutils::CheckConstant(DataType::int32_t, "REVERSE", outputStr, 1, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "REVERSE", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Slice)
{
    std::vector<int32_t> start = {1, 1, 1, 1};
    std::vector<int32_t> size = {1, 1, 1, 1};
    // Create Attribute
    TosaSliceAttribute attribute(start, size);

    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_SLICE,
                                                  Attribute::Attribute_SliceAttribute,
                                                  &attribute,
                                                  {input1Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("slice", "main", {op}, {input1Tensor, outputTensor}, {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "SLICE", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "SLICE", outputStr, 0);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "SLICE", outputStr, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "SLICE", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Tile)
{
    std::vector<int32_t> multiples = {1, 1, 1, 1};
    // Create Attribute
    TosaTileAttribute attribute(multiples);

    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_TILE,
                                                  Attribute::Attribute_TileAttribute,
                                                  &attribute,
                                                  {input1Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("tile", "main", {op}, {input1Tensor, outputTensor}, {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "TILE", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "TILE", outputStr, 1);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "TILE", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Transpose)
{
    std::vector<int32_t> perms = {1, 1, 1, 1};
    // Create Attribute
    TosaTransposeAttribute attribute(perms);

    // Create Tensors
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_BOOL, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_BOOL, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_TRANSPOSE,
                                                  Attribute::Attribute_TransposeAttribute,
                                                  &attribute,
                                                  {input1Name},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("transpose",
                                      "main",
                                      {op},
                                      {input1Tensor, outputTensor},
                                      {input1Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "TRANSPOSE", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "TRANSPOSE", outputStr, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "TRANSPOSE", outputStr);
}
