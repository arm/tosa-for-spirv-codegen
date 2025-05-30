//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <ModuleComparator.hpp>
#include <OpTestUtils.hpp>
#include <TosaSerializationParser.hpp>

#include <AssemblyUtils.hpp>
#include <spirvmodels/Add.hpp>
#include <spirvmodels/Conv2d.hpp>
#include <spirvmodels/MaxPool.hpp>

#include <tosa_serialization_handler.h>

#include <gtest/gtest.h>

using namespace ::tosa;
using namespace tosa2spirv::parsers;

TEST(TOSA2SPIRV_PARSER, ConstInputOpTest)
{
    // Create Conv2d Attribute
    std::vector<int> pad = {0, 0, 0, 0};
    std::vector<int> stride = {2, 2};
    std::vector<int> dilation = {1, 1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, -128, 0, false);

    // Create Rescale Attribute
    std::vector<int> multiplier = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> shift = {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42};
    TosaRescaleAttribute rescaleAttribute(0, 26, multiplier, shift, true, true, true, false, false);

    // Create Tensor Names
    std::string inputName = "input_1";
    std::string filterName = "constant_4";
    std::string biasName = "constant_3";
    std::string rescaleIntermediateName = "layer_intermediate0_4";
    std::string outputName = "output0_2";

    // Create Tensor Shapes
    std::vector<int32_t> input1Shape = {1, 16, 16, 16};
    std::vector<int32_t> filterShape = {2, 2, 2, 2};
    std::vector<int32_t> biasShape = {16};
    std::vector<int32_t> rescaleShape = {1, 8, 8, 16};
    std::vector<int32_t> output0Shape = {1, 8, 8, 16};

    // Create Tosa Tensors
    auto* inputTensor = new TosaSerializationTensor(inputName, input1Shape, DType::DType_INT8, {});

    // Conv2D - Filter / Weights
    std::vector<unsigned char> filterTensorData{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto* filterTensor = new TosaSerializationTensor(filterName, filterShape, DType::DType_INT8, filterTensorData);

    // Conv2D - Bias
    std::vector<int32_t> biasTensorData{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<uint8_t> convertedBias;
    TosaSerializationHandler::ConvertI32toU8(biasTensorData, convertedBias);
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, convertedBias);

    auto* rescaleTensor = new TosaSerializationTensor(rescaleIntermediateName, rescaleShape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, output0Shape, DType::DType_INT8, {});

    // Create Filter / Weights Const Operator
    auto filterOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {filterName});

    // Create Bias Const Operator
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {biasName});

    // Create Conv2D Operator
    auto conv2DOp = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                        Attribute::Attribute_ConvAttribute,
                                                        &conv2DAttribute,
                                                        {inputName, filterName, biasName},
                                                        {rescaleIntermediateName});

    // Create Rescale Operator
    auto rescaleOp = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                         Attribute::Attribute_RescaleAttribute,
                                                         &rescaleAttribute,
                                                         {rescaleIntermediateName},
                                                         {outputName});

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("main",
                                      "main",
                                      {conv2DOp, rescaleOp, biasOp, filterOp},
                                      {inputTensor, rescaleTensor, outputTensor, biasTensor, filterTensor},
                                      {inputName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");

    EXPECT_TRUE(!binarySpirv.empty()) << "BinarySpirv is empty when it shouldn't be.";
}

TEST(TOSA2SPIRV_PARSER, UnsupportedOperator)
{
    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_UNKNOWN, Attribute::Attribute_NONE, nullptr, {}, {});

    // Create a tosa single-op basic block
    // The raw pointers of operators will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("unknown", "main", {op}, {}, {}, {});

    TosaSerializationParser parser(&block);

    // Pass an unsupport operator to the parser
    // Text of exception is: ERROR: Operator Op_UNKNOWN(0) is currently unsupported
    EXPECT_ANY_THROW(parser.GenerateSPIRV("main"));
}

