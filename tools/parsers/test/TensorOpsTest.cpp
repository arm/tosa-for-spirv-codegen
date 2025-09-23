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
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ArgMax)
{
    int32_t axis = 1;
    int32_t nan_mode = 1;
    // Create Attribute
    TosaArgMaxAttribute attribute(axis, static_cast<NanPropagationMode>(nan_mode));

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
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_ARGMAX,
                                                                Attribute::Attribute_ArgMaxAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("argmax", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAARGMAX,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::int32_t, {1}}, {{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, AvgPool2d)
{
    std::vector<int32_t> kernel = {1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> pad = {1, 1, 1, 1};
    int32_t acc_type = 1;
    // Create Attribute
    TosaAvgPool2dAttribute attribute(kernel, stride, pad, DType_INT32);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string input_zpName = "input_zp";
    std::string output_zpName = "output_zp";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> output_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
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
        Op::Op_AVG_POOL2D,
        Attribute::Attribute_AvgPool2dAttribute,
        &attribute,
        std::vector<std::string>{inputName, input_zpName, output_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("avg_pool2d", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAAVG_POOL2D,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {{{1, 1}, DataType::int32_t, {2}},
                            {{1, 1}, DataType::int32_t, {2}},
                            {{1, 1, 1, 1}, DataType::int32_t, {4}},
                            {{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Conv2d)
{
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> dilation = {1, 1};
    int32_t acc_type = 1;
    bool local_bound = true;
    // Create Attribute
    TosaConv2dAttribute attribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto weightTensor =
        std::make_unique<TosaSerializationTensor>(weightName, weightShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(weightTensor));
    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName, biasShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(biasTensor));
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
    auto weight_zpTensor = std::make_unique<TosaSerializationTensor>(weight_zpName,
                                                                     weight_zpShape,
                                                                     DType::DType_INT8,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(weight_zpTensor));
    auto weight_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{weight_zpName},
                                                                         TosaOpLocation{});
    ops.push_back(std::move(weight_zpOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &attribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("conv2d",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSACONV2D,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1, 1, 1, 1}, DataType::int32_t, {4}},
                            {{1, 1}, DataType::int32_t, {2}},
                            {{1, 1}, DataType::int32_t, {2}},
                            {{1}, DataType::int32_t, {1}},
                            {{1}, DataType::bool_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Conv3d)
{
    std::vector<int32_t> pad = {1, 1, 1, 1, 1, 1};
    std::vector<int32_t> stride = {1, 1, 1};
    std::vector<int32_t> dilation = {1, 1, 1};
    int32_t acc_type = 1;
    bool local_bound = true;
    // Create Attribute
    TosaConv3dAttribute attribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto weightTensor =
        std::make_unique<TosaSerializationTensor>(weightName, weightShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(weightTensor));
    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName, biasShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(biasTensor));
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
    auto weight_zpTensor = std::make_unique<TosaSerializationTensor>(weight_zpName,
                                                                     weight_zpShape,
                                                                     DType::DType_INT8,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(weight_zpTensor));
    auto weight_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{weight_zpName},
                                                                         TosaOpLocation{});
    ops.push_back(std::move(weight_zpOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV3D,
        Attribute::Attribute_Conv3dAttribute,
        &attribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("conv3d",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSACONV3D,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1, 1, 1, 1, 1, 1}, DataType::int32_t, {6}},
                            {{1, 1, 1}, DataType::int32_t, {3}},
                            {{1, 1, 1}, DataType::int32_t, {3}},
                            {{1}, DataType::int32_t, {1}},
                            {{1}, DataType::bool_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, DepthwiseConv2d)
{
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> dilation = {1, 1};
    int32_t acc_type = 1;
    bool local_bound = true;
    // Create Attribute
    TosaDepthwiseConv2dAttribute attribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto weightTensor =
        std::make_unique<TosaSerializationTensor>(weightName, weightShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(weightTensor));
    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName, biasShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(biasTensor));
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
    auto weight_zpTensor = std::make_unique<TosaSerializationTensor>(weight_zpName,
                                                                     weight_zpShape,
                                                                     DType::DType_INT8,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(weight_zpTensor));
    auto weight_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{weight_zpName},
                                                                         TosaOpLocation{});
    ops.push_back(std::move(weight_zpOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_DEPTHWISE_CONV2D,
        Attribute::Attribute_DepthwiseConv2dAttribute,
        &attribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("depthwise_conv2d",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSADEPTHWISE_CONV2D,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1, 1, 1, 1}, DataType::int32_t, {4}},
                            {{1, 1}, DataType::int32_t, {2}},
                            {{1, 1}, DataType::int32_t, {2}},
                            {{1}, DataType::int32_t, {1}},
                            {{1}, DataType::bool_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Fft2d)
{
    bool inverse = true;
    bool local_bound = true;
    // Create Attribute
    TosaFFT2dAttribute attribute(inverse, local_bound);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input_realName = "input_real";
    std::string input_imagName = "input_imag";
    std::string output_realName = "output_real";
    std::string output_imagName = "output_imag";

    std::vector<int32_t> input_realShape = {1, 1, 1, 1};
    std::vector<int32_t> input_imagShape = {1, 1, 1, 1};
    std::vector<int32_t> output_realShape = {1, 1, 1, 1};
    std::vector<int32_t> output_imagShape = {1, 1, 1, 1};

    auto input_realTensor = std::make_unique<TosaSerializationTensor>(input_realName,
                                                                      input_realShape,
                                                                      DType::DType_FP32,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(input_realTensor));
    auto input_imagTensor = std::make_unique<TosaSerializationTensor>(input_imagName,
                                                                      input_imagShape,
                                                                      DType::DType_FP32,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(input_imagTensor));
    auto output_realTensor = std::make_unique<TosaSerializationTensor>(output_realName,
                                                                       output_realShape,
                                                                       DType::DType_FP32,
                                                                       std::vector<uint8_t>{});
    tensors.push_back(std::move(output_realTensor));
    auto output_imagTensor = std::make_unique<TosaSerializationTensor>(output_imagName,
                                                                       output_imagShape,
                                                                       DType::DType_FP32,
                                                                       std::vector<uint8_t>{});
    tensors.push_back(std::move(output_imagTensor));

    // Create Operator
    auto op =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_FFT2D,
                                                          Attribute::Attribute_FFT2dAttribute,
                                                          &attribute,
                                                          std::vector<std::string>{input_realName, input_imagName},
                                                          std::vector<std::string>{output_realName, output_imagName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("fft2d",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input_realName, input_imagName},
                                      {output_realName, output_imagName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAFFT2D,
                           {{std::initializer_list<uint32_t>{}, DataType::float32_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::float32_t, {1, 1, 1, 1}}},
                           {{DataType::float32_t, {1, 1, 1, 1}}, {DataType::float32_t, {1, 1, 1, 1}}},
                           {{{1}, DataType::bool_t, {1}}, {{1}, DataType::bool_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Matmul)
{
    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string AName = "A";
    std::string BName = "B";
    std::string A_zpName = "A_zp";
    std::string B_zpName = "B_zp";
    std::string outputName = "output";

    std::vector<int32_t> AShape = {1, 1, 1, 1};
    std::vector<int32_t> BShape = {1, 1, 1, 1};
    std::vector<int32_t> A_zpShape = {1};
    std::vector<int32_t> B_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto ATensor = std::make_unique<TosaSerializationTensor>(AName, AShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(ATensor));
    auto BTensor = std::make_unique<TosaSerializationTensor>(BName, BShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(BTensor));
    auto A_zpTensor =
        std::make_unique<TosaSerializationTensor>(A_zpName, A_zpShape, DType::DType_INT8, std::vector<uint8_t>{1});
    tensors.push_back(std::move(A_zpTensor));
    auto A_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                    Attribute::Attribute_NONE,
                                                                    nullptr,
                                                                    std::vector<std::string>{},
                                                                    std::vector<std::string>{A_zpName},
                                                                    TosaOpLocation{});
    ops.push_back(std::move(A_zpOp));
    auto B_zpTensor =
        std::make_unique<TosaSerializationTensor>(B_zpName, B_zpShape, DType::DType_INT8, std::vector<uint8_t>{1});
    tensors.push_back(std::move(B_zpTensor));
    auto B_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                    Attribute::Attribute_NONE,
                                                                    nullptr,
                                                                    std::vector<std::string>{},
                                                                    std::vector<std::string>{B_zpName},
                                                                    TosaOpLocation{});
    ops.push_back(std::move(B_zpOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_MATMUL,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{AName, BName, A_zpName, B_zpName},
                                                          std::vector<std::string>{outputName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("matmul", "main", std::move(ops), std::move(tensors), std::move(shapes), {AName, BName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAMATMUL,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, MaxPool2d)
{
    std::vector<int32_t> kernel = {1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> pad = {1, 1, 1, 1};
    int32_t nan_mode = 1;
    // Create Attribute
    TosaMaxPool2dAttribute attribute(kernel, stride, pad, static_cast<NanPropagationMode>(nan_mode));

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
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_MAX_POOL2D,
                                                                Attribute::Attribute_MaxPool2dAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("max_pool2d", "main", std::move(ops), std::move(tensors), std::move(shapes), {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSAMAX_POOL2D,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}}},
                           {{DataType::int8_t, {1, 1, 1, 1}}},
                           {{{1, 1}, DataType::int32_t, {2}},
                            {{1, 1}, DataType::int32_t, {2}},
                            {{1, 1, 1, 1}, DataType::int32_t, {4}},
                            {{1}, DataType::int32_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Rfft2d)
{
    bool local_bound = false;
    // Create Attribute
    TosaRFFT2dAttribute attribute(local_bound);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string input_realName = "input_real";
    std::string output_realName = "output_real";
    std::string output_imagName = "output_imag";

    std::vector<int32_t> input_realShape = {1, 1, 1, 1};
    std::vector<int32_t> output_realShape = {1, 1, 1, 1};
    std::vector<int32_t> output_imagShape = {1, 1, 1, 1};

    auto input_realTensor = std::make_unique<TosaSerializationTensor>(input_realName,
                                                                      input_realShape,
                                                                      DType::DType_FP32,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(input_realTensor));
    auto output_realTensor = std::make_unique<TosaSerializationTensor>(output_realName,
                                                                       output_realShape,
                                                                       DType::DType_FP32,
                                                                       std::vector<uint8_t>{});
    tensors.push_back(std::move(output_realTensor));
    auto output_imagTensor = std::make_unique<TosaSerializationTensor>(output_imagName,
                                                                       output_imagShape,
                                                                       DType::DType_FP32,
                                                                       std::vector<uint8_t>{});
    tensors.push_back(std::move(output_imagTensor));

    // Create Operator
    auto op =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_RFFT2D,
                                                          Attribute::Attribute_RFFT2dAttribute,
                                                          &attribute,
                                                          std::vector<std::string>{input_realName},
                                                          std::vector<std::string>{output_realName, output_imagName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("rfft2d",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {input_realName},
                                      {output_realName, output_imagName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSARFFT2D,
                           {{std::initializer_list<uint32_t>{}, DataType::float32_t, {1, 1, 1, 1}}},
                           {{DataType::float32_t, {1, 1, 1, 1}}, {DataType::float32_t, {1, 1, 1, 1}}},
                           {{{0}, DataType::bool_t, {1}}});
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, TransposeConv2d)
{
    std::vector<int32_t> out_pad = {1, 1, 1, 1};
    std::vector<int32_t> stride = {1, 1};
    int32_t acc_type = 1;
    bool local_bound = true;
    // Create Attribute
    TosaTransposeConv2dAttribute attribute(out_pad, stride, local_bound, DType_INT32);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::vector<std::unique_ptr<TosaSerializationShape>> shapes;

    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto weightTensor =
        std::make_unique<TosaSerializationTensor>(weightName, weightShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(weightTensor));
    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName, biasShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(biasTensor));
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
    auto weight_zpTensor = std::make_unique<TosaSerializationTensor>(weight_zpName,
                                                                     weight_zpShape,
                                                                     DType::DType_INT8,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(weight_zpTensor));
    auto weight_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{weight_zpName},
                                                                         TosaOpLocation{});
    ops.push_back(std::move(weight_zpOp));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_TRANSPOSE_CONV2D,
        Attribute::Attribute_TransposeConv2dAttribute,
        &attribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("transpose_conv2d",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::move(shapes),
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    const auto& spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSATRANSPOSE_CONV2D,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 1, 1, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int32_t, {1, 1, 1, 1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int32_t, {1, 1, 1, 1}}},
                           {{{1, 1, 1, 1}, DataType::int32_t, {4}},
                            {{1, 1}, DataType::int32_t, {2}},
                            {{1}, DataType::int32_t, {1}},
                            {{1}, DataType::bool_t, {1}}});
}
