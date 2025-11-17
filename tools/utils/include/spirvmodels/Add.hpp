//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include "Header.hpp"
#include <string>

namespace spirvmodels
{
static const std::string AddZeroDimOutputTensor = SPIRV_HEADER +

                                                  R"(               OpDecorate %9 Binding 0
               OpDecorate %9 DescriptorSet 0
               OpDecorate %16 DescriptorSet 0
               OpDecorate %16 Binding 1
               OpDecorate %19 DescriptorSet 0
               OpDecorate %19 Binding 2
       %uint = OpTypeInt 32 0
     %uint_1 = OpConstant %uint 1
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
          %6 = OpConstantComposite %_arr_uint_uint_1 %uint_1
          %7 = OpTypeTensorARM %uint %uint_1 %6
%_ptr_UniformConstant_7 = OpTypePointer UniformConstant %7
          %9 = OpVariable %_ptr_UniformConstant_7 UniformConstant
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
         %13 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_1 %uint_1
         %14 = OpTypeTensorARM %uint %uint_4 %13
%_ptr_UniformConstant_14 = OpTypePointer UniformConstant %14
         %16 = OpVariable %_ptr_UniformConstant_14 UniformConstant
         %19 = OpVariable %_ptr_UniformConstant_14 UniformConstant
     %uint_0 = OpConstant %uint 0
         %20 = OpTypeGraphARM 2 %7 %14 %14
               OpGraphEntryPointARM %21 "main" %9 %16 %19
         %21 = OpGraphARM %20
         %10 = OpGraphInputARM %7 %uint_0
         %17 = OpGraphInputARM %14 %uint_1
         %18 = OpExtInst %14 %1 ADD %10 %17
               OpGraphSetOutputARM %18 %uint_0
               OpGraphEndARM
)";
} // namespace spirvmodels