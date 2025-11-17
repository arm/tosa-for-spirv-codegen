//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "Header.hpp"
#include <string>

namespace spirvmodels
{
static const std::string simpleRescale = SPIRV_HEADER +
                                         R"(               OpDecorate %11 Binding 0
               OpDecorate %11 DescriptorSet 0
               OpDecorate %27 DescriptorSet 0
               OpDecorate %27 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
          %8 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_4 %uint_4
          %9 = OpTypeTensorARM %uchar %uint_4 %8
%_ptr_UniformConstant_9 = OpTypePointer UniformConstant %9
         %11 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %27 = OpVariable %_ptr_UniformConstant_9 UniformConstant
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
    %uint_18 = OpConstant %uint 18
         %15 = OpConstantComposite %_arr_uint_uint_1 %uint_18
         %16 = OpTypeTensorARM %uint %uint_1 %15
         %17 = OpGraphConstantARM %16 0
         %18 = OpTypeTensorARM %uchar %uint_1 %15
         %19 = OpGraphConstantARM %18 1
     %uint_0 = OpConstant %uint 0
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
         %20 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %21 = OpTypeTensorARM %uchar %uint_1 %20
    %uchar_1 = OpConstant %uchar 1
         %23 = OpConstantComposite %21 %uchar_1
         %28 = OpTypeGraphARM 1 %9 %9
               OpGraphEntryPointARM %29 "main" %11 %27
         %29 = OpGraphARM %28
         %12 = OpGraphInputARM %9 %uint_0
         %26 = OpExtInst %9 %1 RESCALE %true %uint_1 %true %true %true %12 %17 %19 %23 %23
               OpGraphSetOutputARM %26 %uint_0
               OpGraphEndARM
)";

} // namespace spirvmodels
