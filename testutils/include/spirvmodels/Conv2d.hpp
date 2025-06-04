//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include "Header.hpp"
#include <string>

namespace spirvmodels
{
static const std::string SimpleConv2d = SPIRV_HEADER +
                                        R"(               OpDecorate %12 Binding 0
               OpDecorate %12 DescriptorSet 0
               OpDecorate %37 DescriptorSet 0
               OpDecorate %37 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_5 = OpConstant %uint 5
          %9 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_5 %uint_5 %uint_1
         %10 = OpTypeTensorARM %uchar %uint_4 %9
%_ptr_UniformConstant_10 = OpTypePointer UniformConstant %10
         %12 = OpVariable %_ptr_UniformConstant_10 UniformConstant
     %uint_3 = OpConstant %uint 3
         %15 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_3 %uint_3 %uint_1
         %23 = OpTypeTensorARM %uint %uint_4 %15
%_ptr_UniformConstant_23 = OpTypePointer UniformConstant %23
         %37 = OpVariable %_ptr_UniformConstant_23 UniformConstant
         %16 = OpTypeTensorARM %uchar %uint_4 %15
         %17 = OpGraphConstantARM %16 0
     %uint_0 = OpConstant %uint 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %24 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %25 = OpTypeTensorARM %uint %uint_1 %24
         %26 = OpConstantCompositeReplicateEXT %25 %uint_1
     %uint_2 = OpConstant %uint 2
         %27 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %28 = OpTypeTensorARM %uint %uint_1 %27
         %29 = OpConstantCompositeReplicateEXT %28 %uint_2
         %30 = OpConstantCompositeReplicateEXT %28 %uint_1
  %uchar_129 = OpConstant %uchar 129
    %uchar_1 = OpConstant %uchar 1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
         %19 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %20 = OpTypeTensorARM %uint %uint_1 %19
         %22 = OpConstantCompositeReplicateEXT %20 %uint_2
         %38 = OpTypeGraphARM 1 %10 %23
               OpGraphEntryPointARM %39 "main" %12 %37
         %39 = OpGraphARM %38
         %13 = OpGraphInputARM %10 %uint_0
         %35 = OpExtInst %23 %1 CONV2D %26 %29 %30 %uchar_129 %uchar_1 %true %13 %17 %22
               OpGraphSetOutputARM %35 %uint_0
               OpGraphEndARM
)";

static const std::string Conv2DRescaleConv2DDualOutput = SPIRV_HEADER +
                                                         R"(               OpDecorate %12 Binding 0
               OpDecorate %12 Binding 0
               OpDecorate %12 DescriptorSet 0
               OpDecorate %48 DescriptorSet 0
               OpDecorate %48 Binding 1
               OpDecorate %50 DescriptorSet 0
               OpDecorate %50 Binding 2
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_7 = OpConstant %uint 7
          %9 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_7 %uint_7 %uint_1
         %10 = OpTypeTensorARM %uchar %uint_4 %9
%_ptr_UniformConstant_10 = OpTypePointer UniformConstant %10
         %12 = OpVariable %_ptr_UniformConstant_10 UniformConstant
     %uint_3 = OpConstant %uint 3
         %15 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_3 %uint_3 %uint_1
         %45 = OpTypeTensorARM %uint %uint_4 %15
%_ptr_UniformConstant_45 = OpTypePointer UniformConstant %45
         %48 = OpVariable %_ptr_UniformConstant_45 UniformConstant
     %uint_5 = OpConstant %uint 5
         %23 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_5 %uint_5 %uint_1
         %24 = OpTypeTensorARM %uint %uint_4 %23
