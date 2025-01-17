//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Writer.hpp>

#include <spirv/unified1/spirv.hpp>
#include <spirv-tools/libspirv.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace tosa2spirv
{

void WriteBoilerplateExtended(spirvwriter::Writer& writer,
                              std::vector<spv::Capability>* capabilityTypeList)
{
    writer.WriteHeader();

    if (capabilityTypeList)
    {
        std::vector<spv::Capability> capabilities = {spv::Capability::CapabilityVulkanMemoryModel,
                                                     spv::Capability::CapabilityShader};
        capabilities.insert(capabilities.end(), capabilityTypeList->begin(), capabilityTypeList->end());
        capabilities.insert(capabilities.end(), {spv::Capability::CapabilityGraphARM,
                                                 spv::Capability::CapabilityTensorsARM});

        writer.WriteCapabilities(capabilities);
    }
    else
    {
        writer.WriteCapabilities({spv::Capability::CapabilityVulkanMemoryModel,
                                   spv::Capability::CapabilityShader,
                                   spv::Capability::CapabilityInt8,
                                   spv::Capability::CapabilityFloat16,
                                   spv::Capability::CapabilityGraphARM,
                                   spv::Capability::CapabilityTensorsARM});
    }

    writer.WriteExtension("SPV_ARM_graph");
    writer.WriteExtension("SPV_ARM_tensors");
}

void WriteBoilerplate(spirvwriter::Writer& writer)
{
    std::vector<spv::Capability> capabilityTypeList{spv::Capability::CapabilityInt8,
                                                    spv::Capability::CapabilityInt16,
                                                    spv::Capability::CapabilityFloat16};
    WriteBoilerplateExtended(writer, &capabilityTypeList);
}

std::string GetPreSPIRVStr()
{
    std::string preSPIRV = "               OpCapability VulkanMemoryModel\n"
                           "               OpCapability Shader\n"
                           "               OpCapability Int8\n"
                           "               OpCapability Int16\n"
                           "               OpCapability Float16\n"
                           "               OpCapability GraphARM\n"
                           "               OpCapability TensorsARM\n"
                           "               OpCapability ReplicatedCompositesEXT\n"
                           "               OpExtension \"SPV_ARM_graph\"\n"
                           "               OpExtension \"SPV_ARM_tensors\"\n"
                           "               OpExtension \"SPV_EXT_replicated_composites\"\n";
    return preSPIRV;
}

namespace spirvwriter
{

std::string DisassembleSPIRV(const std::vector<uint32_t>& binary, bool runValidation = true)
{
    spvtools::SpirvTools tools{ SPV_ENV_UNIVERSAL_1_6 };

    if (!tools.IsValid())
    {
        std::cerr << "DisassembleSPIRV(): Failed to validate SPIR-V tools." << std::endl;
        return {};
    }

    std::string humanReadableSpirV;

    static const spvtools::MessageConsumer messageConsumer =
        [](spv_message_level_t level, const char*, const spv_position_t& pos, const char* message)
        {
            std::string levelStr;
            switch (level)
            {
                case SPV_MSG_FATAL:
                    levelStr = "FATAL";
                    break;
                case SPV_MSG_INTERNAL_ERROR:
                    levelStr = "INTERNAL ERROR";
                    break;
                case SPV_MSG_ERROR:
                    levelStr = "ERROR";
                    break;
                case SPV_MSG_WARNING:
                    levelStr = "WARNING";
                    break;
                case SPV_MSG_INFO:
                    levelStr = "INFO";
                    break;
                case SPV_MSG_DEBUG:
                    levelStr = "DEBUG";
                    break;
            }
            std::cout << "spvtools reported " << message << "(" << levelStr.c_str() << ") at position: "
                << pos.index << std::endl;
        };
    tools.SetMessageConsumer(messageConsumer);

    if(runValidation)
    {
        if (!tools.Validate(binary))
        {
            std::cerr << "DisassembleSPIRV():Failed to validate SPIR-V program." << std::endl;
            return {};
        }
    }

    // Return SPIR-V in human-readable form with indentation and friendly names for types.
    if (!tools.Disassemble(binary,
                           &humanReadableSpirV,
                           SPV_BINARY_TO_TEXT_OPTION_INDENT |
                           SPV_BINARY_TO_TEXT_OPTION_NO_HEADER |
                           SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES))
    {
        // Exception
        std::cerr << "DisassembleSPIRV():Failed to disassemble SPIR-V program." << std::endl;
        return {};
    }

    return humanReadableSpirV;
}

} // spirvwriter namespace

} // tosa2spirv namespace