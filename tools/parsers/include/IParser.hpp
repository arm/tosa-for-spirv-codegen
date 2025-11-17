//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "ConstantData.hpp"

#include <TosaForSpirvCodegen.hpp>

#include <memory>
#include <vector>
#include <string>

namespace tfsc::parsers
{

/// Abstract IParser class to be subclassed by concrete parser classes.
class IParser
{
    public:
    virtual ~IParser() = default;

    /// GenerateSPIRVModule parses the given serialized tosa file and generates SPIR-V.
    /// @return A SPIR-V spirv Module
    virtual std::shared_ptr<spirv::Module> GenerateSPIRVModule(std::string graphName) = 0;

    /// Pure virtual method which parses the structure and generates SPIR-V.
    /// @return A SPIR-V binary vector.
    virtual std::vector<uint32_t> GenerateSPIRV(std::string graphName) = 0;

    /// Virtual method used to obtain weights, biases and other graph constants.
    /// @return A moved vector of ConstantData elements.
    std::vector<ConstantData> GetExternalConstants() { return std::move(m_ConstantData); };

    protected:
    /// Protected constructor, this abstract class should only be initialized by derived class constructors.
    IParser()
        : m_Version(TOSAVersion::v1_0)
    {
    }

    TOSAVersion m_Version;

    /// Map of GraphConstantId to Variant storing graph constant's data.
    std::vector<ConstantData> m_ConstantData;
};

} // namespace tfsc::parsers
