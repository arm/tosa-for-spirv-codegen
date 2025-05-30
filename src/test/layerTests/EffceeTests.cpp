//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <OpTestUtils.hpp>

#include <gtest/gtest.h>

struct goldenSPIRV
{
    const std::string maxpool2d = R"(
               OpCapability VulkanMemoryModel
               OpCapability Shader
               OpCapability Int8
               OpCapability Int16
               OpCapability Float16
               OpCapability GraphARM
               OpCapability TensorsARM
               OpExtension "SPV_ARM_graph"
               OpExtension "SPV_ARM_tensors"
          %3 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %20 DescriptorSet 0
               OpDecorate %20 Binding 0
               OpDecorate %23 DescriptorSet 0
               OpDecorate %23 Binding 1
       %uint = OpTypeInt 32 0
     %uint_1 = OpConstant %uint 1
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
     %uint_2 = OpConstant %uint 2
          %8 = OpConstantComposite %_arr_uint_uint_1 %uint_2
          %9 = OpTypeTensorARM %uint %uint_1 %8
         %10 = OpConstantComposite %9 %uint_1 %uint_1
     %uint_4 = OpConstant %uint 4
         %12 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %13 = OpTypeTensorARM %uint %uint_1 %12
         %14 = OpConstantCompositeReplicateEXT %13 %uint_1
      %uchar = OpTypeInt 8 0
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
         %17 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_1 %uint_1
         %18 = OpTypeTensorARM %uchar %uint_4 %17
%_ptr_UniformConstant_18 = OpTypePointer UniformConstant %18
     %uint_0 = OpConstant %uint 0
         %20 = OpVariable %_ptr_UniformConstant_18 UniformConstant
         %23 = OpVariable %_ptr_UniformConstant_18 UniformConstant
         %24 = OpTypeGraphARM 1 %18 %18
               OpGraphEntryPointARM %1 "Graph_1" %20 %23
          %1 = OpGraphARM %24
         %22 = OpGraphInputARM %18 %uint_0
          %2 = OpExtInst %18 %3 MAX_POOL2D %10 %10 %14 %22
               OpGraphSetOutputARM %2 %uint_0
               OpGraphEndARM
    )";

    const std::string rescale = tosa2spirv::GetPreSPIRVStr() + R"(
                  %3 = OpExtInstImport "TOSA.000080.1"
                       OpMemoryModel Logical Vulkan
                       OpDecorate %21 DescriptorSet 0
                       OpDecorate %21 Binding 0
                       OpDecorate %23 DescriptorSet 0
                       OpDecorate %23 Binding 1
              %uchar = OpTypeInt 8 0
            %uchar_1 = OpConstant %uchar 1
               %uint = OpTypeInt 32 0
             %uint_1 = OpConstant %uint 1
        %_arr_uint_uint_1 = OpTypeArray %uint %uint_1
                 %10 = OpConstantComposite %_arr_uint_uint_1 %uint_1
                 %11 = OpTypeTensorARM %uint %uint_1 %10
                  %6 = OpGraphConstantARM %11 0
                 %13 = OpTypeTensorARM %uchar %uint_1 %10
                 %12 = OpGraphConstantARM %13 1
               %bool = OpTypeBool
               %true = OpConstantTrue %bool
             %uint_4 = OpConstant %uint 4
        %_arr_uint_uint_4 = OpTypeArray %uint %uint_4
                 %18 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_1 %uint_1
                 %19 = OpTypeTensorARM %uchar %uint_4 %18
        %_ptr_UniformConstant_19 = OpTypePointer UniformConstant %19
                 %21 = OpVariable %_ptr_UniformConstant_19 UniformConstant
                 %23 = OpVariable %_ptr_UniformConstant_19 UniformConstant
                 %24 = OpTypeGraphARM 1 %19 %19
                       OpGraphEntryPointARM %1 "Graph_1" %21 %23
                  %1 = OpGraphARM %24
                 %22 = OpGraphInputARM %19
                  %2 = OpExtInst %19 %3 RESCALE %uchar_1 %uchar_1 %6 %12 %true %true %true %true %true %22
                       OpGraphSetOutputARM %2 0
                       OpGraphEndARM
    )";

    const std::string clamp = R"(
                                   OpDecorate %15 DescriptorSet 0
                                   OpDecorate %15 Binding 0
                                   OpDecorate %17 DescriptorSet 0
                                   OpDecorate %17 Binding 1
                          %uchar = OpTypeInt 8 0
                        %uchar_1 = OpConstant %uchar 1
               %uchar_4294967295 = OpConstant %uchar 4294967295
                           %uint = OpTypeInt 32 0
                         %uint_4 = OpConstant %uint 4
               %_arr_uint_uint_4 = OpTypeArray %uint %uint_4
                         %uint_1 = OpConstant %uint 1
                             %12 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_4 %uint_4
                             %13 = OpTypeTensorARM %uchar %uint_4 %12
        %_ptr_UniformConstant_13 = OpTypePointer UniformConstant %13
                             %15 = OpVariable %_ptr_UniformConstant_13 UniformConstant
                             %17 = OpVariable %_ptr_UniformConstant_13 UniformConstant
                             %18 = OpTypeGraphARM 1 %13 %13
                                   OpGraphEntryPointARM %1 \"Graph_1\" %15 %17
                              %1 = OpGraphARM %18
                             %16 = OpGraphInputARM %13
                              %2 = OpExtInst %13 %4 CLAMP %uchar_1 %uchar_4294967295 %16
                              %3 = OpExtInst %13 %4 CLAMP %uchar_1 %uchar_4294967295 %2
                                   OpGraphSetOutputARM %3
                                   OpGraphEndARM
    )";
};

namespace ts = tosa2spirv::spirvwriter;

TEST(TOSA2SPIRV_LAYERS, CheckInputTensor)
{
    ts::CheckInputTensor({1, 1, 1, 1}, DataType::uint8_t, "MAX_POOL2D", goldenSPIRV().maxpool2d);
}

TEST(TOSA2SPIRV_LAYERS, CheckOutputTensor)
{
    ts::CheckOutputTensor({1, 1, 1, 1}, DataType::uint8_t, "MAX_POOL2D", goldenSPIRV().maxpool2d);
}

TEST(TOSA2SPIRV_LAYERS, CheckConstCompositeTensor)
{
    ts::CheckConstCompositeTensor({1, 1, 1, 1}, "MAX_POOL2D", goldenSPIRV().maxpool2d, 2);
}

TEST(TOSA2SPIRV_LAYERS, CheckGraphConstant)
{
    ts::CheckGraphConstant({1}, DataType::uint8_t, "RESCALE", goldenSPIRV().rescale);
}

TEST(TOSA2SPIRV_LAYERS, CheckConstant)
{
    ts::CheckConstant(DataType::uint8_t, "CLAMP", goldenSPIRV().clamp, 4294967295, 1);
}

TEST(TOSA2SPIRV_LAYERS, CheckBoolConstant)
{
    ts::CheckBoolConstant(DataType::bool_t, "RESCALE", goldenSPIRV().rescale, true, 4);
}