TEST(TOSA2SPIRV_PARSER, InvalidDtype)
{
    // Create Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> kernel = {2, 2};
    std::vector<int> stride = {2, 2};
    DType dType = DType::DType_UNKNOWN;
    TosaPoolAttribute attribute(pad, kernel, stride, 0, 0, dType);

    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    TosaSerializationTensor* inputTensor = new TosaSerializationTensor(inputName, inputShape, dType, {});
    TosaSerializationTensor* outputTensor = new TosaSerializationTensor(outputName, outputShape, dType, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_MAX_POOL2D,
                                                  Attribute::Attribute_PoolAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("max_pool_2d",
                                      "main",
                                      {op},
                                      {inputTensor, outputTensor},
                                      {inputName},
                                      {outputName});

    TosaSerializationParser parser(&block);

    // Pass an unsupport DType to the parser
    // Text of exception is: ERROR: No Corresponding DataType for DType UNKNOWN (0).
    EXPECT_ANY_THROW(parser.GenerateSPIRV("main"));
}

// MAX POOL
TosaSerializationBasicBlock GeneratorSimpleMaxpool2dModel()
{
    // Create Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> kernel = {2, 2};
    std::vector<int> stride = {2, 2};
    DType dType = DType::DType_INT8;
    TosaPoolAttribute attribute(pad, kernel, stride, 0, 0, dType);

    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, dType, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, dType, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_MAX_POOL2D,
                                                  Attribute::Attribute_PoolAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("max_pool_2d",
                                      "main",
                                      {op},
                                      {inputTensor, outputTensor},
                                      {inputName},
                                      {outputName});

    return block;
}

// Simple Parser API usage test referenced in the README.md
TEST(TOSA2SPIRV_PARSER, Readme)
{
    // Create a simple MaxPool2d model block.
    TosaSerializationBasicBlock block = GeneratorSimpleMaxpool2dModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    TosaSerializationParser parser(&block);

    // Call GenerateSPIRV, which returns a SPIR-V binary vector.
    // This can then be disassembled into human-readable SPIR-V or passed for execution.
    const auto module = parser.GenerateSPIRVModule("main");

    // Ensure output is as expected
    const auto diff = testutils::CompareModules(module, spirvmodels::SimpleMaxpool2d);
    EXPECT_TRUE(diff.empty());
}

// Multi Layer Conv2d reusing the weight and bias GraphConstantId
TosaSerializationBasicBlock GeneratorConv2DRescaleConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> dilation = {1, 1};
    std::vector<int32_t> stride = {2, 2};
    std::vector<int8_t> conv2d_input_zp = {-127};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Rescale Attribute
    std::vector<int8_t> rescale_input_zp = {0};
    std::vector<int8_t> output_zp = {0};
    std::vector<int32_t> multiplier = {8};
    std::vector<int32_t> shift = {8};
    bool scale32 = true;
    bool double_round = false;
    bool per_channel = true;
    bool input_signed = true;
    bool output_signed = true;
    TosaRescaleAttribute rescaleAttribute(rescale_input_zp[0],
                                          output_zp[0],
                                          multiplier,
                                          shift,
                                          scale32,
                                          double_round,
                                          per_channel,
                                          input_signed,
                                          output_signed);

    // Create Tensors
    std::string inputName = "input1";
    std::string weightsName = "weights";
    std::string biasName = "bias";
    std::string rescaleInName = "rescale1";
    std::string rescaleOutName = "rescale2";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> weightsShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> rescaleInShape = {1, 5, 5, 1};
    std::vector<int32_t> rescaleOutShape = {1, 5, 5, 1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightsTensor = new TosaSerializationTensor(weightsName, weightsShape, DType::DType_INT8, {});
    std::vector<int32_t> biasValues{0};
    std::vector<uint8_t> convertedBiasValues;
    TosaSerializationHandler::ConvertI32toU8(biasValues, convertedBiasValues);
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, convertedBiasValues);
    auto* rescaleInTensor = new TosaSerializationTensor(rescaleInName, rescaleInShape, DType::DType_INT32, {});
    auto* rescaleOutTensor = new TosaSerializationTensor(rescaleOutName, rescaleOutShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operators Conv2D, Rescale, Conv2D
    auto weightsOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {weightsName});
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {biasName});
    auto op1 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   {inputName, weightsName, biasName},
                                                   {rescaleInName});
    auto op2 = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                   Attribute::Attribute_RescaleAttribute,
                                                   &rescaleAttribute,
                                                   {rescaleInName},
                                                   {rescaleOutName});
    auto op3 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   {rescaleOutName, weightsName, biasName},
                                                   {outputName});

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block(
        "multi_Layer",
        "main",
        {weightsOp, biasOp, op1, op2, op3},
        {inputTensor, weightsTensor, biasTensor, rescaleInTensor, rescaleOutTensor, outputTensor},
        {inputName},
        {outputName});

    return block;
}

