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
               OpDecorate %25 DescriptorSet 0
               OpDecorate %25 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
          %8 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_4 %uint_4
          %9 = OpTypeTensorARM %uchar %uint_4 %8
%_ptr_UniformConstant_9 = OpTypePointer UniformConstant %9
         %11 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %25 = OpVariable %_ptr_UniformConstant_9 UniformConstant
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %14 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %15 = OpTypeTensorARM %uint %uint_1 %14
         %16 = OpGraphConstantARM %15 0
         %17 = OpTypeTensorARM %uchar %uint_1 %14
         %18 = OpGraphConstantARM %17 1
     %uint_0 = OpConstant %uint 0
   %uchar_20 = OpConstant %uchar 20
  %uchar_128 = OpConstant %uchar 128
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
      %false = OpConstantFalse %bool
         %26 = OpTypeGraphARM 1 %9 %9
               OpGraphEntryPointARM %27 "main" %11 %25
         %27 = OpGraphARM %26
         %12 = OpGraphInputARM %9 %uint_0
         %24 = OpExtInst %9 %1 RESCALE %uchar_20 %uchar_128 %16 %18 %true %false %true %true %true %12
               OpGraphSetOutputARM %24 %uint_0
               OpGraphEndARM
)";

} // namespace spirvmodels
