//
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <tosa2spirv.hpp>

#include <cstdint>
#include <fstream>
#include <memory>
#include <variant>

#include <map>
#include <vector>

// Forward declare tosa::TosaSerializationTensor to remove dependency from IParser
namespace tosa
{
    class TosaSerializationTensor;
}

namespace tosa2spirv
{

namespace parsers
{


using GraphConstant = std::variant<tosa::TosaSerializationTensor*,
                      std::vector<int8_t>,
                      std::vector<int16_t>,
                      std::vector<int32_t>,
                      std::vector<float>>;

/// Abstract IParser class to be subclassed by concrete parser classes.
class IParser : graphbuilder::IVisitable
{
public:
    /// Pure virtual method which parses the structure and generates SPIR-V.
    /// @return A SPIR-V binary vector.
    virtual std::vector<uint32_t> GenerateSPIRV(std::string graphName) = 0;

    /// Accept function to allow for getting graph / layer information via a Visitor
    /// @param[in] visitor The visitor which will hold the graph information
    void Accept(graphbuilder::IVisitor& visitor) const override
    {
        m_Module->Accept(visitor);
    }

    /// Virtual method used to obtain weights, biases and other graph constants.
    /// @return vector<tosa::TosaSerializationTensor*> for all operators in TOSA block.
    virtual std::map<uint32_t, GraphConstant> GetGraphConstants() { return m_GraphConstants; };

protected:
    /// Protected constructor, this abstract class should only be initialized by derived class constructors.
    IParser()
        : m_Module(graphbuilder::Module::Create())
    {}

    /// Adds a single graph to the Module.
    std::shared_ptr<graphbuilder::Graph> AddGraph(const std::string& name) { return m_Module->AddGraph(name); };

    /// Pointer to Module, simplifying the interface.
    graphbuilder::Module::ModulePtr m_Module;

    /// Map of GraphConstantId to Variant storing graph constant's data.
    std::map<uint32_t, GraphConstant> m_GraphConstants;
};

} // namespace parsers

} // namespace tosa2spirv