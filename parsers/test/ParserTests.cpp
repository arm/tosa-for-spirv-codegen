//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <TosaSerializationParser.hpp>
#include <TestUtils.hpp>
#include <OpTestUtils.hpp>

#include <gtest/gtest.h>

using namespace tosa;
using namespace tosa2spirv;
using namespace parsers;

TEST(TOSA2SPIRV_PARSER, ConstInputOpTest)
{
    // Create Conv2d Attribute
    std::vector<int> pad      = {0, 0, 0, 0};
    std::vector<int> stride   = {2, 2};
    std::vector<int> dilation = {1, 1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, -128, 0, false);

    // Create Rescale Attribute
    std::vector<int> multiplier = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> shift = {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42};
    TosaRescaleAttribute rescaleAttribute(0, 26, multiplier, shift, true, true, true, false, false);

    // Create Tensor Names
    std::string inputName               = "input_1";
    std::string filterName              = "constant_4";
    std::string biasName                = "constant_3";
    std::string rescaleIntermediateName = "layer_intermediate0_4";
    std::string outputName              = "output0_2";

    // Create Tensor Shapes
    std::vector<int32_t> input1Shape  = {1, 16, 16, 16};
    std::vector<int32_t> filterShape  = {2, 2, 2, 2};
    std::vector<int32_t> biasShape    = {16};
    std::vector<int32_t> rescaleShape = {1, 8, 8, 16};
    std::vector<int32_t> output0Shape = {1, 8, 8, 16};

    // Create Tosa Tensors
    auto* inputTensor    = new TosaSerializationTensor(inputName, input1Shape, DType::DType_INT8, {});

    // Conv2D - Filter / Weights
    std::vector<unsigned char> filterTensorData {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto* filterTensor   = new TosaSerializationTensor(filterName, filterShape, DType::DType_INT8, filterTensorData);

    // Conv2D - Bias
    std::vector<unsigned char> biasTensorData {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    auto* biasTensor     = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, biasTensorData);

    auto* rescaleTensor  = new TosaSerializationTensor(rescaleIntermediateName, rescaleShape, DType::DType_INT32, {});
    auto* outputTensor   = new TosaSerializationTensor(outputName, output0Shape, DType::DType_INT8, {});

    // Create Filter / Weights Const Operator
    auto filterOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                        Attribute::Attribute_NONE,
                                                        nullptr,
                                                        {},
                                                        { filterName });

    // Create Bias Const Operator
    auto biasOp   = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                        Attribute::Attribute_NONE,
                                                        nullptr,
                                                        {},
                                                        { biasName });

    // Create Conv2D Operator
    auto conv2DOp = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                        Attribute::Attribute_ConvAttribute,
                                                        &conv2DAttribute,
                                                        {inputName, filterName, biasName },
                                                        {rescaleIntermediateName });

    // Create Rescale Operator
    auto rescaleOp = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                         Attribute::Attribute_RescaleAttribute,
                                                         &rescaleAttribute,
                                                         {rescaleIntermediateName },
                                                         {outputName });

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("main",
                                      "main",
                                      { conv2DOp, rescaleOp, biasOp, filterOp },
                                      { inputTensor, rescaleTensor, outputTensor, biasTensor, filterTensor },
                                      { inputName },
                                      { outputName });

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
    auto op = new tosa::TosaSerializationOperator(Op::Op_UNKNOWN,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  {},
                                                  {});

    // Create a tosa single-op basic block
    // The raw pointers of operators will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("unknown",
                                      "main",
                                      { op },
                                      {},
                                      {},
                                      {});

    TosaSerializationParser parser(&block);

    // Pass an unsupport operator to the parser
    // Text of exception is: ERROR: Operator Op_UNKNOWN(0) is currently unsupported
    EXPECT_ANY_THROW(parser.GenerateSPIRV());
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
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("max_pool_2d",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);

    // Pass an unsupport DType to the parser
    // Text of exception is: ERROR: No Corresponding DataType for DType UNKNOWN (0).
    EXPECT_ANY_THROW(parser.GenerateSPIRV());
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
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("max_pool_2d",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    return block;
}

