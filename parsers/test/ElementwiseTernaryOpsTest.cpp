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
TEST(TOSA2SPIRV_PARSER, Select)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string input3Name = "input3";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {1, 1, 1, 1};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> input3Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto input1Tensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input1Tensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto input3Tensor =
        std::make_unique<TosaSerializationTensor>(input3Name, input3Shape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(input3Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_BOOL, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_SELECT,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{input1Name, input2Name, input3Name},
                                                          std::vector<std::string>{outputName});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("select",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      {input1Name, input2Name, input3Name},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "SELECT", outputStr));
    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "SELECT", outputStr));
    EXPECT_TRUE(testutils::CheckInputTensor({1, 1, 1, 1}, DataType::bool_t, "SELECT", outputStr));
    EXPECT_TRUE(testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::bool_t, "SELECT", outputStr));
}
