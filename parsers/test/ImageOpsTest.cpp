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
TEST(TOSA2SPIRV_PARSER, Resize)
{
    int32_t mode = 1;
    // Create Attribute
    TosaResizeAttribute attribute(static_cast<ResizeMode>(mode));

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string inputName = "input";
    std::string scaleName = "scale";
    std::string offsetName = "offset";
    std::string borderName = "border";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> scaleShape = {1, 1, 1, 1};
    std::vector<int32_t> offsetShape = {1, 1};
    std::vector<int32_t> borderShape = {1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto scaleTensor = std::make_unique<TosaSerializationTensor>(scaleName,
                                                                 scaleShape,
                                                                 DType::DType_INT32,
                                                                 std::vector<uint8_t>{1, 1, 1, 1});
    tensors.push_back(std::move(scaleTensor));
    auto scaleOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{scaleName});
    ops.push_back(std::move(scaleOp));
    auto offsetTensor = std::make_unique<TosaSerializationTensor>(offsetName,
                                                                  offsetShape,
                                                                  DType::DType_INT32,
                                                                  std::vector<uint8_t>{1, 1});
    tensors.push_back(std::move(offsetTensor));
    auto offsetOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{offsetName});
    ops.push_back(std::move(offsetOp));
    auto borderTensor = std::make_unique<TosaSerializationTensor>(borderName,
                                                                  borderShape,
                                                                  DType::DType_INT32,
                                                                  std::vector<uint8_t>{1, 1});
    tensors.push_back(std::move(borderTensor));
    auto borderOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{borderName});
    ops.push_back(std::move(borderOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_RESIZE,
        Attribute::Attribute_ResizeAttribute,
        &attribute,
        std::vector<std::string>{inputName, scaleName, offsetName, borderName},
        std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("resize", "main", std::move(ops), std::move(tensors), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "RESIZE", outputStr);

    testutils::CheckGraphConstant({1, 1, 1, 1}, DataType::int32_t, "RESIZE", outputStr, 2, 0);

    testutils::CheckGraphConstant({1, 1}, DataType::int32_t, "RESIZE", outputStr, 3, 1);

    testutils::CheckGraphConstant({1, 1}, DataType::int32_t, "RESIZE", outputStr, 4, 2);

    testutils::CheckConstant(DataType::int32_t, "RESIZE", outputStr, 1, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "RESIZE", outputStr);
}