struct SimpleMaxpool2dExpected
{
    std::string goldenSPIRV = GetPreSPIRVStr() +
R"(          %3 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %23 DescriptorSet 0
               OpDecorate %23 Binding 0
               OpDecorate %27 DescriptorSet 0
               OpDecorate %27 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_3 = OpConstant %uint 3
         %10 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_3 %uint_3
         %11 = OpTypeTensorARM %uchar %uint_4 %10
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
     %uint_2 = OpConstant %uint 2
         %14 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %15 = OpTypeTensorARM %uint %uint_1 %14
         %16 = OpConstantCompositeReplicateEXT %15 %uint_2
         %17 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %18 = OpTypeTensorARM %uint %uint_1 %17
         %19 = OpConstantCompositeReplicateEXT %18 %uint_1
         %20 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_4 %uint_4
         %21 = OpTypeTensorARM %uchar %uint_4 %20
%_ptr_UniformConstant_21 = OpTypePointer UniformConstant %21
     %uint_0 = OpConstant %uint 0
%_ptr_UniformConstant_11 = OpTypePointer UniformConstant %11
         %23 = OpVariable %_ptr_UniformConstant_21 UniformConstant
         %27 = OpVariable %_ptr_UniformConstant_11 UniformConstant
         %28 = OpTypeGraphARM 1 %21 %11
               OpGraphEntryPointARM %1 "Graph_1" %23 %27
          %1 = OpGraphARM %28
         %25 = OpGraphInputARM %21 %uint_0
          %2 = OpExtInst %11 %3 MAX_POOL2D %16 %16 %19 %25
               OpGraphSetOutputARM %2 %uint_0
               OpGraphEndARM
)";
};

// Simple Parser API usage test referenced in the README.md
TEST(TOSA2SPIRV_PARSER, Readme)
{
    // Create a simple MaxPool2d model block.
    TosaSerializationBasicBlock block = GeneratorSimpleMaxpool2dModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    TosaSerializationParser parser(&block);

    // Call GenerateSPIRV, which returns a SPIR-V binary vector.
    // This can then be disassembled into human-readable SPIR-V or passed for execution.
    auto binarySpirv = parser.GenerateSPIRV();
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    SimpleMaxpool2dExpected expected;
    // Ensure output is as expected
    EXPECT_EQ(expected.goldenSPIRV, outputStr);
}

