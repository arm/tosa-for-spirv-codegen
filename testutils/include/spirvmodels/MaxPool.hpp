//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include "Header.hpp"
#include <string>

namespace spirvmodels
{

static const std::string SimpleMaxpool2d = SPIRV_HEADER +
                                           R"(               OpDecorate %11 Binding 0
               OpDecorate %11 DescriptorSet 0
               OpDecorate %26 DescriptorSet 0
               OpDecorate %26 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
          %8 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_4 %uint_4
          %9 = OpTypeTensorARM %uchar %uint_4 %8
%_ptr_UniformConstant_9 = OpTypePointer UniformConstant %9
         %11 = OpVariable %_ptr_UniformConstant_9 UniformConstant
     %uint_3 = OpConstant %uint 3
         %14 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_3 %uint_3
         %15 = OpTypeTensorARM %uchar %uint_4 %14
%_ptr_UniformConstant_15 = OpTypePointer UniformConstant %15
         %26 = OpVariable %_ptr_UniformConstant_15 UniformConstant
     %uint_0 = OpConstant %uint 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
     %uint_2 = OpConstant %uint 2
         %18 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %19 = OpTypeTensorARM %uint %uint_1 %18
         %20 = OpConstantCompositeReplicateEXT %19 %uint_2
         %21 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %22 = OpTypeTensorARM %uint %uint_1 %21
         %23 = OpConstantCompositeReplicateEXT %22 %uint_1
         %27 = OpTypeGraphARM 1 %9 %15
               OpGraphEntryPointARM %28 "main" %11 %26
         %28 = OpGraphARM %27
         %12 = OpGraphInputARM %9 %uint_0
         %24 = OpExtInst %15 %1 MAX_POOL2D %20 %20 %23 %12
               OpGraphSetOutputARM %24 %uint_0
               OpGraphEndARM
)";

static const std::string SimpleMaxpool2dGenerator =
    R"(               OpCapability Shader
               OpCapability Float16
               OpCapability Int64
               OpCapability Int16
               OpCapability Int8
               OpCapability TensorsARM
               OpCapability GraphARM
               OpCapability VulkanMemoryModel
               OpCapability ReplicatedCompositesEXT
               OpExtension "SPV_ARM_graph"
               OpExtension "SPV_ARM_tensors"
               OpExtension "SPV_EXT_replicated_composites"
          %1 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %12 Binding 0
               OpDecorate %12 DescriptorSet 0
               OpDecorate %27 DescriptorSet 0
               OpDecorate %27 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
    %uint_16 = OpConstant %uint 16
          %9 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_16 %uint_16 %uint_16
         %10 = OpTypeTensorARM %uchar %uint_4 %9
%_ptr_UniformConstant_10 = OpTypePointer UniformConstant %10
         %12 = OpVariable %_ptr_UniformConstant_10 UniformConstant
     %uint_8 = OpConstant %uint 8
         %15 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_8 %uint_8 %uint_16
         %16 = OpTypeTensorARM %uchar %uint_4 %15
%_ptr_UniformConstant_16 = OpTypePointer UniformConstant %16
         %27 = OpVariable %_ptr_UniformConstant_16 UniformConstant
     %uint_0 = OpConstant %uint 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
     %uint_2 = OpConstant %uint 2
         %19 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %20 = OpTypeTensorARM %uint %uint_1 %19
         %21 = OpConstantCompositeReplicateEXT %20 %uint_2
         %22 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %23 = OpTypeTensorARM %uint %uint_1 %22
         %24 = OpConstantNull %23
         %28 = OpTypeGraphARM 1 %10 %16
               OpGraphEntryPointARM %29 "main" %12 %27
         %29 = OpGraphARM %28
         %13 = OpGraphInputARM %10 %uint_0
         %25 = OpExtInst %16 %1 MAX_POOL2D %21 %21 %24 %13
               OpGraphSetOutputARM %25 %uint_0
               OpGraphEndARM
)";
} // namespace spirvmodels