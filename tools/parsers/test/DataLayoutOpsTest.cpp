//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA 1.0.1. DO NOT EDIT!
// See tosa-for-spirv-codegen/python/code_generator.py and README

#include <AssemblyUtils.hpp>
#include <OpTestUtils.hpp>
#include <TosaSerializationParser.hpp>

#include <gtest/gtest.h>

using namespace tosa;
using namespace tfsc::parsers;
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Concat)
{
    int32_t axis = 1;
    // Create Attribute
    TosaConcatAttribute attribute(axis);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

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
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("concat", "main", std::move(ops), std::move(tensors), std::move(shapes), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSACONCAT,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Pad)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;
    std::string paddingName = "padding";
    std::vector<int64_t> paddingDims = {1, 1, 1, 1};
    std::vector<uint8_t> paddingBytes;
    TosaSerializationHandler::ConvertI64toU8(paddingDims, paddingBytes);
    auto paddingShape = std::make_unique<TosaSerializationShape>(paddingName, static_cast<uint32_t>(4), paddingBytes);
    shapes.push_back(std::move(paddingShape));
    auto paddingOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST_SHAPE,
                                                                       Attribute::Attribute_ConstShapeAttribute,
                                                                       nullptr,
                                                                       std::vector<std::string>{},
                                                                       std::vector<std::string>{paddingName},
                                                                       TosaOpLocation{});
    ops.push_back(std::move(paddingOp));

    std::string input1Name = "input1";
    std::string pad_constName = "pad_const";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> pad_constShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto pad_constTensor = std::make_unique<TosaSerializationTensor>(pad_constName,
                                                                     pad_constShape,
                                                                     DType::DType_BOOL,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(pad_constTensor));
    auto pad_constOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{pad_constName},
                                                                         TosaOpLocation{});
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
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("pad", "main", std::move(ops), std::move(tensors), std::move(shapes), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAPAD,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}},
                            {{1, 1, 1, 1}, DataType::int32_t, {4}},
                            {{1}, DataType::bool_t, {1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Reshape)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;
    std::string shapeName = "shape";
    std::vector<int64_t> shapeDims = {1, 1, 1, 1};
    std::vector<uint8_t> shapeBytes;
    TosaSerializationHandler::ConvertI64toU8(shapeDims, shapeBytes);
    auto shapeShape = std::make_unique<TosaSerializationShape>(shapeName, static_cast<uint32_t>(4), shapeBytes);
    shapes.push_back(std::move(shapeShape));
    auto shapeOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST_SHAPE,
                                                                     Attribute::Attribute_ConstShapeAttribute,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{shapeName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(shapeOp));

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
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_RESHAPE,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, shapeName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("reshape", "main", std::move(ops), std::move(tensors), std::move(shapes), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(
        spirvModule,
        TOSARESHAPE,
        {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}, {{1, 1, 1, 1}, DataType::int32_t, {4}}},
        {{DataType::bool_t, {1, 1, 1, 1}}},
        {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Reverse)
{
    int32_t axis = 1;
    // Create Attribute
    TosaReverseAttribute attribute(axis);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

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
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("reverse", "main", std::move(ops), std::move(tensors), std::move(shapes), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAREVERSE,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Slice)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;
    std::string startName = "start";
    std::vector<int64_t> startDims = {1, 1, 1, 1};
    std::vector<uint8_t> startBytes;
    TosaSerializationHandler::ConvertI64toU8(startDims, startBytes);
    auto startShape = std::make_unique<TosaSerializationShape>(startName, static_cast<uint32_t>(4), startBytes);
    shapes.push_back(std::move(startShape));
    auto startOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST_SHAPE,
                                                                     Attribute::Attribute_ConstShapeAttribute,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{startName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(startOp));
    std::string sizeName = "size";
    std::vector<int64_t> sizeDims = {1, 1, 1, 1};
    std::vector<uint8_t> sizeBytes;
    TosaSerializationHandler::ConvertI64toU8(sizeDims, sizeBytes);
    auto sizeShape = std::make_unique<TosaSerializationShape>(sizeName, static_cast<uint32_t>(4), sizeBytes);
    shapes.push_back(std::move(sizeShape));
    auto sizeOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST_SHAPE,
                                                                    Attribute::Attribute_ConstShapeAttribute,
                                                                    nullptr,
                                                                    std::vector<std::string>{},
                                                                    std::vector<std::string>{sizeName},
                                                                    TosaOpLocation{});
    ops.push_back(std::move(sizeOp));

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
    auto op =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_SLICE,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{input1Name, startName, sizeName},
                                                          std::vector<std::string>{outputName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("slice", "main", std::move(ops), std::move(tensors), std::move(shapes), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSASLICE,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}},
                            {{1, 1, 1, 1}, DataType::int32_t, {4}},
                            {{1, 1, 1, 1}, DataType::int32_t, {4}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Tile)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;
    std::string multiplesName = "multiples";
    std::vector<int64_t> multiplesDims = {1, 1, 1, 1};
    std::vector<uint8_t> multiplesBytes;
    TosaSerializationHandler::ConvertI64toU8(multiplesDims, multiplesBytes);
    auto multiplesShape =
        std::make_unique<TosaSerializationShape>(multiplesName, static_cast<uint32_t>(4), multiplesBytes);
    shapes.push_back(std::move(multiplesShape));
    auto multiplesOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST_SHAPE,
                                                                         Attribute::Attribute_ConstShapeAttribute,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{multiplesName},
                                                                         TosaOpLocation{});
    ops.push_back(std::move(multiplesOp));

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
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_TILE,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, multiplesName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("tile", "main", std::move(ops), std::move(tensors), std::move(shapes), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(
        spirvModule,
        TOSATILE,
        {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}, {{1, 1, 1, 1}, DataType::int32_t, {4}}},
        {{DataType::bool_t, {1, 1, 1, 1}}},
        {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Transpose)
{
    std::vector<int32_t> perms = {1, 1, 1, 1};
    // Create Attribute
    TosaTransposeAttribute attribute(perms);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

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
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("transpose", "main", std::move(ops), std::move(tensors), std::move(shapes), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSATRANSPOSE,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {{{1, 1, 1, 1}, DataType::int32_t, {4}}});
}