// Multi Layer Conv2d reusing the weight and bias GraphConstantId
TosaSerializationBasicBlock GeneratorConv2DRescaleConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad      = { 1, 1, 1, 1 };
    std::vector<int32_t> dilation = { 1, 1 };
    std::vector<int32_t> stride   = { 2, 2 };
    std::vector<int8_t> conv2d_input_zp = {-127};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Rescale Attribute
    std::vector<int8_t> rescale_input_zp    = {0};
    std::vector<int8_t> output_zp   = {0};
    std::vector<int32_t> multiplier = {8};
    std::vector<int32_t> shift      = {8};
    bool scale32       = true;
    bool double_round  = false;
    bool per_channel   = true;
    bool input_signed  = true;
    bool output_signed = true;
    TosaRescaleAttribute rescaleAttribute(
        rescale_input_zp[0], output_zp[0], multiplier, shift, scale32,
        double_round, per_channel, input_signed, output_signed);

    // Create Tensors
    std::string inputName      = "input1";
    std::string weightsName    = "weights";
    std::string biasName       = "bias";
    std::string rescaleInName  = "rescale1";
    std::string rescaleOutName = "rescale2";
    std::string outputName     = "output1";
    std::vector<int32_t> inputShape      = { 1, 7, 7, 1 };
    std::vector<int32_t> weightsShape    = { 1, 3, 3, 1 };
    std::vector<int32_t> biasShape       = { 1 };
    std::vector<int32_t> rescaleInShape  = { 1, 5, 5, 1 };
    std::vector<int32_t> rescaleOutShape = { 1, 5, 5, 1 };
    std::vector<int32_t> outputShape     = { 1, 3, 3, 1 };

    auto* inputTensor      = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightsTensor    = new TosaSerializationTensor(weightsName, weightsShape, DType::DType_INT8, {});
    auto* biasTensor       = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {});
    auto* rescaleInTensor  = new TosaSerializationTensor(rescaleInName, rescaleInShape, DType::DType_INT32, {});
    auto* rescaleOutTensor = new TosaSerializationTensor(rescaleOutName, rescaleOutShape, DType::DType_INT8, {});
    auto* outputTensor     = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operators Conv2D, Rescale, Conv2D
    auto weightsOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         {},
                                                         { weightsName });
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                      Attribute::Attribute_NONE,
                                                      nullptr,
                                                      {},
                                                      { biasName });
    auto op1 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   { inputName, weightsName, biasName },
                                                   { rescaleInName });
    auto op2 = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                   Attribute::Attribute_RescaleAttribute,
                                                   &rescaleAttribute,
                                                   { rescaleInName },
                                                   { rescaleOutName });
    auto op3 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   { rescaleOutName, weightsName, biasName },
                                                   { outputName });

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("multi_Layer",
                                      "main",
                                      { weightsOp, biasOp, op1, op2, op3 },
                                      { inputTensor, weightsTensor, biasTensor,
                                        rescaleInTensor, rescaleOutTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    return block;
}

struct Conv2DRescaleConv2DExpected
{
    std::string goldenSPIRV = GetPreSPIRVStr() +
        R"(         %11 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %43 DescriptorSet 0
               OpDecorate %43 Binding 0
               OpDecorate %51 DescriptorSet 0
               OpDecorate %51 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_3 = OpConstant %uint 3
         %18 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_3 %uint_3 %uint_1
         %19 = OpTypeTensorARM %uchar %uint_4 %18
          %2 = OpGraphConstantARM %19 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %21 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %22 = OpTypeTensorARM %uint %uint_1 %21
          %3 = OpGraphConstantARM %22 1
          %5 = OpGraphConstantARM %22 2
         %23 = OpTypeTensorARM %uchar %uint_1 %21
          %6 = OpGraphConstantARM %23 3
          %8 = OpGraphConstantARM %19 4
          %9 = OpGraphConstantARM %22 5
     %uint_5 = OpConstant %uint 5
         %25 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_5 %uint_5 %uint_1
         %26 = OpTypeTensorARM %uint %uint_4 %25
         %27 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %28 = OpTypeTensorARM %uint %uint_1 %27
         %29 = OpConstantCompositeReplicateEXT %28 %uint_1
     %uint_2 = OpConstant %uint 2
         %31 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %32 = OpTypeTensorARM %uint %uint_1 %31
         %33 = OpConstantCompositeReplicateEXT %32 %uint_2
         %34 = OpConstantCompositeReplicateEXT %32 %uint_1
  %uchar_129 = OpConstant %uchar 129
    %uchar_1 = OpConstant %uchar 1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
     %uint_7 = OpConstant %uint 7
         %40 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_7 %uint_7 %uint_1
         %41 = OpTypeTensorARM %uchar %uint_4 %40
%_ptr_UniformConstant_41 = OpTypePointer UniformConstant %41
     %uint_0 = OpConstant %uint 0
         %43 = OpVariable %_ptr_UniformConstant_41 UniformConstant
         %46 = OpTypeTensorARM %uchar %uint_4 %25
    %uchar_0 = OpConstant %uchar 0
      %false = OpConstantFalse %bool
         %49 = OpTypeTensorARM %uint %uint_4 %18
%_ptr_UniformConstant_49 = OpTypePointer UniformConstant %49
         %51 = OpVariable %_ptr_UniformConstant_49 UniformConstant
         %52 = OpTypeGraphARM 1 %41 %49
               OpGraphEntryPointARM %1 "main" %43 %51
          %1 = OpGraphARM %52
         %45 = OpGraphInputARM %41 %uint_0
          %4 = OpExtInst %26 %11 CONV2D %29 %33 %34 %uchar_129 %uchar_1 %true %45 %2 %3
          %7 = OpExtInst %46 %11 RESCALE %uint_0 %uchar_0 %5 %6 %true %false %true %true %true %4
         %10 = OpExtInst %49 %11 CONV2D %29 %33 %34 %uchar_129 %uchar_1 %true %7 %8 %9
               OpGraphSetOutputARM %10 %uint_0
               OpGraphEndARM
)";
};

TEST(TOSA2SPIRV_PARSER, Conv2DRescaleConv2D)
{
    // Create a model with 2 Conv2D operators which reuse the same weight and bias GraphConstantId
    TosaSerializationBasicBlock block = GeneratorConv2DRescaleConv2DModel();

    TosaSerializationParser parser(&block);

    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    Conv2DRescaleConv2DExpected expected;

    EXPECT_EQ(expected.goldenSPIRV, outputStr);
}

