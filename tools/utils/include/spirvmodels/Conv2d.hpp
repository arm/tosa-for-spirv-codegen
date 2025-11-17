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
               OpDecorate %40 DescriptorSet 0
               OpDecorate %40 Binding 1
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
         %26 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_3 %uint_3 %uint_1
         %27 = OpTypeTensorARM %uint %uint_4 %26
%_ptr_UniformConstant_27 = OpTypePointer UniformConstant %27
         %40 = OpVariable %_ptr_UniformConstant_27 UniformConstant
     %uint_6 = OpConstant %uint 6
         %16 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_3 %uint_6 %uint_1
         %17 = OpTypeTensorARM %uchar %uint_4 %16
         %18 = OpGraphConstantARM %17 0
         %19 = OpTypeTensorARM %uint %uint_4 %16
         %20 = OpGraphConstantARM %19 1
     %uint_0 = OpConstant %uint 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %28 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %29 = OpTypeTensorARM %uint %uint_1 %28
         %30 = OpConstantCompositeReplicateEXT %29 %uint_1
     %uint_2 = OpConstant %uint 2
         %32 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %33 = OpTypeTensorARM %uint %uint_1 %32
         %34 = OpConstantCompositeReplicateEXT %33 %uint_2
         %35 = OpConstantCompositeReplicateEXT %33 %uint_1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
         %22 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %23 = OpTypeTensorARM %uchar %uint_1 %22
    %uchar_1 = OpConstant %uchar 1
         %25 = OpConstantCompositeReplicateEXT %23 %uchar_1
         %41 = OpTypeGraphARM 1 %10 %27
               OpGraphEntryPointARM %42 "main" %12 %40
         %42 = OpGraphARM %41
         %13 = OpGraphInputARM %10 %uint_0
         %38 = OpExtInst %27 %1 CONV2D %30 %34 %35 %uint_1 %true %13 %18 %20 %25 %25
               OpGraphSetOutputARM %38 %uint_0
               OpGraphEndARM
)";

static const std::string Conv2DRescaleConv2DDualOutput = SPIRV_HEADER +
                                                         R"(               OpDecorate %12 Binding 0
               OpDecorate %12 DescriptorSet 0
               OpDecorate %45 DescriptorSet 0
               OpDecorate %45 Binding 1
               OpDecorate %47 DescriptorSet 0
               OpDecorate %47 Binding 2
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
         %42 = OpTypeTensorARM %uint %uint_4 %15
%_ptr_UniformConstant_42 = OpTypePointer UniformConstant %42
         %45 = OpVariable %_ptr_UniformConstant_42 UniformConstant
     %uint_5 = OpConstant %uint 5
         %28 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_5 %uint_5 %uint_1
         %29 = OpTypeTensorARM %uint %uint_4 %28
%_ptr_UniformConstant_29 = OpTypePointer UniformConstant %29
         %47 = OpVariable %_ptr_UniformConstant_29 UniformConstant
         %16 = OpTypeTensorARM %uchar %uint_4 %15
         %17 = OpGraphConstantARM %16 0
     %uint_0 = OpConstant %uint 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %30 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %31 = OpTypeTensorARM %uint %uint_1 %30
         %32 = OpConstantCompositeReplicateEXT %31 %uint_1
     %uint_2 = OpConstant %uint 2
         %34 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %35 = OpTypeTensorARM %uint %uint_1 %34
         %36 = OpConstantCompositeReplicateEXT %35 %uint_2
         %37 = OpConstantCompositeReplicateEXT %35 %uint_1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
         %19 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %20 = OpTypeTensorARM %uint %uint_1 %19
         %21 = OpConstantNull %20
         %22 = OpTypeTensorARM %uchar %uint_1 %19
    %uchar_1 = OpConstant %uchar 1
         %24 = OpConstantComposite %22 %uchar_1
%uint_16777216 = OpConstant %uint 16777216
         %26 = OpConstantComposite %20 %uint_16777216
         %48 = OpTypeGraphARM 1 %10 %42 %29
               OpGraphEntryPointARM %49 "main" %12 %45 %47
         %49 = OpGraphARM %48
         %13 = OpGraphInputARM %10 %uint_0
         %40 = OpExtInst %29 %1 CONV2D %32 %36 %37 %uint_1 %true %13 %17 %21 %24 %24
         %41 = OpExtInst %16 %1 RESCALE %true %uint_1 %true %true %true %40 %26 %24 %26 %24
         %43 = OpExtInst %42 %1 CONV2D %32 %36 %37 %uint_1 %true %41 %17 %21 %24 %24
               OpGraphSetOutputARM %43 %uint_0
               OpGraphSetOutputARM %40 %uint_1
               OpGraphEndARM
)";

static const std::string Conv2DRescaleConv2D = SPIRV_HEADER +
                                               R"(               OpDecorate %12 Binding 0
               OpDecorate %12 DescriptorSet 0
               OpDecorate %45 DescriptorSet 0
               OpDecorate %45 Binding 1
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
         %42 = OpTypeTensorARM %uint %uint_4 %15
%_ptr_UniformConstant_42 = OpTypePointer UniformConstant %42
         %45 = OpVariable %_ptr_UniformConstant_42 UniformConstant
         %16 = OpTypeTensorARM %uchar %uint_4 %15
         %17 = OpGraphConstantARM %16 0
     %uint_0 = OpConstant %uint 0
     %uint_5 = OpConstant %uint 5
         %28 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_5 %uint_5 %uint_1
         %29 = OpTypeTensorARM %uint %uint_4 %28
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %30 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %31 = OpTypeTensorARM %uint %uint_1 %30
         %32 = OpConstantCompositeReplicateEXT %31 %uint_1
     %uint_2 = OpConstant %uint 2
         %34 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %35 = OpTypeTensorARM %uint %uint_1 %34
         %36 = OpConstantCompositeReplicateEXT %35 %uint_2
         %37 = OpConstantCompositeReplicateEXT %35 %uint_1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
         %19 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %20 = OpTypeTensorARM %uint %uint_1 %19
         %21 = OpConstantNull %20
         %22 = OpTypeTensorARM %uchar %uint_1 %19
    %uchar_1 = OpConstant %uchar 1
         %24 = OpConstantComposite %22 %uchar_1
%uint_16777216 = OpConstant %uint 16777216
         %26 = OpConstantComposite %20 %uint_16777216
         %46 = OpTypeGraphARM 1 %10 %42
               OpGraphEntryPointARM %47 "main" %12 %45
         %47 = OpGraphARM %46
         %13 = OpGraphInputARM %10 %uint_0
         %40 = OpExtInst %29 %1 CONV2D %32 %36 %37 %uint_1 %true %13 %17 %21 %24 %24
         %41 = OpExtInst %16 %1 RESCALE %true %uint_1 %true %true %true %40 %26 %24 %26 %24
         %43 = OpExtInst %42 %1 CONV2D %32 %36 %37 %uint_1 %true %41 %17 %21 %24 %24
               OpGraphSetOutputARM %43 %uint_0
               OpGraphEndARM
)";

} // namespace spirvmodels
