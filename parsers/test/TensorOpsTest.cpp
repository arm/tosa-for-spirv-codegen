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
TEST(TOSA2SPIRV_PARSER, Argmax)
{
    int32_t axis = 1;
    // Create Attribute
    TosaAxisAttribute attribute(axis);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_ARGMAX,
                                                  Attribute::Attribute_AxisAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("argmax", "main", {op}, {inputTensor, outputTensor}, {inputName}, {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "ARGMAX", outputStr);
    testutils::CheckConstant(DataType::int32_t, "ARGMAX", outputStr, 1, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "ARGMAX", outputStr);
}

TEST(TOSA2SPIRV_PARSER, AvgPool2d)
{
    std::vector<int32_t> kernel = {1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> pad = {1, 1, 1, 1};
    int32_t input_zp = 1;
    int32_t output_zp = 1;
    // Create Attribute
    TosaPoolAttribute attribute(pad, kernel, stride, input_zp, output_zp, DType::DType_FP16);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_AVG_POOL2D,
                                                  Attribute::Attribute_PoolAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("avg_pool2d",
                                      "main",
                                      {op},
                                      {inputTensor, outputTensor},
                                      {inputName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "AVG_POOL2D", outputStr);
    testutils::CheckConstCompositeTensor({1, 1}, "AVG_POOL2D", outputStr, 0);
    testutils::CheckConstCompositeTensor({1, 1}, "AVG_POOL2D", outputStr, 1);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "AVG_POOL2D", outputStr, 2);
    testutils::CheckConstant(DataType::int32_t, "AVG_POOL2D", outputStr, 1, 3);
    testutils::CheckConstant(DataType::int8_t, "AVG_POOL2D", outputStr, 1, 4);
    testutils::CheckConstant(DataType::int8_t, "AVG_POOL2D", outputStr, 1, 5);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "AVG_POOL2D", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Conv2d)
{
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> dilation = {1, 1};
    int8_t input_zp = 1;
    int8_t weight_zp = 1;
    bool local_bound = true;
    // Create Attribute
    TosaConvAttribute attribute(pad, stride, dilation, input_zp, weight_zp, local_bound);

    // Create Tensors
    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightTensor = new TosaSerializationTensor(weightName, weightShape, DType::DType_INT8, {});
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                  Attribute::Attribute_ConvAttribute,
                                                  &attribute,
                                                  {inputName, weightName, biasName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("conv2d",
                                      "main",
                                      {op},
                                      {inputTensor, weightTensor, biasTensor, outputTensor},
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "CONV2D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "CONV2D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "CONV2D", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "CONV2D", outputStr, 0);
    testutils::CheckConstCompositeTensor({1, 1}, "CONV2D", outputStr, 1);
    testutils::CheckConstCompositeTensor({1, 1}, "CONV2D", outputStr, 2);
    testutils::CheckConstant(DataType::int8_t, "CONV2D", outputStr, 1, 3);
    testutils::CheckConstant(DataType::int8_t, "CONV2D", outputStr, 1, 4);
    testutils::CheckBoolConstant(DataType::bool_t, "CONV2D", outputStr, true, 5);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "CONV2D", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Conv3d)
{
    std::vector<int32_t> pad = {1, 1, 1, 1, 1, 1};
    std::vector<int32_t> stride = {1, 1, 1};
    std::vector<int32_t> dilation = {1, 1, 1};
    int8_t input_zp = 1;
    int8_t weight_zp = 1;
    bool local_bound = true;
    // Create Attribute
    TosaConvAttribute attribute(pad, stride, dilation, input_zp, weight_zp, local_bound);

    // Create Tensors
    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightTensor = new TosaSerializationTensor(weightName, weightShape, DType::DType_INT8, {});
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_CONV3D,
                                                  Attribute::Attribute_ConvAttribute,
                                                  &attribute,
                                                  {inputName, weightName, biasName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("conv3d",
                                      "main",
                                      {op},
                                      {inputTensor, weightTensor, biasTensor, outputTensor},
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "CONV3D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "CONV3D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "CONV3D", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1, 1, 1}, "CONV3D", outputStr, 0);
    testutils::CheckConstCompositeTensor({1, 1, 1}, "CONV3D", outputStr, 1);
    testutils::CheckConstCompositeTensor({1, 1, 1}, "CONV3D", outputStr, 2);
    testutils::CheckConstant(DataType::int8_t, "CONV3D", outputStr, 1, 3);
    testutils::CheckConstant(DataType::int8_t, "CONV3D", outputStr, 1, 4);
    testutils::CheckBoolConstant(DataType::bool_t, "CONV3D", outputStr, true, 5);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "CONV3D", outputStr);
}

TEST(TOSA2SPIRV_PARSER, DepthwiseConv2d)
{
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> dilation = {1, 1};
    int8_t input_zp = 1;
    int8_t weight_zp = 1;
    bool local_bound = true;
    // Create Attribute
    TosaConvAttribute attribute(pad, stride, dilation, input_zp, weight_zp, local_bound);

    // Create Tensors
    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightTensor = new TosaSerializationTensor(weightName, weightShape, DType::DType_INT8, {});
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_DEPTHWISE_CONV2D,
                                                  Attribute::Attribute_ConvAttribute,
                                                  &attribute,
                                                  {inputName, weightName, biasName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("depthwise_conv2d",
                                      "main",
                                      {op},
                                      {inputTensor, weightTensor, biasTensor, outputTensor},
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "DEPTHWISE_CONV2D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "DEPTHWISE_CONV2D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "DEPTHWISE_CONV2D", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "DEPTHWISE_CONV2D", outputStr, 0);
    testutils::CheckConstCompositeTensor({1, 1}, "DEPTHWISE_CONV2D", outputStr, 1);
    testutils::CheckConstCompositeTensor({1, 1}, "DEPTHWISE_CONV2D", outputStr, 2);
    testutils::CheckConstant(DataType::int8_t, "DEPTHWISE_CONV2D", outputStr, 1, 3);
    testutils::CheckConstant(DataType::int8_t, "DEPTHWISE_CONV2D", outputStr, 1, 4);
    testutils::CheckBoolConstant(DataType::bool_t, "DEPTHWISE_CONV2D", outputStr, true, 5);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "DEPTHWISE_CONV2D", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Fft2d)
{
    bool inverse = true;
    bool local_bound = true;
    // Create Attribute
    TosaFFTAttribute attribute(inverse, local_bound);

    // Create Tensors
    std::string input_realName = "input_real";
    std::string input_imagName = "input_imag";
    std::string output_realName = "output_real";
    std::string output_imagName = "output_imag";

    std::vector<int32_t> input_realShape = {1, 1, 1, 1};
    std::vector<int32_t> input_imagShape = {1, 1, 1, 1};
    std::vector<int32_t> output_realShape = {1, 1, 1, 1};
    std::vector<int32_t> output_imagShape = {1, 1, 1, 1};

    auto* input_realTensor = new TosaSerializationTensor(input_realName, input_realShape, DType::DType_FP32, {});
    auto* input_imagTensor = new TosaSerializationTensor(input_imagName, input_imagShape, DType::DType_FP32, {});
    auto* output_realTensor = new TosaSerializationTensor(output_realName, output_realShape, DType::DType_FP32, {});
    auto* output_imagTensor = new TosaSerializationTensor(output_imagName, output_imagShape, DType::DType_FP32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_FFT2D,
                                                  Attribute::Attribute_FFTAttribute,
                                                  &attribute,
                                                  {input_realName, input_imagName},
                                                  {output_realName, output_imagName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("fft2d",
                                      "main",
                                      {op},
                                      {input_realTensor, input_imagTensor, output_realTensor, output_imagTensor},
                                      {input_realName, input_imagName},
                                      {output_realName, output_imagName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::float32_t, "FFT2D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::float32_t, "FFT2D", outputStr);
    testutils::CheckBoolConstant(DataType::bool_t, "FFT2D", outputStr, true, 0);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::float32_t, "FFT2D", outputStr);
    testutils::CheckBoolConstant(DataType::bool_t, "FFT2D", outputStr, true, 1);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::float32_t, "FFT2D", outputStr);
}

TEST(TOSA2SPIRV_PARSER, FullyConnected)
{
    int8_t input_zp = 1;
    int8_t weight_zp = 1;
    // Create Attribute
    TosaFullyConnectedAttribute attribute(input_zp, weight_zp);

    // Create Tensors
    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightTensor = new TosaSerializationTensor(weightName, weightShape, DType::DType_INT8, {});
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_FULLY_CONNECTED,
                                                  Attribute::Attribute_FullyConnectedAttribute,
                                                  &attribute,
                                                  {inputName, weightName, biasName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("fully_connected",
                                      "main",
                                      {op},
                                      {inputTensor, weightTensor, biasTensor, outputTensor},
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "FULLY_CONNECTED", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "FULLY_CONNECTED", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "FULLY_CONNECTED", outputStr);
    testutils::CheckConstant(DataType::int8_t, "FULLY_CONNECTED", outputStr, 1, 0);
    testutils::CheckConstant(DataType::int8_t, "FULLY_CONNECTED", outputStr, 1, 1);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "FULLY_CONNECTED", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Matmul)
{
    int8_t A_zp = 1;
    int8_t B_zp = 1;
    // Create Attribute
    TosaMatMulAttribute attribute(A_zp, B_zp);

    // Create Tensors
    std::string AName = "A";
    std::string BName = "B";
    std::string outputName = "output";

    std::vector<int32_t> AShape = {1, 1, 1, 1};
    std::vector<int32_t> BShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* ATensor = new TosaSerializationTensor(AName, AShape, DType::DType_INT8, {});
    auto* BTensor = new TosaSerializationTensor(BName, BShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_MATMUL,
                                                  Attribute::Attribute_MatMulAttribute,
                                                  &attribute,
                                                  {AName, BName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("matmul",
                                      "main",
                                      {op},
                                      {ATensor, BTensor, outputTensor},
                                      {AName, BName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "MATMUL", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "MATMUL", outputStr);
    testutils::CheckConstant(DataType::int8_t, "MATMUL", outputStr, 1, 0);
    testutils::CheckConstant(DataType::int8_t, "MATMUL", outputStr, 1, 1);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "MATMUL", outputStr);
}

TEST(TOSA2SPIRV_PARSER, MaxPool2d)
{
    std::vector<int32_t> kernel = {1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> pad = {1, 1, 1, 1};
    // Create Attribute
    TosaPoolAttribute attribute(pad, kernel, stride, 0, 0, DType::DType_INT32);

    // Create Tensors
    std::string inputName = "input";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_MAX_POOL2D,
                                                  Attribute::Attribute_PoolAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("max_pool2d",
                                      "main",
                                      {op},
                                      {inputTensor, outputTensor},
                                      {inputName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "MAX_POOL2D", outputStr);
    testutils::CheckConstCompositeTensor({1, 1}, "MAX_POOL2D", outputStr, 0);
    testutils::CheckConstCompositeTensor({1, 1}, "MAX_POOL2D", outputStr, 1);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "MAX_POOL2D", outputStr, 2);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int8_t, "MAX_POOL2D", outputStr);
}

TEST(TOSA2SPIRV_PARSER, Rfft2d)
{
    // Create Tensors
    std::string inputName = "input";
    std::string output_realName = "output_real";
    std::string output_imagName = "output_imag";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> output_realShape = {1, 1, 1, 1};
    std::vector<int32_t> output_imagShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_FP32, {});
    auto* output_realTensor = new TosaSerializationTensor(output_realName, output_realShape, DType::DType_FP32, {});
    auto* output_imagTensor = new TosaSerializationTensor(output_imagName, output_imagShape, DType::DType_FP32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_RFFT2D,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  {inputName},
                                                  {output_realName, output_imagName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("rfft2d",
                                      "main",
                                      {op},
                                      {inputTensor, output_realTensor, output_imagTensor},
                                      {inputName},
                                      {output_realName, output_imagName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::float32_t, "RFFT2D", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::float32_t, "RFFT2D", outputStr);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::float32_t, "RFFT2D", outputStr);
}

TEST(TOSA2SPIRV_PARSER, TransposeConv2d)
{
    std::vector<int32_t> out_pad = {1, 1, 1, 1};
    std::vector<int32_t> stride = {1, 1};
    std::vector<int32_t> out_shape = {1, 1, 1, 1};
    int8_t input_zp = 1;
    int8_t weight_zp = 1;
    bool local_bound = true;
    // Create Attribute
    TosaTransposeConvAttribute attribute(out_pad, stride, out_shape, input_zp, weight_zp, local_bound);

    // Create Tensors
    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string outputName = "output";

    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightTensor = new TosaSerializationTensor(weightName, weightShape, DType::DType_INT8, {});
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_TRANSPOSE_CONV2D,
                                                  Attribute::Attribute_TransposeConvAttribute,
                                                  &attribute,
                                                  {inputName, weightName, biasName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("transpose_conv2d",
                                      "main",
                                      {op},
                                      {inputTensor, weightTensor, biasTensor, outputTensor},
                                      {inputName, weightName, biasName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "TRANSPOSE_CONV2D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int8_t, "TRANSPOSE_CONV2D", outputStr);
    testutils::CheckInputTensor({1, 1, 1, 1}, DataType::int32_t, "TRANSPOSE_CONV2D", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "TRANSPOSE_CONV2D", outputStr, 0);
    testutils::CheckConstCompositeTensor({1, 1}, "TRANSPOSE_CONV2D", outputStr, 1);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "TRANSPOSE_CONV2D", outputStr, 2);
    testutils::CheckConstant(DataType::int8_t, "TRANSPOSE_CONV2D", outputStr, 1, 3);
    testutils::CheckConstant(DataType::int8_t, "TRANSPOSE_CONV2D", outputStr, 1, 4);
    testutils::CheckBoolConstant(DataType::bool_t, "TRANSPOSE_CONV2D", outputStr, true, 5);
    testutils::CheckOutputTensor({1, 1, 1, 1}, DataType::int32_t, "TRANSPOSE_CONV2D", outputStr);
}