// Invalid block with only IdentityOp
TEST(TOSA2SPIRV_PARSER, InvalidBlockOnlyIdentity)
{
    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                  Attribute::Attribute_NONE,
                                                  nullptr,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("identity",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    TosaSerializationParser parser(&block);

    // Pass an invalid IdentityOp only block to the parser
    // Text of exception is: TosaSerializationParser: Block contains only Identity operator!
    EXPECT_ANY_THROW(parser.GenerateSPIRV({}));
}

// Invalid block with graph input and output directly connected via IdentityOps
// (input) - IDENTITY - IDENTITY - (output1) - CONV2D - (output2)
TEST(TOSA2SPIRV_PARSER, InvalidBlockInputOutputConnectWithIdentity)
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad            = { 1, 1, 1, 1 };
    std::vector<int32_t> dilation       = { 1, 1 };
    std::vector<int32_t> stride         = { 2, 2 };
    std::vector<int8_t> conv2d_input_zp = {1};
    std::vector<int8_t> weight_zp       = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Tensors
    std::string inputName               = "input1";
    std::string output1Name             = "output1";
    std::string output2Name             = "output2";
    std::string identityInterTensorName = "output2";
    std::string conv2DWeightsName       = "weights";
    std::string conv2DBiasName          = "bias";
    std::vector<int32_t> inputShape     = {1, 7, 7, 1};
    std::vector<int32_t> output1Shape   = {1, 7, 7, 1};
    std::vector<int32_t> output2Shape   = {1, 3, 3, 1};
    std::vector<int32_t> weightsShape   = { 1, 3, 3, 1 };
    std::vector<int32_t> biasShape      = { 1 };

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* output1Tensor = new TosaSerializationTensor(output1Name, output1Shape, DType::DType_INT8, {});
    auto* identityInterTensor = new TosaSerializationTensor(output1Name, output1Shape, DType::DType_INT8, {});
    auto* output2Tensor = new TosaSerializationTensor(output2Name, output2Shape, DType::DType_INT8, {});
    auto* conv2DWeightsTensor = new TosaSerializationTensor(conv2DWeightsName, weightsShape, DType::DType_INT8, {});
    auto* conv2DBiasTensor = new TosaSerializationTensor(conv2DBiasName, biasShape, DType::DType_INT32, {});

    // Create Identity Operators
    auto identity1 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         { inputName },
                                                         { identityInterTensorName });
    auto identity2 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         { identityInterTensorName },
                                                         { output1Name });

    // Create Operators Conv2D
    auto weightsOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         {},
                                                         { conv2DWeightsName });
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                      Attribute::Attribute_NONE,
                                                      nullptr,
                                                      {},
                                                      { conv2DBiasName });
    auto conv2d_op = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                         Attribute::Attribute_ConvAttribute,
                                                         &conv2DAttribute,
                                                         { output1Name, conv2DWeightsName, conv2DBiasName },
                                                         { output2Name });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("identity",
                                      "main",
                                      { weightsOp, biasOp, identity1, conv2d_op, identity2 },
                                      { inputTensor, output1Tensor, output2Tensor,
                                        conv2DWeightsTensor, conv2DBiasTensor, identityInterTensor },
                                      { inputName },
                                      { output1Name, output2Name });

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
    std::vector<int32_t> pad      = { 1, 1, 1, 1 };
    std::vector<int32_t> dilation = { 1, 1 };
    std::vector<int32_t> stride   = { 2, 2 };
    std::vector<int8_t> conv2d_input_zp = {1};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Tensors
    std::string identity1InputName   = "identity1_input";
    std::string identity1OutputName  = "identity1_output";
    std::string conv2DWeightsName    = "weights";
    std::string conv2DBiasName       = "bias";
    std::string identity2InputName   = "identity2_input";
    std::string identity2OutputName  = "identity2_output";

    std::vector<int32_t> inputShape      = { 1, 7, 7, 1 };
    std::vector<int32_t> weightsShape    = { 1, 3, 3, 1 };
    std::vector<int32_t> biasShape       = { 1 };
    std::vector<int32_t> outputShape     = { 1, 3, 3, 1 };

    auto* identity1InputTensor  = new TosaSerializationTensor(identity1InputName, inputShape, DType::DType_INT8, {});
    auto* identity1OutputTensor = new TosaSerializationTensor(identity1OutputName, inputShape, DType::DType_INT8, {});
    auto* conv2DWeightsTensor   = new TosaSerializationTensor(conv2DWeightsName, weightsShape, DType::DType_INT8, {});
    auto* conv2DBiasTensor      = new TosaSerializationTensor(conv2DBiasName, biasShape, DType::DType_INT32, {});
    auto* identity2InputTensor  = new TosaSerializationTensor(identity2InputName, outputShape, DType::DType_INT32, {});
    auto* identity2OutputTensor = new TosaSerializationTensor(identity2OutputName, outputShape, DType::DType_INT32, {});

    // Create Operators Conv2D, Rescale, Conv2D
    auto weightsOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         {},
                                                         { conv2DWeightsName });
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                      Attribute::Attribute_NONE,
                                                      nullptr,
                                                      {},
                                                      { conv2DBiasName });
    auto conv2d_op = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                         Attribute::Attribute_ConvAttribute,
                                                         &conv2DAttribute,
                                                         { identity1OutputName, conv2DWeightsName, conv2DBiasName },
                                                         { identity2InputName });
    auto identity1_op = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                            Attribute::Attribute_NONE,
                                                            nullptr,
                                                            { identity1InputName },
                                                            { identity1OutputName });
    auto identity2_op = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                            Attribute::Attribute_NONE,
                                                            nullptr,
                                                            { identity2InputName },
                                                            { identity2OutputName });

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("multi_Layer",
                                      "main",
                                      { weightsOp, biasOp, identity1_op, conv2d_op, identity2_op },
                                      { identity1InputTensor, identity1OutputTensor, conv2DWeightsTensor,
                                        conv2DBiasTensor, identity2InputTensor, identity2OutputTensor },
                                      { identity1InputName },
                                      { identity2OutputName });

    return block;
}

