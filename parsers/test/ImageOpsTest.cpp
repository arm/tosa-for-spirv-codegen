//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
// See tosa2spirv/python/code_generator.py and README

#include <AssemblyUtils.hpp>
#include <OpTestUtils.hpp>
#include <TosaSerializationParser.hpp>

#include <gtest/gtest.h>

using namespace tosa;
using namespace tosa2spirv::parsers;
TEST(TOSA2SPIRV_PARSER, Resize)
{
    std::vector<int16_t> scale = {1, 1, 1, 1};
    std::vector<int16_t> offset = {1, 1};
    std::vector<int16_t> border = {1, 1};
    ResizeMode mode = ResizeMode_BILINEAR;
    // Create Attribute
    TosaResizeAttribute attribute(scale, offset, border, mode);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_RESIZE,
                                                  Attribute::Attribute_ResizeAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("resize", "main", {op}, {inputTensor, outputTensor}, {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "RESIZE", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "RESIZE", outputStr, 2);
    testutils::CheckConstCompositeTensor({1, 1}, "RESIZE", outputStr, 3);
    testutils::CheckConstCompositeTensor({1, 1}, "RESIZE", outputStr, 4);
    testutils::CheckConstant(DataType::int32_t, "RESIZE", outputStr, 1, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "RESIZE", outputStr);
}
