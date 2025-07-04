//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA 1.0.0. DO NOT EDIT!
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
    TosaConcatAttribute attribute(axis);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONCAT,
                                                                Attribute::Attribute_ConcatAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{input1Name},
                                                                std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("concat", "main", std::move(ops), std::move(tensors), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "CONCAT", outputStr));

    EXPECT_TRUE(testutils::CheckConstant(DataType::int32_t, "CONCAT", outputStr, 1, 0));
    EXPECT_TRUE(testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "CONCAT", outputStr));
}

TEST(TOSA2SPIRV_PARSER, Pad)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string input1Name = "input1";
    std::string paddingName = "padding";
    std::string pad_constName = "pad_const";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> paddingShape = {1, 1, 1, 1};
    std::vector<int32_t> pad_constShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto paddingTensor = std::make_unique<TosaSerializationTensor>(paddingName,
                                                                   paddingShape,
                                                                   DType::DType_INT32,
                                                                   std::vector<uint8_t>{1, 1, 1, 1});
    tensors.push_back(std::move(paddingTensor));
    auto paddingOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                       Attribute::Attribute_NONE,
                                                                       nullptr,
                                                                       std::vector<std::string>{},
                                                                       std::vector<std::string>{paddingName});
    ops.push_back(std::move(paddingOp));
    auto pad_constTensor = std::make_unique<TosaSerializationTensor>(pad_constName,
                                                                     pad_constShape,
                                                                     DType::DType_BOOL,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(pad_constTensor));
    auto pad_constOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{pad_constName});
    ops.push_back(std::move(pad_constOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_PAD,
        Attribute::Attribute_NONE,
        nullptr,
        std::vector<std::string>{input1Name, paddingName, pad_constName},
        std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("pad", "main", std::move(ops), std::move(tensors), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "PAD", outputStr));

    EXPECT_TRUE(testutils::CheckGraphConstant({1, 1, 1, 1}, DataType::int32_t, "PAD", outputStr, 1, 0));

    EXPECT_TRUE(testutils::CheckConstCompositeTensor({1}, "PAD", outputStr, 2, "bool"));

    EXPECT_TRUE(testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "PAD", outputStr));
}

TEST(TOSA2SPIRV_PARSER, Reshape)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string input1Name = "input1";
    std::string shapeName = "shape";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> shapeShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto shapeTensor = std::make_unique<TosaSerializationTensor>(shapeName,
                                                                 shapeShape,
                                                                 DType::DType_INT32,
                                                                 std::vector<uint8_t>{1, 1, 1, 1});
    tensors.push_back(std::move(shapeTensor));
    auto shapeOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{shapeName});
    ops.push_back(std::move(shapeOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_RESHAPE,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, shapeName},
                                                                std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reshape",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      {input1Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "RESHAPE", outputStr));

    EXPECT_TRUE(testutils::CheckGraphConstant({1, 1, 1, 1}, DataType::int32_t, "RESHAPE", outputStr, 1, 0));

    EXPECT_TRUE(testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "RESHAPE", outputStr));
}

TEST(TOSA2SPIRV_PARSER, Reverse)
{
    int32_t axis = 1;
    // Create Attribute
    TosaReverseAttribute attribute(axis);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_REVERSE,
                                                                Attribute::Attribute_ReverseAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{input1Name},
                                                                std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reverse",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      {input1Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "REVERSE", outputStr));

    EXPECT_TRUE(testutils::CheckConstant(DataType::int32_t, "REVERSE", outputStr, 1, 0));
    EXPECT_TRUE(testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "REVERSE", outputStr));
}

TEST(TOSA2SPIRV_PARSER, Slice)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string input1Name = "input1";
    std::string startName = "start";
    std::string sizeName = "size";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> startShape = {1, 1, 1, 1};
    std::vector<int32_t> sizeShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto startTensor = std::make_unique<TosaSerializationTensor>(startName,
                                                                 startShape,
                                                                 DType::DType_INT32,
                                                                 std::vector<uint8_t>{1, 1, 1, 1});
    tensors.push_back(std::move(startTensor));
    auto startOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{startName});
    ops.push_back(std::move(startOp));
    auto sizeTensor = std::make_unique<TosaSerializationTensor>(sizeName,
                                                                sizeShape,
                                                                DType::DType_INT32,
                                                                std::vector<uint8_t>{1, 1, 1, 1});
    tensors.push_back(std::move(sizeTensor));
    auto sizeOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                    Attribute::Attribute_NONE,
                                                                    nullptr,
                                                                    std::vector<std::string>{},
                                                                    std::vector<std::string>{sizeName});
    ops.push_back(std::move(sizeOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_SLICE,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{input1Name, startName, sizeName},
                                                          std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("slice", "main", std::move(ops), std::move(tensors), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "SLICE", outputStr));

    EXPECT_TRUE(testutils::CheckGraphConstant({1, 1, 1, 1}, DataType::int32_t, "SLICE", outputStr, 1, 0));

    EXPECT_TRUE(testutils::CheckGraphConstant({1, 1, 1, 1}, DataType::int32_t, "SLICE", outputStr, 2, 1));

    EXPECT_TRUE(testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "SLICE", outputStr));
}

TEST(TOSA2SPIRV_PARSER, Tile)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string input1Name = "input1";
    std::string multiplesName = "multiples";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> multiplesShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto multiplesTensor = std::make_unique<TosaSerializationTensor>(multiplesName,
                                                                     multiplesShape,
                                                                     DType::DType_INT32,
                                                                     std::vector<uint8_t>{1, 1, 1, 1});
    tensors.push_back(std::move(multiplesTensor));
    auto multiplesOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{multiplesName});
    ops.push_back(std::move(multiplesOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_TILE,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, multiplesName},
                                                                std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("tile", "main", std::move(ops), std::move(tensors), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "TILE", outputStr));

    EXPECT_TRUE(testutils::CheckGraphConstant({1, 1, 1, 1}, DataType::int32_t, "TILE", outputStr, 1, 0));

    EXPECT_TRUE(testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "TILE", outputStr));
}

TEST(TOSA2SPIRV_PARSER, Transpose)
{
    std::vector<int32_t> perms = {1, 1, 1, 1};
    // Create Attribute
    TosaTransposeAttribute attribute(perms);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string input1Name = "input1";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_TRANSPOSE,
                                                                Attribute::Attribute_TransposeAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{input1Name},
                                                                std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("transpose",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      {input1Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "TRANSPOSE", outputStr));

    EXPECT_TRUE(testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "TRANSPOSE", outputStr, 0));
    EXPECT_TRUE(testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "TRANSPOSE", outputStr));
}