TEST(TOSA2SPIRV_PARSER, IdentityConv2dIdentity)
{
    // Create a IdentityConv2dIdentity model block.
    TosaSerializationBasicBlock block = GeneratorIdentityConv2DIdentityModel();

    TosaSerializationParser parser(&block);

    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, false));

    tosa2spirv::spirvwriter::CheckInputTensor({1, 7, 7, 1}, DataType::int8_t, "CONV2D", outputStr);
    tosa2spirv::spirvwriter::CheckInputTensor({1, 3, 3, 1}, DataType::int8_t, "CONV2D", outputStr);
    tosa2spirv::spirvwriter::CheckInputTensor({ 1 }, DataType::int32_t, "CONV2D", outputStr);
    tosa2spirv::spirvwriter::CheckConstCompositeTensor({1, 1, 1, 1}, "CONV2D", outputStr, 0);
    tosa2spirv::spirvwriter::CheckConstCompositeTensor({2, 2}, "CONV2D", outputStr, 1);
    tosa2spirv::spirvwriter::CheckConstCompositeTensor({1, 1}, "CONV2D", outputStr, 2);
    tosa2spirv::spirvwriter::CheckConstant(DataType::int8_t, "CONV2D", outputStr, 1, 3);
    tosa2spirv::spirvwriter::CheckConstant(DataType::int8_t, "CONV2D", outputStr, 1, 4);
    tosa2spirv::spirvwriter::CheckBoolConstant(DataType::bool_t, "CONV2D", outputStr, true, 5);
    tosa2spirv::spirvwriter::CheckOutputTensor({1, 3, 3, 1}, DataType::int32_t, "CONV2D", outputStr);
}

