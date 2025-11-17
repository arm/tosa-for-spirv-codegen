//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include "Header.hpp"
#include <string>

namespace spirvmodels
{
static const std::string GetTosaSerializationHandlerTensorNameTestGolden = SPIRV_HEADER +

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
               OpDecorate %13 DescriptorSet 0
               OpDecorate %13 Binding 1
               OpDecorate %15 DescriptorSet 0
               OpDecorate %15 Binding 2
               OpDecorate %39 DescriptorSet 0
               OpDecorate %39 Binding 3
               OpDecorate %40 DescriptorSet 0
               OpDecorate %40 Binding 4
               OpDecorate %41 DescriptorSet 0
               OpDecorate %41 Binding 5
      %float = OpTypeFloat 32
       %uint = OpTypeInt 32 0
     %uint_1 = OpConstant %uint 1
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
     %uint_2 = OpConstant %uint 2
          %8 = OpConstantComposite %_arr_uint_uint_1 %uint_2
          %9 = OpTypeTensorARM %float %uint_1 %8
%_ptr_UniformConstant_9 = OpTypePointer UniformConstant %9
         %11 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %13 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %15 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %39 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %40 = OpVariable %_ptr_UniformConstant_9 UniformConstant
         %41 = OpVariable %_ptr_UniformConstant_9 UniformConstant
     %uint_0 = OpConstant %uint 0
      %uchar = OpTypeInt 8 0
         %18 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %19 = OpTypeTensorARM %uchar %uint_1 %18
         %20 = OpConstantNull %19
    %float_2 = OpConstant %float 2
         %22 = OpConstantCompositeReplicateEXT %9 %float_2
         %25 = OpTypeTensorARM %float %uint_1 %18
         %26 = OpConstantNull %25
    %float_4 = OpConstant %float 4
         %24 = OpConstantComposite %9 %float_4 %float_2
         %42 = OpTypeGraphARM 3 %9 %9 %9 %9 %9 %9
               OpGraphEntryPointARM %43 "main" %11 %13 %15 %39 %40 %41
         %43 = OpGraphARM %42
         %12 = OpGraphInputARM %9 %uint_0
         %14 = OpGraphInputARM %9 %uint_1
         %16 = OpGraphInputARM %9 %uint_2
         %27 = OpExtInst %9 %1 MUL %12 %16 %20
         %28 = OpExtInst %9 %1 POW %14 %22
         %29 = OpExtInst %9 %1 MUL %12 %22 %20
         %30 = OpExtInst %9 %1 NEGATE %14 %26 %26
         %31 = OpExtInst %9 %1 MUL %27 %24 %20
         %32 = OpExtInst %9 %1 RECIPROCAL %29
         %33 = OpExtInst %9 %1 SUB %28 %31
         %34 = OpExtInst %9 %1 RSQRT %33
         %35 = OpExtInst %9 %1 SUB %30 %34
         %36 = OpExtInst %9 %1 ADD %30 %34
         %37 = OpExtInst %9 %1 MUL %35 %32 %20
         %38 = OpExtInst %9 %1 MUL %36 %32 %20
               OpGraphSetOutputARM %33 %uint_0
               OpGraphSetOutputARM %38 %uint_1
               OpGraphSetOutputARM %37 %uint_2
               OpGraphEndARM
)";
} // namespace spirvmodels