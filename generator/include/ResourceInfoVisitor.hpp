//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <IVisitor.hpp>
#include <ResourceInfo.hpp>
#include <stdexcept>


namespace tosa2spirv
{

namespace generator
{

using VulkanFormat = std::string;

/// Helper function to get VulkanFormat from Tensor DataType.
static VulkanFormat GetVulkanFormatFromDataType(const graphbuilder::DataType& dataType)
{
    using namespace graphbuilder;
    switch (dataType)
    {
        case DataType::bool_t:
            return "VK_FORMAT_R8_BOOL_ARM";
        case DataType::int8_t:
            return "VK_FORMAT_R8_SINT";
        case DataType::int16_t:
            return "VK_FORMAT_R16_SINT";
        case DataType::int32_t:
            return "VK_FORMAT_R32_SINT";
        case DataType::uint8_t:
            return "VK_FORMAT_R8_UINT";
        case DataType::uint16_t:
            return "VK_FORMAT_R16_UINT";
        case DataType::uint32_t:
            return "VK_FORMAT_R32_UINT";
        case DataType::float16_t:
            return "VK_FORMAT_R16_SFLOAT";
        case DataType::float32_t:
            return "VK_FORMAT_R32_SFLOAT";
        default:
            throw std::runtime_error("GetVulkanFormatFromDataType: Unsupported DataType.");
    }
}

/// Visitor class used to visit the Graph or Modules in order to gather Resource Information
class ResourceInfoVisitor : public graphbuilder::IVisitor
{
public:
    ResourceInfoVisitor() = default;;

    ResourceInfo& GetResourceInfo() { return m_ResourceInfo; };
    ~ResourceInfoVisitor();

private:
    ResourceInfo m_ResourceInfo;

public:
    void Visit(const graphbuilder::Graph& graph) override;
    void Visit(const graphbuilder::Module& module) override;
};

} // tosa2spirv::generator

} // tosa2spirv