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
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Add)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_ADD,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("add",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAADD,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ArithmeticRightShift)
{
    bool round = true;
    // Create Attribute
    TosaArithmeticRightShiftAttribute attribute(round);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_ARITHMETIC_RIGHT_SHIFT,
                                                                Attribute::Attribute_ArithmeticRightShiftAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("arithmetic_right_shift",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAARITHMETIC_RIGHT_SHIFT,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::bool_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, BitwiseAnd)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_BITWISE_AND,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("bitwise_and",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSABITWISE_AND,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, BitwiseOr)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_BITWISE_OR,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("bitwise_or",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSABITWISE_OR,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, BitwiseXor)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_BITWISE_XOR,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("bitwise_xor",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSABITWISE_XOR,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, IntDiv)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_INTDIV,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("intdiv",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAINTDIV,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, LogicalAnd)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_LOGICAL_AND,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_and",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSALOGICAL_AND,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, LogicalLeftShift)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_LOGICAL_LEFT_SHIFT,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_left_shift",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSALOGICAL_LEFT_SHIFT,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, LogicalRightShift)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_LOGICAL_RIGHT_SHIFT,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_right_shift",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSALOGICAL_RIGHT_SHIFT,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, LogicalOr)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_LOGICAL_OR,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_or",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSALOGICAL_OR,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, LogicalXor)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_LOGICAL_XOR,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("logical_xor",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSALOGICAL_XOR,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Maximum)
{
    int32_t nan_mode = 1;
    // Create Attribute
    TosaMaximumAttribute attribute(static_cast<NanPropagationMode>(nan_mode));

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_MAXIMUM,
                                                                Attribute::Attribute_MaximumAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("maximum",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAMAXIMUM,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Minimum)
{
    int32_t nan_mode = 1;
    // Create Attribute
    TosaMinimumAttribute attribute(static_cast<NanPropagationMode>(nan_mode));

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_MINIMUM,
                                                                Attribute::Attribute_MinimumAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("minimum",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAMINIMUM,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Mul)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string shiftName = "shift";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> shiftShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto shiftTensor =
        std::make_unique<TosaSerializationTensor>(shiftName, shiftShape, DType::DType_INT8, std::vector<uint8_t>{1});
    tensors.push_back(std::move(shiftTensor));
    auto shiftOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{shiftName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(shiftOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_MUL,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{input1Name, input2Name, shiftName},
                                                          std::vector<std::string>{outputName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("mul",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAMUL,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Pow)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_POW,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("pow",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAPOW,
                           {{std::initializer_list<uint32_t>{}, DataType::float16_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::float16_t, {1, 1, 1, 1}}},
                           {{DataType::float16_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Sub)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_SUB,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("sub",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSASUB,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Table)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input1Name = "input1";
    std::string tableName = "table";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> tableShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto tableTensor =
        std::make_unique<TosaSerializationTensor>(tableName, tableShape, DType::DType_INT8, std::vector<uint8_t>{1});
    tensors.push_back(std::move(tableTensor));
    auto tableOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{tableName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(tableOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_TABLE,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, tableName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("table", "main", std::move(ops), std::move(tensors), std::move(shapes), {input1Name}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(
        spirvModule,
        TOSATABLE,
        {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}, {{1}, DataType::int8_t, {1}}},
        {{DataType::int8_t, {1, 1, 1, 1}}},
        {});
}