TEST(TOSA2SPIRV_PARSER, Conv2DRescaleConv2D)
{
    // Create a model with 2 Conv2D operators which reuse the same weight and bias GraphConstantId
    TosaSerializationBasicBlock block = GeneratorConv2DRescaleConv2DModel();

    TosaSerializationParser parser(&block);

    const auto module1 = parser.GenerateSPIRVModule("main");
    const auto diff = testutils::CompareModules(module1, spirvmodels::Conv2DRescaleConv2D);
    EXPECT_TRUE(diff.empty());
}

// Invalid block with graph input and output directly connected via IdentityOps
// (input) - IDENTITY - IDENTITY - (output1) - CONV2D - (output2)
TEST(TOSA2SPIRV_PARSER, InvalidBlockInputOutputConnectWithIdentity)
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> dilation = {1, 1};
    std::vector<int32_t> stride = {2, 2};
    std::vector<int8_t> conv2d_input_zp = {1};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Tensors
    std::string inputName = "input1";
    std::string output1Name = "output1";
    std::string output2Name = "output2";
    std::string identityInterTensorName = "output2";
    std::string conv2DWeightsName = "weights";
    std::string conv2DBiasName = "bias";
    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> output1Shape = {1, 7, 7, 1};
    std::vector<int32_t> output2Shape = {1, 3, 3, 1};
    std::vector<int32_t> weightsShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* output1Tensor = new TosaSerializationTensor(output1Name, output1Shape, DType::DType_INT8, {});
    auto* identityInterTensor = new TosaSerializationTensor(output1Name, output1Shape, DType::DType_INT8, {});
    auto* output2Tensor = new TosaSerializationTensor(output2Name, output2Shape, DType::DType_INT8, {});
    auto* conv2DWeightsTensor = new TosaSerializationTensor(conv2DWeightsName, weightsShape, DType::DType_INT8, {});
    std::vector<int32_t> biasValues{0};
    std::vector<uint8_t> convertedBiasValues;
    TosaSerializationHandler::ConvertI32toU8(biasValues, convertedBiasValues);
    auto* conv2DBiasTensor =
        new TosaSerializationTensor(conv2DBiasName, biasShape, DType::DType_INT32, convertedBiasValues);

    // Create Identity Operators
    auto identity1 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         {inputName},
                                                         {identityInterTensorName});
    auto identity2 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         {identityInterTensorName},
                                                         {output1Name});

    // Create Operators Conv2D
    auto weightsOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {conv2DWeightsName});
    auto biasOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {conv2DBiasName});
    auto conv2d_op = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                         Attribute::Attribute_ConvAttribute,
                                                         &conv2DAttribute,
                                                         {output1Name, conv2DWeightsName, conv2DBiasName},
                                                         {output2Name});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block(
        "identity",
        "main",
        {weightsOp, biasOp, identity1, conv2d_op, identity2},
        {inputTensor, output1Tensor, output2Tensor, conv2DWeightsTensor, conv2DBiasTensor, identityInterTensor},
        {inputName},
        {output1Name, output2Name});

    TosaSerializationParser parser(&block);

    // Pass an invalid IdentityOp only block to the parser
    // Text of exception is:
    // TosaSerializationParser: Graph input/output should not be connected directly with Identity Op!
    EXPECT_ANY_THROW(parser.GenerateSPIRV({}));
}

