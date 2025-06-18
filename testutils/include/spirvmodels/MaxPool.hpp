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
          %4 = OpTypeInt 8 0
          %2 = OpTypeInt 32 0
          %5 = OpConstant %2 4
          %6 = OpTypeArray %2 %5
          %7 = OpConstant %2 1
          %8 = OpConstantComposite %6 %7 %7 %5 %5
          %9 = OpTypeTensorARM %4 %5 %8
         %10 = OpTypePointer UniformConstant %9
         %11 = OpVariable %10 UniformConstant
         %13 = OpConstant %2 3
         %14 = OpConstantComposite %6 %7 %7 %13 %13
         %15 = OpTypeTensorARM %4 %5 %14
         %25 = OpTypePointer UniformConstant %15
         %26 = OpVariable %25 UniformConstant
          %3 = OpConstant %2 0
         %16 = OpTypeArray %2 %7
         %17 = OpConstant %2 2
         %18 = OpConstantComposite %16 %17
         %19 = OpTypeTensorARM %2 %7 %18
         %20 = OpConstantCompositeReplicateEXT %19 %17
         %21 = OpConstantComposite %16 %5
         %22 = OpTypeTensorARM %2 %7 %21
         %23 = OpConstantCompositeReplicateEXT %22 %7
         %27 = OpTypeGraphARM 1 %9 %15
               OpGraphEntryPointARM %28 "main" %11 %26
         %28 = OpGraphARM %27
         %12 = OpGraphInputARM %9 %3
         %24 = OpExtInst %15 %1 MAX_POOL2D %20 %20 %23 %7 %12
               OpGraphSetOutputARM %24 %3
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
          %1 = OpExtInstImport "TOSA.001000.1"
               OpMemoryModel Logical Vulkan
               OpDecorate %11 Binding 0
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
         %24 = OpExtInst %15 %1 MAX_POOL2D %20 %20 %23 %uint_1 %12
               OpGraphSetOutputARM %24 %uint_0
               OpGraphEndARM
)";
} // namespace spirvmodels