%_ptr_UniformConstant_24 = OpTypePointer UniformConstant %24
         %50 = OpVariable %_ptr_UniformConstant_24 UniformConstant
         %16 = OpTypeTensorARM %uchar %uint_4 %15
         %17 = OpGraphConstantARM %16 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %19 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %20 = OpTypeTensorARM %uint %uint_1 %19
         %38 = OpGraphConstantARM %20 1
         %39 = OpTypeTensorARM %uchar %uint_1 %19
         %40 = OpGraphConstantARM %39 2
     %uint_0 = OpConstant %uint 0
         %25 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %26 = OpTypeTensorARM %uint %uint_1 %25
         %27 = OpConstantCompositeReplicateEXT %26 %uint_1
     %uint_2 = OpConstant %uint 2
         %29 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %30 = OpTypeTensorARM %uint %uint_1 %29
         %31 = OpConstantCompositeReplicateEXT %30 %uint_2
         %32 = OpConstantCompositeReplicateEXT %30 %uint_1
  %uchar_129 = OpConstant %uchar 129
    %uchar_1 = OpConstant %uchar 1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
         %21 = OpConstantNull %20
         %41 = OpTypeTensorARM %uchar %uint_4 %23
    %uchar_0 = OpConstant %uchar 0
      %false = OpConstantFalse %bool
         %51 = OpTypeGraphARM 1 %10 %45 %24
               OpGraphEntryPointARM %52 "main" %12 %48 %50
         %52 = OpGraphARM %51
         %13 = OpGraphInputARM %10 %uint_0
         %37 = OpExtInst %24 %1 CONV2D %27 %31 %32 %uchar_129 %uchar_1 %true %13 %17 %21
         %44 = OpExtInst %41 %1 RESCALE %uint_0 %uchar_0 %38 %40 %true %false %true %true %true %37
         %46 = OpExtInst %45 %1 CONV2D %27 %31 %32 %uchar_129 %uchar_1 %true %44 %17 %21
               OpGraphSetOutputARM %46 %uint_0
               OpGraphSetOutputARM %37 %uint_1
               OpGraphEndARM
)";

static const std::string Conv2DRescaleConv2D = SPIRV_HEADER +
                                               R"(               OpDecorate %12 Binding 0
               OpDecorate %12 DescriptorSet 0
               OpDecorate %48 DescriptorSet 0
               OpDecorate %48 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_7 = OpConstant %uint 7
          %9 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_7 %uint_7 %uint_1
         %10 = OpTypeTensorARM %uchar %uint_4 %9
%_ptr_UniformConstant_10 = OpTypePointer UniformConstant %10
         %12 = OpVariable %_ptr_UniformConstant_10 UniformConstant
     %uint_3 = OpConstant %uint 3
         %15 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_3 %uint_3 %uint_1
         %45 = OpTypeTensorARM %uint %uint_4 %15
%_ptr_UniformConstant_45 = OpTypePointer UniformConstant %45
         %48 = OpVariable %_ptr_UniformConstant_45 UniformConstant
         %16 = OpTypeTensorARM %uchar %uint_4 %15
         %17 = OpGraphConstantARM %16 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %19 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %20 = OpTypeTensorARM %uint %uint_1 %19
         %38 = OpGraphConstantARM %20 1
         %39 = OpTypeTensorARM %uchar %uint_1 %19
         %40 = OpGraphConstantARM %39 2
     %uint_0 = OpConstant %uint 0
     %uint_5 = OpConstant %uint 5
         %23 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_5 %uint_5 %uint_1
         %24 = OpTypeTensorARM %uint %uint_4 %23
         %25 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %26 = OpTypeTensorARM %uint %uint_1 %25
         %27 = OpConstantCompositeReplicateEXT %26 %uint_1
     %uint_2 = OpConstant %uint 2
         %29 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %30 = OpTypeTensorARM %uint %uint_1 %29
         %31 = OpConstantCompositeReplicateEXT %30 %uint_2
         %32 = OpConstantCompositeReplicateEXT %30 %uint_1
  %uchar_129 = OpConstant %uchar 129
    %uchar_1 = OpConstant %uchar 1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
         %21 = OpConstantNull %20
         %41 = OpTypeTensorARM %uchar %uint_4 %23
    %uchar_0 = OpConstant %uchar 0
      %false = OpConstantFalse %bool
         %49 = OpTypeGraphARM 1 %10 %45
               OpGraphEntryPointARM %50 "main" %12 %48
         %50 = OpGraphARM %49
         %13 = OpGraphInputARM %10 %uint_0
         %37 = OpExtInst %24 %1 CONV2D %27 %31 %32 %uchar_129 %uchar_1 %true %13 %17 %21
         %44 = OpExtInst %41 %1 RESCALE %uint_0 %uchar_0 %38 %40 %true %false %true %true %true %37
         %46 = OpExtInst %45 %1 CONV2D %27 %31 %32 %uchar_129 %uchar_1 %true %44 %17 %21
               OpGraphSetOutputARM %46 %uint_0
               OpGraphEndARM
)";
} // namespace spirvmodels
