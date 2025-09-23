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
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Equal)
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
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_EQUAL,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("equal",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAEQUAL,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Greater)
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
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_GREATER,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("greater",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAGREATER,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, GreaterEqual)
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
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_GREATER_EQUAL,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{input1Name, input2Name},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("greater_equal",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAGREATER_EQUAL,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {});
}
