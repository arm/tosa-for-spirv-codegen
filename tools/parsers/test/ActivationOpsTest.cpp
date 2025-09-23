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
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Clamp)
{
    int32_t min_val = 1;
    int32_t max_val = 1;
    int32_t nan_mode = 1;
    // Create Attribute
    TosaClampAttribute attribute({static_cast<unsigned char>(min_val)},
                                 {static_cast<unsigned char>(max_val)},
                                 static_cast<NanPropagationMode>(nan_mode));

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CLAMP,
                                                                Attribute::Attribute_ClampAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("clamp", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSACLAMP,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int8_t, {1}}, {{1}, DataType::int8_t, {1}}, {{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Erf)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_ERF,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("erf", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAERF,
                           {{std::initializer_list<uint32_t>{}, DataType::float16_t, {1, 1, 1, 1}}},
                           {{DataType::float16_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Sigmoid)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_SIGMOID,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("sigmoid", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSASIGMOID,
                           {{std::initializer_list<uint32_t>{}, DataType::float16_t, {1, 1, 1, 1}}},
                           {{DataType::float16_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Tanh)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_FP16, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_TANH,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("tanh", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSATANH,
                           {{std::initializer_list<uint32_t>{}, DataType::float16_t, {1, 1, 1, 1}}},
                           {{DataType::float16_t, {1, 1, 1, 1}}},
                           {});
}
