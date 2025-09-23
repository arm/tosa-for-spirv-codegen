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
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Gather)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string valuesName = "values";
    std::string indicesName = "indices";
    std::string outputName = "output";

    std::vector<int32_t> valuesShape = {1, 1, 1, 1};
    std::vector<int32_t> indicesShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto valuesTensor =
        std::make_unique<TosaSerializationTensor>(valuesName, valuesShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(valuesTensor));
    auto indicesTensor = std::make_unique<TosaSerializationTensor>(indicesName,
                                                                   indicesShape,
                                                                   DType::DType_INT32,
                                                                   std::vector<uint8_t>{});
    tensors.push_back(std::move(indicesTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_GATHER,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{valuesName, indicesName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("gather",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {valuesName, indicesName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAGATHER,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Scatter)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string values_inName = "values_in";
    std::string indicesName = "indices";
    std::string inputName = "input";
    std::string values_outName = "values_out";

    std::vector<int32_t> values_inShape = {1, 1, 1, 1};
    std::vector<int32_t> indicesShape = {1, 1, 1, 1};
    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> values_outShape = {1, 1, 1, 1};

    auto values_inTensor = std::make_unique<TosaSerializationTensor>(values_inName,
                                                                     values_inShape,
                                                                     DType::DType_INT8,
                                                                     std::vector<uint8_t>{});
    tensors.push_back(std::move(values_inTensor));
    auto indicesTensor = std::make_unique<TosaSerializationTensor>(indicesName,
                                                                   indicesShape,
                                                                   DType::DType_INT32,
                                                                   std::vector<uint8_t>{});
    tensors.push_back(std::move(indicesTensor));
    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto values_outTensor = std::make_unique<TosaSerializationTensor>(values_outName,
                                                                      values_outShape,
                                                                      DType::DType_INT8,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(values_outTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_SCATTER,
        Attribute::Attribute_NONE,
        nullptr,
        std::vector<std::string>{values_inName, indicesName, inputName},
        std::vector<std::string>{values_outName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("scatter",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {values_inName, indicesName, inputName},
                                      {values_outName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSASCATTER,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {});
}
