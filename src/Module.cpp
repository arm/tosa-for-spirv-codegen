//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Context.hpp>
#include <Operator.hpp>
#include <Graph.hpp>
#include <Module.hpp>
#include <Writer.hpp>
#include <algorithm>

namespace tosa2spirv::graphbuilder
{

Module* Module::CreateRaw()
{
    Module* module = new Module;
    module->m_Context = std::make_shared<Context>();
    return module;
}

Module::ModulePtr Module::Create()
{
    return Module::ModulePtr(CreateRaw(), Destroy);
}

void Module::Destroy(Module* module)
{
    delete module;
}

std::vector<uint32_t> Module::Write()
{
    // Set the IdGenerator to number of layer and graphs plus one otherwise if a user was to call Write()
    // multiple times the IDs would keep increasing, if we don't account for layers then their IDs would
    // overlap with those created during WriteExtInstSetImport.
    auto idGenerator = IdGenerator(m_IdGenerator);
    // Initialize the Spir-V Writer.
    auto writer = spirvwriter::Writer(idGenerator, m_Context);

    // Write the Module Header to Binary.
    writer.WriteHeader();

    writer.WriteCapabilities({spv::Capability::CapabilityVulkanMemoryModel,
                              spv::Capability::CapabilityShader,
                              spv::Capability::CapabilityInt8,
                              spv::Capability::CapabilityInt16,
                              spv::Capability::CapabilityFloat16,
                              spv::Capability::CapabilityGraphARM,
                              spv::Capability::CapabilityTensorsARM,
                              spv::Capability::CapabilityReplicatedCompositesEXT});


    // Write Extensions.
    writer.WriteExtension("SPV_ARM_graph");
    writer.WriteExtension("SPV_ARM_tensors");
    writer.WriteExtension("SPV_EXT_replicated_composites");

    // Extended instruction set import.
    auto tosaId = writer.WriteExtInstSetImport("TOSA.000080.2");

    // Write Memory Model
    writer.WriteMemoryModel();

    // Save the position for WriteDecoration to use later.
    writer.SaveDecorationPosition();

    for (auto& constant : m_GraphConstants)
    {
        writer.WriteGraphConstantInstruction(constant, spirvwriter::array);
    }
    // Write each Graph to Binary.
    for (const auto& graph : m_Graphs)
    {
        graph->Write(writer, tosaId.Get());
    }

    // Once we're finished writing all instructions we can then set the bounds.
    writer.WriteBounds(idGenerator.GetNextId().Get());

    // Reset state of Descriptor Set Bindings to ensure they are assigned correctly on re-write.
    for (auto& descSetBinding : m_DescSetBindings)
    {
        descSetBinding.second.SetAssignedPointerId(0);
        descSetBinding.second.SetAssignedGraphInputId(0);
    }

    // Reset state of GraphConstant ResId to ensure they are assigned correctly on re-write.
    for (auto& graphConstantResId : m_GraphConstants)
    {
        graphConstantResId.SetGraphConstantResId(0);
    }

    // Finally, we can return the Binary.
    return writer.GetBinary();
}

std::shared_ptr<Graph> Module::AddGraph(const std::string& name)
{
    // Using new as shared pointer can't use private Graph() constructor in std::make_shared<Graph>().
    std::shared_ptr<Graph> ptr(new Graph(m_IdGenerator, name));

    // Add graph pointer to Modules graphs.
    m_Graphs.push_back(ptr);

    // Return the pointer to graph for the user to then make calls to AddXXXLayer.
    return ptr;
}

void Module::SetTargetTOSAVersion(TOSAVersion version)
{
    m_Context->SetTOSAVersion(version);
}

TOSAVersion Module::GetTargetTOSAVersion()
{
    return m_Context->GetTOSAVersion();
}

DescSetBinding& Module::AddDescSetBindingPair(uint32_t descSet,
                                              uint32_t binding,
                                              const std::string& name)
{
    // Check for duplicates here
    if (m_DescSetBindings.find(name) != m_DescSetBindings.end())
    {
        throw std::invalid_argument("Error: Attempted to add a duplicate DescSetBinding; name must be unique.");
    }

    DescSetBinding descSetBinding(descSet, binding, name);
    m_DescSetBindings.insert(std::make_pair(name, descSetBinding));

    return m_DescSetBindings.find(name)->second;
}

uint32_t Module::GetNextAvailableBinding(uint32_t descriptorSet)
{
    std::vector<uint32_t> bindingIds;
    bindingIds.reserve(m_DescSetBindings.size());

    // For each binding pair, push back binding number to vector.
    for (const auto& bindingPair : m_DescSetBindings)
    {
        if (bindingPair.second.GetDescriptorSet() == descriptorSet)
        {
            bindingIds.emplace_back(bindingPair.second.GetBinding());
        }
    }

    if (bindingIds.empty())
    {
       return 0;
    }

    // Find the max number in the vector.
    auto max = *std::max_element(bindingIds.begin(), bindingIds.end());
    return max+1;
}

std::unordered_map<std::string, DescSetBinding>& Module::GetDescSetBindings() { return m_DescSetBindings; }

DescSetBinding& Module::GetDescSetBindingByName(const std::string& name)
{
    if (m_DescSetBindings.find(name) == m_DescSetBindings.end())
    {
        throw std::invalid_argument("DescSetBinding with provided name not found.");
    }
    return m_DescSetBindings.find(name)->second;
}

bool Module::DescSetBindingExists(const std::string& name)
{
    if (m_DescSetBindings.find(name) == m_DescSetBindings.end())
    {
        return false;
    }
    return true;
}

GraphConstant& Module::AddGraphConstant(const std::string& name, const Tensor& tensor)
{
    GraphConstant graphConstant(m_NextConstantId++, name, tensor);
    graphConstant.SetGraphConstantResId(tensor.GetResId().Get());
    m_GraphConstants.push_back(graphConstant);
    return m_GraphConstants.back();
}

GraphConstant& Module::AddGraphConstant(uint32_t identifier, const Tensor& tensor, const std::string& name)
{
    if (CheckExistingGraphConstantId(name))
    {
        throw std::invalid_argument("Error: Attempted to add a duplicate GraphConstant; "
                                    "name must be unique.");
    }

    GraphConstant graphConstant(identifier, name, tensor);
    m_GraphConstants.push_back(graphConstant);
    return m_GraphConstants.back();
}

bool Module::CheckExistingGraphConstantId(const std::string& name)
{
    return (std::find_if(m_GraphConstants.begin(), m_GraphConstants.end(),
                         [&](const auto& element)
                         { return element.m_Name == name; })!= m_GraphConstants.end());
}

GraphConstant& Module::GetGraphConstantByName(const std::string& name)
{
    for (auto& element : m_GraphConstants)
    {
        if (element.m_Name == name)
        {
            return element;
        }
    }
    throw std::invalid_argument("GraphConstant with provided name not found.");
}

void Module::Accept(IVisitor& visitor) const
{
    for (const auto& graph: m_Graphs)
    {
        graph->Accept(visitor);
    }
    visitor.Visit(*this);
}

} // namespace graphbuilder::tosa2spirv