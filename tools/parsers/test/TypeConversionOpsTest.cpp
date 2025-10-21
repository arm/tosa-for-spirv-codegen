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
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Cast)
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
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CAST,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("cast", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSACAST,
                           {{std::initializer_list<uint32_t>{}, DataType::bool_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Rescale)
{
    bool scale32 = true;
    int32_t rounding_mode = 1;
    bool per_channel = true;
    bool input_unsigned = true;
    bool output_unsigned = true;
    // Create Attribute
    TosaRescaleAttribute attribute(scale32,
                                   static_cast<const RoundingMode>(rounding_mode),
                                   per_channel,
                                   input_unsigned,
                                   output_unsigned);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string multiplierName = "multiplier";
    std::string shiftName = "shift";
    std::string input_zpName = "input_zp";
    std::string output_zpName = "output_zp";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> multiplierShape = {1};
    std::vector<int32_t> shiftShape = {1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> output_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto multiplierTensor = std::make_unique<TosaSerializationTensor>(multiplierName,
                                                                      multiplierShape,
                                                                      DType::DType_INT32,
                                                                      std::vector<uint8_t>{{1, 0, 0, 0}});
    tensors.push_back(std::move(multiplierTensor));
    auto multiplierOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                          Attribute::Attribute_NONE,
                                                                          nullptr,
                                                                          std::vector<std::string>{},
                                                                          std::vector<std::string>{multiplierName},
                                                                          TosaOpLocation{});
    ops.push_back(std::move(multiplierOp));
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
    auto input_zpTensor = std::make_unique<TosaSerializationTensor>(input_zpName,
                                                                    input_zpShape,
                                                                    DType::DType_INT8,
                                                                    std::vector<uint8_t>{1});
    tensors.push_back(std::move(input_zpTensor));
    auto input_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                        Attribute::Attribute_NONE,
                                                                        nullptr,
                                                                        std::vector<std::string>{},
                                                                        std::vector<std::string>{input_zpName},
                                                                        TosaOpLocation{});
    ops.push_back(std::move(input_zpOp));
    auto output_zpTensor = std::make_unique<TosaSerializationTensor>(output_zpName,
                                                                     output_zpShape,
                                                                     DType::DType_INT8,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(output_zpTensor));
    auto output_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{output_zpName},
                                                                         TosaOpLocation{});
    ops.push_back(std::move(output_zpOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_RESCALE,
        Attribute::Attribute_RescaleAttribute,
        &attribute,
        std::vector<std::string>{inputName, multiplierName, shiftName, input_zpName, output_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("rescale", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSARESCALE,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {{1}, DataType::int32_t, {1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::bool_t, {1}},
                            {{1}, DataType::int32_t, {1}},
                            {{1}, DataType::bool_t, {1}},
                            {{1}, DataType::bool_t, {1}},
                            {{1}, DataType::bool_t, {1}}});
}