// Multi Layer CONV2D - IDENTITY - RESCALE - CONV2D reusing the weight and bias GraphConstantId
// It should be identical to CONV2D - RESCALE - CONV2D
TosaSerializationBasicBlock GeneratorConv2DIdentityConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad      = { 1, 1, 1, 1 };
    std::vector<int32_t> dilation = { 1, 1 };
    std::vector<int32_t> stride   = { 2, 2 };
    std::vector<int8_t> conv2d_input_zp = {-127};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Rescale Attribute
    std::vector<int8_t> rescale_input_zp = {0};
    std::vector<int8_t> output_zp        = {0};
    std::vector<int32_t> multiplier      = {8};
    std::vector<int32_t> shift           = {8};
    bool scale32       = true;
    bool double_round  = false;
    bool per_channel   = true;
    bool input_signed  = true;
    bool output_signed = true;
    TosaRescaleAttribute rescaleAttribute(
        rescale_input_zp[0], output_zp[0], multiplier, shift, scale32,
        double_round, per_channel, input_signed, output_signed);

    // Create Tensors
    std::string inputName       = "input1";
    std::string weightsName     = "weights";
    std::string biasName        = "bias";
    std::string identityInName  = "identity_input";
    std::string identityOutName = "identity_output";
    std::string rescaleOutName  = "rescale_output";
    std::string outputName      = "output1";
    std::vector<int32_t> inputShape       = { 1, 7, 7, 1 };
    std::vector<int32_t> weightsShape     = { 1, 3, 3, 1 };
    std::vector<int32_t> biasShape        = { 1 };
    std::vector<int32_t> InterTensorShape = { 1, 5, 5, 1 };
    std::vector<int32_t> outputShape      = { 1, 3, 3, 1 };

    auto* inputTensor       = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightsTensor     = new TosaSerializationTensor(weightsName, weightsShape, DType::DType_INT8, {});
    auto* biasTensor        = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {});
    auto* identityInTensor  = new TosaSerializationTensor(identityInName, InterTensorShape, DType::DType_INT32, {});
    auto* identityOutTensor = new TosaSerializationTensor(identityOutName, InterTensorShape, DType::DType_INT32, {});
    auto* rescaleOutTensor  = new TosaSerializationTensor(rescaleOutName, InterTensorShape, DType::DType_INT8, {});
    auto* outputTensor      = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operators Conv2D, Rescale, Conv2D
    auto weightsOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         {},
                                                         { weightsName });
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                      Attribute::Attribute_NONE,
                                                      nullptr,
                                                      {},
                                                      { biasName });
    auto op1 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   { inputName, weightsName, biasName },
                                                   { identityInName });
    auto op2 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                   Attribute::Attribute_NONE,
                                                   nullptr,
                                                   { identityInName },
                                                   { identityOutName });
    auto op3 = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                   Attribute::Attribute_RescaleAttribute,
                                                   &rescaleAttribute,
                                                   { identityOutName },
                                                   { rescaleOutName });
    auto op4 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   { rescaleOutName, weightsName, biasName },
                                                   { outputName });

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("multi_layer",
                                      "main",
                                      { weightsOp, biasOp, op1, op2, op3, op4 },
                                      { inputTensor, weightsTensor, biasTensor,
                                        identityInTensor, identityOutTensor, rescaleOutTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    return block;
}

TEST(TOSA2SPIRV_PARSER, Conv2dIdentityConv2d)
{
    // Create a IdentityConv2dIdentity model block.
    TosaSerializationBasicBlock block = GeneratorConv2DIdentityConv2DModel();

    TosaSerializationParser parser(&block);

    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    Conv2DRescaleConv2DExpected expected;

    EXPECT_EQ(expected.goldenSPIRV, outputStr);
}

// Multi Layer CONV2D - IDENTITY - (Graph Output) - IDENTITY - RESCALE - CONV2D
// reusing the weight and bias GraphConstantId
// It should be identical to CONV2D - RESCALE - CONV2D with an extra graph output from the first CONV2D
TosaSerializationBasicBlock GeneratorConv2DIdentityOutputConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int32_t> pad      = { 1, 1, 1, 1 };
    std::vector<int32_t> dilation = { 1, 1 };
    std::vector<int32_t> stride   = { 2, 2 };
    std::vector<int8_t> conv2d_input_zp = {-127};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute conv2DAttribute(pad, stride, dilation, conv2d_input_zp[0], weight_zp[0], true);

    // Create Rescale Attribute
    std::vector<int8_t> rescale_input_zp = {0};
    std::vector<int8_t> output_zp        = {0};
    std::vector<int32_t> multiplier      = {8};
    std::vector<int32_t> shift           = {8};
    bool scale32       = true;
    bool double_round  = false;
    bool per_channel   = true;
    bool input_signed  = true;
    bool output_signed = true;
    TosaRescaleAttribute rescaleAttribute(
        rescale_input_zp[0], output_zp[0], multiplier, shift, scale32,
        double_round, per_channel, input_signed, output_signed);

    // Create Tensors
    std::string inputName       = "input1";
    std::string weightsName     = "weights";
    std::string biasName        = "bias";
    std::string identity1InName  = "identity1_input";
    std::string identity1OutName = "identity1_output";
    std::string identity2OutName = "identity2_output";
    std::string rescaleOutName  = "rescale_output";
    std::string outputName      = "output1";
    std::vector<int32_t> inputShape       = { 1, 7, 7, 1 };
    std::vector<int32_t> weightsShape     = { 1, 3, 3, 1 };
    std::vector<int32_t> biasShape        = { 1 };
    std::vector<int32_t> InterTensorShape = { 1, 5, 5, 1 };
    std::vector<int32_t> outputShape      = { 1, 3, 3, 1 };

    auto* inputTensor       = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightsTensor     = new TosaSerializationTensor(weightsName, weightsShape, DType::DType_INT8, {});
    auto* biasTensor        = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {});
    auto* identity1InTensor  = new TosaSerializationTensor(identity1InName, InterTensorShape, DType::DType_INT32, {});
    auto* identity1OutTensor = new TosaSerializationTensor(identity1OutName, InterTensorShape, DType::DType_INT32, {});
    auto* identity2OutTensor = new TosaSerializationTensor(identity2OutName, InterTensorShape, DType::DType_INT32, {});
    auto* rescaleOutTensor  = new TosaSerializationTensor(rescaleOutName, InterTensorShape, DType::DType_INT8, {});
    auto* outputTensor      = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operators Conv2D, Rescale, Conv2D
    auto weightsOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         {},
                                                         { weightsName });
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                      Attribute::Attribute_NONE,
                                                      nullptr,
                                                      {},
                                                      { biasName });
    auto op1 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   { inputName, weightsName, biasName },
                                                   { identity1InName });
    auto op2 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                   Attribute::Attribute_NONE,
                                                   nullptr,
                                                   { identity1InName },
                                                   { identity1OutName });
    auto op3 = new tosa::TosaSerializationOperator(Op::Op_IDENTITY,
                                                   Attribute::Attribute_NONE,
                                                   nullptr,
                                                   { identity1OutName },
                                                   { identity2OutName });
    auto op4 = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                   Attribute::Attribute_RescaleAttribute,
                                                   &rescaleAttribute,
                                                   { identity2OutName },
                                                   { rescaleOutName });
    auto op5 = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                   Attribute::Attribute_ConvAttribute,
                                                   &conv2DAttribute,
                                                   { rescaleOutName, weightsName, biasName },
                                                   { outputName });

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("multi_layer",
                                      "main",
                                      { weightsOp, biasOp, op1, op2, op3, op4, op5 },
                                      { inputTensor, weightsTensor, biasTensor,
                                        identity1InTensor, identity1OutTensor, identity2OutTensor,
                                        rescaleOutTensor, outputTensor },
                                      { inputName },
                                      { outputName, identity1OutName });

    return block;
}

