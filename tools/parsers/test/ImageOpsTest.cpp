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
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Resize)
{
    int32_t mode = 1;
    // Create Attribute
    TosaResizeAttribute attribute(static_cast<ResizeMode>(mode));

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;
    std::string scaleName = "scale";
    std::vector<int64_t> scaleDims = {1, 1, 1, 1};
    std::vector<uint8_t> scaleBytes;
    TosaSerializationHandler::ConvertI64toU8(scaleDims, scaleBytes);
    auto scaleShape = std::make_unique<TosaSerializationShape>(scaleName, static_cast<uint32_t>(4), scaleBytes);
    shapes.push_back(std::move(scaleShape));
    auto scaleOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST_SHAPE,
                                                                     Attribute::Attribute_ConstShapeAttribute,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{scaleName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(scaleOp));
    std::string offsetName = "offset";
    std::vector<int64_t> offsetDims = {1, 1};
    std::vector<uint8_t> offsetBytes;
    TosaSerializationHandler::ConvertI64toU8(offsetDims, offsetBytes);
    auto offsetShape = std::make_unique<TosaSerializationShape>(offsetName, static_cast<uint32_t>(2), offsetBytes);
    shapes.push_back(std::move(offsetShape));
    auto offsetOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST_SHAPE,
                                                                      Attribute::Attribute_ConstShapeAttribute,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{offsetName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(offsetOp));
    std::string borderName = "border";
    std::vector<int64_t> borderDims = {1, 1};
    std::vector<uint8_t> borderBytes;
    TosaSerializationHandler::ConvertI64toU8(borderDims, borderBytes);
    auto borderShape = std::make_unique<TosaSerializationShape>(borderName, static_cast<uint32_t>(2), borderBytes);
    shapes.push_back(std::move(borderShape));
    auto borderOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST_SHAPE,
                                                                      Attribute::Attribute_ConstShapeAttribute,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{borderName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(borderOp));

    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_RESIZE,
        Attribute::Attribute_ResizeAttribute,
        &attribute,
        std::vector<std::string>{inputName, scaleName, offsetName, borderName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("resize", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSARESIZE,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {{1, 1, 1, 1}, DataType::int32_t, {4}},
                            {{1, 1}, DataType::int32_t, {2}},
                            {{1, 1}, DataType::int32_t, {2}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}});
}
