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
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ReduceAll)
{
    int32_t axis = 1;
    // Create Attribute
    TosaReduceAllAttribute attribute(axis);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_REDUCE_ALL,
                                                                Attribute::Attribute_ReduceAllAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("reduce_all", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAREDUCE_ALL,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ReduceAny)
{
    int32_t axis = 1;
    // Create Attribute
    TosaReduceAnyAttribute attribute(axis);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_REDUCE_ANY,
                                                                Attribute::Attribute_ReduceAnyAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("reduce_any", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAREDUCE_ANY,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::bool_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ReduceMax)
{
    int32_t axis = 1;
    int32_t nan_mode = 1;
    // Create Attribute
    TosaReduceMaxAttribute attribute(axis, static_cast<NanPropagationMode>(nan_mode));

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
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_REDUCE_MAX,
                                                                Attribute::Attribute_ReduceMaxAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("reduce_max", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAREDUCE_MAX,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}, {{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ReduceMin)
{
    int32_t axis = 1;
    int32_t nan_mode = 1;
    // Create Attribute
    TosaReduceMinAttribute attribute(axis, static_cast<NanPropagationMode>(nan_mode));

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
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_REDUCE_MIN,
                                                                Attribute::Attribute_ReduceMinAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("reduce_min", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAREDUCE_MIN,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}, {{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ReduceProduct)
{
    int32_t axis = 1;
    // Create Attribute
    TosaReduceProductAttribute attribute(axis);

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
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_REDUCE_PRODUCT,
                                                                Attribute::Attribute_ReduceProductAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("reduce_product",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {inputName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAREDUCE_PRODUCT,
                           {{std::initializer_list<uint32_t>{}, DataType::float16_t, {1, 1, 1, 1}}},
                           {{DataType::float16_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ReduceSum)
{
    int32_t axis = 1;
    // Create Attribute
    TosaReduceSumAttribute attribute(axis);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_REDUCE_SUM,
                                                                Attribute::Attribute_ReduceSumAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("reduce_sum", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAREDUCE_SUM,
                           {{std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}
