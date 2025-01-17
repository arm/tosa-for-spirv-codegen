//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <DescSetBinding.hpp>
#include <IdGenerator.hpp>
#include <IVisitor.hpp>
#include <GraphConstant.hpp>

#include <Version.hpp>
#include <deque>
#include <unordered_map>
#include <memory>
#include <vector>

namespace tosa2spirv::graphbuilder
{

/// Forward declaration of Context.
class Context;

/// Forward declaration of Graph.
class Graph;

/// Class which encapsulates tosa2spirv's internal representation of a SPIR-V Module.
class Module : public IVisitable
{
public:
    using ModulePtr = std::unique_ptr<Module, void(*)(Module* module)>;

    /// Creation method returning a raw pointer to a Module.
    /// @return Raw pointer to Module.
    static Module* CreateRaw();

    /// Creation method returning a unique pointer to a Module.
    /// @return Unique pointer to Module.
    static ModulePtr Create();

    /// Destructor method used to safely destroy a Module.
    /// @param[in] module Pointer to Module which delete will be called on.
    static void Destroy(Module* module);

    /// Write method used to write the internal representation of Module out as a vector of uint32_t words.
    /// @return A SPIR-V binary vector.
    std::vector<uint32_t> Write();

    /// Method used to add a Graph to the Module's vector of Graphs.
    /// @param[in] name std::string that will be used as the name of the graph.
    /// @return Shared pointer to the Graph that has been added, to be used to call AddXXXLayer().
    std::shared_ptr<Graph> AddGraph(const std::string& name = std::string());

    /// Method used to add a DescSetBinding to the Module's list of DescSetBinding objects.
    /// @param[in] descSet uint32_t DescSetBinding descriptor set.
    /// @param[in] binding uint32_t DescSetBinding binding.
    /// @param[in] name std::string DescSetBinding name.
    /// @return A reference to the Graph that has been added, to be used to call AddXXXLayer().
    DescSetBinding& AddDescSetBindingPair(uint32_t descSet,
                                          uint32_t binding,
                                          const std::string& name);

    /// Method used to retrieve the next available DescSetBinding binding.
    /// @param[in] descriptorSet uint32_t DescSetBinding descriptor set.
    /// @return unsigned int representing the next available DescSetBinding binding,
    /// i.e. max of value of bindings for that descriptor set incremented by 1.
    uint32_t GetNextAvailableBinding(uint32_t descriptorSet);

    /// Method used to retrieve a reference to a map of DescSetBinding objects.
    std::unordered_map<std::string, DescSetBinding>& GetDescSetBindings();

    /// Method used to retrieve a DescSetBinding object using a given name.
    /// @param[in] name std::string DescSetBinding name.
    /// @return A reference to the DescSetBinding object which has relevant name.
    DescSetBinding& GetDescSetBindingByName(const std::string& name);

    /// Method used to check if a DescSetBinding object has been set using a given name.
    /// @param[in] name std::string DescSetBinding name.
    /// @return bool representing whether the DescSetBinding object has been set or not.
    bool DescSetBindingExists(const std::string& name);

    /// Method used to retrieve the next available GraphConstant identifier.
    /// @return unsigned int representing the next available GraphConstant identifier.
    uint32_t GetNextAvailableGraphConstantId();

    /// Method used to add a GraphConstant to the Module's list of GraphConstant objects.
    /// @param[in] name std::string GraphConstant name.
    /// @param[in] addIdSuffix bool to specify if unique GraphConstant name is required.
    /// @return A reference to the GraphConstant object that has been added.
    GraphConstant& AddGraphConstant(const std::string& name, const Tensor& tensor);

    /// Method used to add a new GraphConstant to the Module..
    /// @param[in] identifier uint32_t GraphConstant identifier.
    /// @param[in] name std::string GraphConstant name.
    /// @return A reference to the GraphConstant object that has been added.
    GraphConstant& AddGraphConstant(uint32_t identifier, const Tensor& tensor, const std::string& name);

    /// Method used to retrieve a GraphConstant object using a given name.
    /// @param[in] name std::string used to identify a GraphConstant object.
    /// @return A reference to the GraphConstant object which has relevant name.
    GraphConstant& GetGraphConstantByName(const std::string& name);

    /// Method returns a constant reference to the modules GraphConstants.
    const std::deque<GraphConstant>& GetGraphConstants() const { return m_GraphConstants; };
    /// Method used to override the default TOSA Version of the Module.
    /// @param[in] TosaVersion enum that will set TOSA Version for the Module.
    void SetTargetTOSAVersion(TOSAVersion version);

    /// Method used to get TOSA SPIRV Version for Module.
    /// @return TosaVersion enum for the Module.
    TOSAVersion GetTargetTOSAVersion();

    /// Visitor method to allow the ResourceInfoVisitor to collect information on the Module
    void Accept(IVisitor& visitor) const override;

protected:
     /// Default Destructor.
     /// Marked Protected to prevent user deleting ptr.get() causing segfault when unique_ptr calls its destructor.
    ~Module() override = default;

private:
    /// Default constructor.
    /// Marked private as user should initialize a Module using Creation Methods Create() or CreateRaw().
    Module() = default;

    /// Member IdGenerator passed down to Write() function to give unique Ids for instructions within this Module.
    IdGenerator m_IdGenerator;

    std::shared_ptr<Context> m_Context;
    std::vector<std::shared_ptr<Graph>> m_Graphs;

    /// unordered_map used as it's safe for using references to items compared to vector that invalidates on resize
    /// also ensures uniqueness of string key
    std::unordered_map<std::string, DescSetBinding> m_DescSetBindings;

    /// deque used as it's safe for using references to items compared to vector that invalidates on resize
    /// but also quicker than list for insertions and provides subscript operator for GetDescSetBindings[idx]
    std::deque<GraphConstant> m_GraphConstants;
    /// Stores the Id for the next graph constant
    unsigned int m_NextConstantId = 0;
    /// Used to either provide next available GraphConstantId or existing associated Id if name already exists.
    bool CheckExistingGraphConstantId(const std::string& name);
};

} // namespace graphbuilder::tosa2spirv