struct Conv2DRescaleConv2DDualOutputExpected
{
    std::string goldenSPIRV = GetPreSPIRVStr() +
        R"(         %11 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %43 DescriptorSet 0
               OpDecorate %43 Binding 0
               OpDecorate %47 DescriptorSet 0
               OpDecorate %47 Binding 2
               OpDecorate %53 DescriptorSet 0
               OpDecorate %53 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_3 = OpConstant %uint 3
         %18 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_3 %uint_3 %uint_1
         %19 = OpTypeTensorARM %uchar %uint_4 %18
          %2 = OpGraphConstantARM %19 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %21 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %22 = OpTypeTensorARM %uint %uint_1 %21
          %3 = OpGraphConstantARM %22 1
          %5 = OpGraphConstantARM %22 2
         %23 = OpTypeTensorARM %uchar %uint_1 %21
          %6 = OpGraphConstantARM %23 3
          %8 = OpGraphConstantARM %19 4
          %9 = OpGraphConstantARM %22 5
     %uint_5 = OpConstant %uint 5
         %25 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_5 %uint_5 %uint_1
         %26 = OpTypeTensorARM %uint %uint_4 %25
         %27 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %28 = OpTypeTensorARM %uint %uint_1 %27
         %29 = OpConstantCompositeReplicateEXT %28 %uint_1
     %uint_2 = OpConstant %uint 2
         %31 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %32 = OpTypeTensorARM %uint %uint_1 %31
         %33 = OpConstantCompositeReplicateEXT %32 %uint_2
         %34 = OpConstantCompositeReplicateEXT %32 %uint_1
  %uchar_129 = OpConstant %uchar 129
    %uchar_1 = OpConstant %uchar 1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
     %uint_7 = OpConstant %uint 7
         %40 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_7 %uint_7 %uint_1
         %41 = OpTypeTensorARM %uchar %uint_4 %40
%_ptr_UniformConstant_41 = OpTypePointer UniformConstant %41
     %uint_0 = OpConstant %uint 0
%_ptr_UniformConstant_26 = OpTypePointer UniformConstant %26
         %43 = OpVariable %_ptr_UniformConstant_41 UniformConstant
         %47 = OpVariable %_ptr_UniformConstant_26 UniformConstant
         %48 = OpTypeTensorARM %uchar %uint_4 %25
    %uchar_0 = OpConstant %uchar 0
      %false = OpConstantFalse %bool
         %51 = OpTypeTensorARM %uint %uint_4 %18
%_ptr_UniformConstant_51 = OpTypePointer UniformConstant %51
         %53 = OpVariable %_ptr_UniformConstant_51 UniformConstant
         %54 = OpTypeGraphARM 1 %41 %26 %51
               OpGraphEntryPointARM %1 "main" %43 %47 %53
          %1 = OpGraphARM %54
         %45 = OpGraphInputARM %41 %uint_0
          %4 = OpExtInst %26 %11 CONV2D %29 %33 %34 %uchar_129 %uchar_1 %true %45 %2 %3
          %7 = OpExtInst %48 %11 RESCALE %uint_0 %uchar_0 %5 %6 %true %false %true %true %true %4
         %10 = OpExtInst %51 %11 CONV2D %29 %33 %34 %uchar_129 %uchar_1 %true %7 %8 %9
               OpGraphSetOutputARM %10 %uint_0
               OpGraphSetOutputARM %4 %uint_1
               OpGraphEndARM
)";
};