// Multi Layer IDENTITY - CONV2D - IDENTITY
TosaSerializationBasicBlock GeneratorIdentityConv2DIdentityModel()
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> dilation = {1, 1};
    std::vector<int32_t> stride = {2, 2};
    std::vector<int8_t> conv2d_input_zp = {1};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Tensors
    std::string identity1InputName = "identity1_input";
    std::string identity1OutputName = "identity1_output";
    std::string conv2DWeightsName = "weights";
    std::string conv2DBiasName = "bias";
    std::string identity2InputName = "identity2_input";
    std::string identity2OutputName = "identity2_output";

    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> weightsShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};

    auto* identity1InputTensor = new TosaSerializationTensor(identity1InputName, inputShape, DType::DType_INT8, {});
    auto* identity1OutputTensor = new TosaSerializationTensor(identity1OutputName, inputShape, DType::DType_INT8, {});
    auto* conv2DWeightsTensor = new TosaSerializationTensor(conv2DWeightsName, weightsShape, DType::DType_INT8, {});
    std::vector<int32_t> biasValues{0};
    std::vector<uint8_t> convertedBiasValues;
    TosaSerializationHandler::ConvertI32toU8(biasValues, convertedBiasValues);
    auto* conv2DBiasTensor =
        new TosaSerializationTensor(conv2DBiasName, biasShape, DType::DType_INT32, convertedBiasValues);
    auto* identity2InputTensor = new TosaSerializationTensor(identity2InputName, outputShape, DType::DType_INT32, {});
    auto* identity2OutputTensor = new TosaSerializationTensor(identity2OutputName, outputShape, DType::DType_INT32, {});

    // Create Operators Conv2D, Rescale, Conv2D
    auto weightsOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {conv2DWeightsName});
    auto biasOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {conv2DBiasName});
    auto conv2d_op = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                         Attribute::Attribute_ConvAttribute,
                                                         &conv2DAttribute,
                                                         {identity1OutputName, conv2DWeightsName, conv2DBiasName},
                                                         {identity2InputName});
    auto identity1_op = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                            Attribute::Attribute_NONE,
                                                            nullptr,
                                                            {identity1InputName},
                                                            {identity1OutputName});
    auto identity2_op = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                            Attribute::Attribute_NONE,
                                                            nullptr,
                                                            {identity2InputName},
                                                            {identity2OutputName});

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("multi_Layer",
                                      "main",
                                      {weightsOp, biasOp, identity1_op, conv2d_op, identity2_op},
                                      {identity1InputTensor,
                                       identity1OutputTensor,
                                       conv2DWeightsTensor,
                                       conv2DBiasTensor,
                                       identity2InputTensor,
                                       identity2OutputTensor},
                                      {identity1InputName},
                                      {identity2OutputName});

    return block;
}

TEST(TOSA2SPIRV_PARSER, IdentityConv2dIdentity)
{
    // Create a IdentityConv2dIdentity model block.
    TosaSerializationBasicBlock block = GeneratorIdentityConv2DIdentityModel();

    TosaSerializationParser parser(&block);

    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, false));

    testutils::CheckInputTensor({1, 7, 7, 1}, DataType::int8_t, "CONV2D", outputStr);
    testutils::CheckInputTensor({1, 3, 3, 1}, DataType::int8_t, "CONV2D", outputStr);
    testutils::CheckConstCompositeTensor({1, 1, 1, 1}, "CONV2D", outputStr, 0);
    testutils::CheckConstCompositeTensor({2, 2}, "CONV2D", outputStr, 1);
    testutils::CheckConstCompositeTensor({1, 1}, "CONV2D", outputStr, 2);
    testutils::CheckConstant(DataType::int8_t, "CONV2D", outputStr, 1, 3);
    testutils::CheckConstant(DataType::int8_t, "CONV2D", outputStr, 1, 4);
    testutils::CheckBoolConstant(DataType::bool_t, "CONV2D", outputStr, true, 5);
    testutils::CheckOutputTensor({1, 3, 3, 1}, DataType::int32_t, "CONV2D", outputStr);
}

