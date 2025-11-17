//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once
#include <string>

namespace spirvmodels
{
static const std::string SPIRV_HEADER = "               OpCapability Shader\n"
                                        "               OpCapability Float16\n"
                                        "               OpCapability Int64\n"
                                        "               OpCapability Int16\n"
                                        "               OpCapability Int8\n"
                                        "               OpCapability TensorsARM\n"
                                        "               OpCapability GraphARM\n"
                                        "               OpCapability VulkanMemoryModel\n"
                                        "               OpCapability ReplicatedCompositesEXT\n"
                                        "               OpExtension \"SPV_ARM_graph\"\n"
                                        "               OpExtension \"SPV_ARM_tensors\"\n"
                                        "               OpExtension \"SPV_EXT_replicated_composites\"\n"
                                        "          %1 = OpExtInstImport \"TOSA.001000.1\"\n"
                                        "               OpMemoryModel Logical Vulkan\n";
}
