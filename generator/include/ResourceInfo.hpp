//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Tensor.hpp>
#include <cstdint>
#include <vector>
#include <map>

namespace tosa2spirv
{

namespace generator
{

/// Struct used to encapsulate the graph constants of a module.
struct Constant
{
    bool operator==(const Constant& other) const
    {
        return m_Format == other.m_Format &&
               m_Shape == other.m_Shape;
    }
    std::string m_Format = std::string();
    std::vector<unsigned int> m_Shape = {};
};

/// Alias used to encapsulate the intermediate values of the graph.
using Intermediate = Constant;

/// Struct used to encapsulate the inputs and outputs information of the graph.
struct IOInfo
{
    bool operator==(const IOInfo& other) const
    {
        return m_BindingId == other.m_BindingId &&
               m_DescriptorSet == other.m_DescriptorSet &&
               m_Format == other.m_Format &&
               m_IOIndex == other.m_IOIndex &&
               m_Layout == other.m_Layout &&
               m_Name == other.m_Name &&
               m_ResourceType == other.m_ResourceType &&
               m_Shape == other.m_Shape;
    }
    int32_t m_BindingId = -1;
    int32_t m_DescriptorSet = -1;
    std::string m_Format = std::string();
    int32_t m_IOIndex = -1;
    std::string m_Layout = std::string();
    std::string m_Name = std::string();
    std::string m_ResourceType = std::string();
    std::vector<unsigned int> m_Shape = {};
};

/// Class which encapsulates resource info information of the module for use by Vulkan ML API.
class ResourceInfo
{
public:

    /// Method used to add an entry for a constant.
    void AddConstant(const std::string& format = std::string(),
                     uint32_t id = -1,
                     const std::vector<unsigned int>& shape = {})
    {
        m_Constants[id] = {format,
                           shape};
    }

    /// Method used to add an entry for an input.
    void AddInput(int32_t bindingId = -1,
                  int32_t descriptorSet = -1,
                  const std::string&  format = std::string(),
                  int32_t inputIndex = -1,
                  const std::string& layout = std::string(),
                  const std::string& name = std::string(),
                  const std::string& resourceType = std::string(),
                  const graphbuilder::Tensor::TensorShape& shape = {})
    {
        m_Inputs.push_back({bindingId,
                            descriptorSet,
                            format,
                            inputIndex,
                            layout,
                            name,
                            resourceType,
                            shape});
    }

    /// Method used to add an entry for an intermediate value.
    void AddIntermediate(const std::string& format = std::string(),
                         uint32_t id = -1,
                         const std::vector<unsigned int>& shape = {})
    {
        m_Intermediates[id] = {format,
                               shape};
    }

    /// Method used to add an entry for an output.
    void AddOutput(int32_t bindingId = -1,
                   int32_t descriptorSet = -1,
                   const std::string& format = std::string(),
                   int32_t outputIndex = -1,
                   const std::string& layout = std::string(),
                   const std::string& name = std::string(),
                   const std::string& resourceType = std::string(),
                   const graphbuilder::Tensor::TensorShape& shape = {})
    {
        m_Outputs.push_back({bindingId, descriptorSet, format, outputIndex, layout, name, resourceType, shape});
    }

    /// Method used to return the vector of constants
    std::map<uint32_t, Constant>& GetConstants() { return m_Constants; };
    /// Method used to return the map of input entries.
    std::vector<IOInfo>& GetInputs() { return m_Inputs; };
    /// Method used to return the map of intermediate entries.
    std::map<uint32_t, Intermediate>& GetIntermediates() { return m_Intermediates; };
    /// Method used to return the vector of output entries.
    std::vector<IOInfo>& GetOutputs() { return m_Outputs; };

    bool operator==(const ResourceInfo& other) const
    {
        return m_Constants == other.m_Constants &&
               m_Inputs == other.m_Inputs &&
               m_Intermediates == other.m_Intermediates &&
               m_Outputs == other.m_Outputs;
    }

private:
    std::map<uint32_t, Constant> m_Constants;
    std::vector<IOInfo> m_Inputs;
    std::map<uint32_t, Intermediate> m_Intermediates;
    std::vector<IOInfo> m_Outputs;
};

} // graphbuilder namespace

} // tosa2spirv namespace