// Multi Layer CONV2D - IDENTITY - RESCALE - CONV2D reusing the weight and bias GraphConstantId
// It should be identical to CONV2D - RESCALE - CONV2D
TosaSerializationBasicBlock GeneratorConv2DIdentityConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> dilation = {1, 1};
    std::vector<int32_t> stride = {2, 2};
    std::vector<int8_t> conv2d_input_zp = {-127};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Rescale Attribute
    std::vector<int8_t> rescale_input_zp = {0};
    std::vector<int8_t> output_zp = {0};
    std::vector<int32_t> multiplier = {8};
    std::vector<int32_t> shift = {8};
    bool scale32 = true;
    bool double_round = false;
    bool per_channel = true;
    bool input_signed = true;
    bool output_signed = true;
    TosaRescaleAttribute rescaleAttribute(rescale_input_zp[0],
                                          output_zp[0],
                                          multiplier,
                                          shift,
                                          scale32,
                                          double_round,
                                          per_channel,
                                          input_signed,
                                          output_signed);

    // Create Tensors
    std::string inputName = "input1";
    std::string weightsName = "weights";
    std::string biasName = "bias";
    std::string identityInName = "identity_input";
    std::string identityOutName = "identity_output";
    std::string rescaleOutName = "rescale_output";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> weightsShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> InterTensorShape = {1, 5, 5, 1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightsTensor = new TosaSerializationTensor(weightsName, weightsShape, DType::DType_INT8, {});
    std::vector<int32_t> biasValues{0};
    std::vector<uint8_t> convertedBiasValues;
    TosaSerializationHandler::ConvertI32toU8(biasValues, convertedBiasValues);
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, convertedBiasValues);
    auto* identityInTensor = new TosaSerializationTensor(identityInName, InterTensorShape, DType::DType_INT32, {});
    auto* identityOutTensor = new TosaSerializationTensor(identityOutName, InterTensorShape, DType::DType_INT32, {});
    auto* rescaleOutTensor = new TosaSerializationTensor(rescaleOutName, InterTensorShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operators Conv2D, Rescale, Conv2D
    auto weightsOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {weightsName});
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {biasName});
    auto op1 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   {inputName, weightsName, biasName},
                                                   {identityInName});
    auto op2 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                   Attribute::Attribute_NONE,
                                                   nullptr,
                                                   {identityInName},
                                                   {identityOutName});
    auto op3 = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                   Attribute::Attribute_RescaleAttribute,
                                                   &rescaleAttribute,
                                                   {identityOutName},
                                                   {rescaleOutName});
    auto op4 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   {rescaleOutName, weightsName, biasName},
                                                   {outputName});

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block(
        "multi_layer",
        "main",
        {weightsOp, biasOp, op1, op2, op3, op4},
        {inputTensor, weightsTensor, biasTensor, identityInTensor, identityOutTensor, rescaleOutTensor, outputTensor},
        {inputName},
        {outputName});

    return block;
}

TEST(TOSA2SPIRV_PARSER, Conv2dIdentityConv2d)
{
    // Create a IdentityConv2dIdentity model block.
    TosaSerializationBasicBlock block = GeneratorConv2DIdentityConv2DModel();

    TosaSerializationParser parser(&block);

    auto module1 = parser.GenerateSPIRVModule("main");
    const auto diff = testutils::CompareModules(module1, spirvmodels::Conv2DRescaleConv2D);
    EXPECT_TRUE(diff.empty());
}