TEST(TOSA2SPIRV_PARSER, Conv2dIdentityConv2dDualOutput)
{
    // Create a IdentityConv2dIdentity model block.
    TosaSerializationBasicBlock block = GeneratorConv2DIdentityOutputConv2DModel();

    TosaSerializationParser parser(&block);

    auto binarySpirv = parser.GenerateSPIRV("main");
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    Conv2DRescaleConv2DDualOutputExpected expected;

    EXPECT_EQ(expected.goldenSPIRV, outputStr);
}

// Struct containing golden SPIRV of Add operator with one input having an empty shape.
struct ZeroDimTensorOutputExpected
{
    std::string goldenSPIRV = GetPreSPIRVStr() +
        R"(          %3 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %14 DescriptorSet 0
               OpDecorate %14 Binding 0
               OpDecorate %18 DescriptorSet 0
               OpDecorate %18 Binding 1
               OpDecorate %20 DescriptorSet 0
               OpDecorate %20 Binding 2
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
          %8 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_1 %uint_1
          %9 = OpTypeTensorARM %uint %uint_4 %8
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %11 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %12 = OpTypeTensorARM %uint %uint_1 %11
%_ptr_UniformConstant_12 = OpTypePointer UniformConstant %12
     %uint_0 = OpConstant %uint 0
%_ptr_UniformConstant_9 = OpTypePointer UniformConstant %9
         %14 = OpVariable %_ptr_UniformConstant_12 UniformConstant
         %18 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %20 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %21 = OpTypeGraphARM 2 %12 %9 %9
               OpGraphEntryPointARM %1 "main" %14 %18 %20
          %1 = OpGraphARM %21
         %16 = OpGraphInputARM %12 %uint_0
         %19 = OpGraphInputARM %9 %uint_1
          %2 = OpExtInst %9 %3 ADD %16 %19
               OpGraphSetOutputARM %2 %uint_0
               OpGraphEndARM
)";
};

// Test case to test that we correctly transform empty tensor shape to shape of { 1 } otherwise this would fail.
TEST(TOSA2SPIRV_PARSER, EmptyTensor)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = { };
    std::vector<int32_t> input2Shape = {1,1,1,1};
    std::vector<int32_t> outputShape = {1,1,1,1};

    auto* input1Tensor = new TosaSerializationTensor(input1Name, input1Shape, DType::DType_INT32, {});
    auto* input2Tensor = new TosaSerializationTensor(input2Name, input2Shape, DType::DType_INT32, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create a Add model block.
    auto addOp = new tosa::TosaSerializationOperator(Op::Op_ADD,
                                                     Attribute::Attribute_NONE,
                                                     nullptr,
                                                     { input1Name, input2Name },
                                                     { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("add",
                                      "main",
                                      { addOp },
                                      { input1Tensor, input2Tensor, outputTensor },
                                      { input1Name, input2Name },
                                      { outputName });

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");

    ZeroDimTensorOutputExpected expected;
    const std::string outputStr(spirvwriter::DisassembleSPIRV(binarySpirv, true));

    EXPECT_EQ(expected.goldenSPIRV, outputStr);
}