// Multi Layer CONV2D - IDENTITY - (Graph Output) - IDENTITY - RESCALE - CONV2D
// reusing the weight and bias GraphConstantId
// It should be identical to CONV2D - RESCALE - CONV2D with an extra graph output from the first CONV2D
TosaSerializationBasicBlock GeneratorConv2DIdentityOutputConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> dilation = {1, 1};
    std::vector<int32_t> stride = {2, 2};
    std::vector<int8_t> conv2d_input_zp = {-127};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Rescale Attribute
    std::vector<int8_t> rescale_input_zp = {0};
    std::vector<int8_t> output_zp = {0};
    std::vector<int32_t> multiplier = {8};
    std::vector<int32_t> shift = {8};
    bool scale32 = true;
    bool double_round = false;
    bool per_channel = true;
    bool input_signed = true;
    bool output_signed = true;
    TosaRescaleAttribute rescaleAttribute(rescale_input_zp[0],
                                          output_zp[0],
                                          multiplier,
                                          shift,
                                          scale32,
                                          double_round,
                                          per_channel,
                                          input_signed,
                                          output_signed);

    // Create Tensors
    std::string inputName = "input1";
    std::string weightsName = "weights";
    std::string biasName = "bias";
    std::string identity1InName = "identity1_input";
    std::string identity1OutName = "identity1_output";
    std::string identity2OutName = "identity2_output";
    std::string rescaleOutName = "rescale_output";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> weightsShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> InterTensorShape = {1, 5, 5, 1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightsTensor = new TosaSerializationTensor(weightsName, weightsShape, DType::DType_INT8, {});
    std::vector<int32_t> biasValues{0};
    std::vector<uint8_t> convertedBiasValues;
    TosaSerializationHandler::ConvertI32toU8(biasValues, convertedBiasValues);
    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, convertedBiasValues);
    auto* identity1InTensor = new TosaSerializationTensor(identity1InName, InterTensorShape, DType::DType_INT32, {});
    auto* identity1OutTensor = new TosaSerializationTensor(identity1OutName, InterTensorShape, DType::DType_INT32, {});
    auto* identity2OutTensor = new TosaSerializationTensor(identity2OutName, InterTensorShape, DType::DType_INT32, {});
    auto* rescaleOutTensor = new TosaSerializationTensor(rescaleOutName, InterTensorShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operators Conv2D, Rescale, Conv2D
    auto weightsOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {weightsName});
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {biasName});
    auto op1 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   {inputName, weightsName, biasName},
                                                   {identity1InName});
    auto op2 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                   Attribute::Attribute_NONE,
                                                   nullptr,
                                                   {identity1InName},
                                                   {identity1OutName});
    auto op3 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                   Attribute::Attribute_NONE,
                                                   nullptr,
                                                   {identity1OutName},
                                                   {identity2OutName});
    auto op4 = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                   Attribute::Attribute_RescaleAttribute,
                                                   &rescaleAttribute,
                                                   {identity2OutName},
                                                   {rescaleOutName});
    auto op5 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   {rescaleOutName, weightsName, biasName},
                                                   {outputName});

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("multi_layer",
                                      "main",
                                      {weightsOp, biasOp, op1, op2, op3, op4, op5},
                                      {inputTensor,
                                       weightsTensor,
                                       biasTensor,
                                       identity1InTensor,
                                       identity1OutTensor,
                                       identity2OutTensor,
                                       rescaleOutTensor,
                                       outputTensor},
                                      {inputName},
                                      {outputName, identity1OutName});

    return block;
}

TEST(TOSA2SPIRV_PARSER, Conv2dIdentityConv2dDualOutput)
{
    // Create a IdentityConv2dIdentity model block.
    TosaSerializationBasicBlock block = GeneratorConv2DIdentityOutputConv2DModel();

    TosaSerializationParser parser(&block);

    const auto module1 = parser.GenerateSPIRVModule("main");
    const auto diff = testutils::CompareModules(module1, spirvmodels::Conv2DRescaleConv2DDualOutput);
    const auto binary = tosa2spirv::WriteToBinary(module1);
    std::string dis = testutils::DisassembleSPIRV(binary, true);
    EXPECT_TRUE(diff.empty());
}

// Struct containing golden SPIRV of Add operator with one input having an empty shape.

// Test case to test that we correctly transform empty tensor shape to shape of { 1 } otherwise this would fail.
TEST(TOSA2SPIRV_PARSER, EmptyTensor)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create a Add model block.
    auto addOp = new tosa::TosaSerializationOperator(Op::Op_ADD,
                                                     Attribute::Attribute_NONE,
                                                     nullptr,
                                                     {input1Name, input2Name},
                                                     {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("add",
                                      "main",
                                      {addOp},
                                      {input1Tensor, input2Tensor, outputTensor},
                                      {input1Name, input2Name},
                                      {outputName});

    TosaSerializationParser parser(&block);

    auto module1 = parser.GenerateSPIRVModule("main");
    const auto diff = testutils::CompareModules(module1, spirvmodels::AddZeroDimOutputTensor);
    EXPECT_TRUE(diff.empty());
}
