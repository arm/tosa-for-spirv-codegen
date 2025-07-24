//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <AssemblyUtils.hpp>
#include <Graph.hpp>
#include <OpGenerator.hpp>
#include <Operand.hpp>
#include <OperatorDefinitions.hpp>
#include <Tensor.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>

#include <vector>

using tosa2spirv::tosa::DataType;

namespace testutils
{

/// Checks that a given spirv Module instance contains an operator with the given parameters
bool CheckModule(const std::shared_ptr<spirv::Module>& module,
                 const TOSAInstructions op,
                 const std::vector<tosa::Tensor>& inputs,
                 const std::vector<tosa::Tensor>& graphConstants,
                 const std::vector<tosa::Attribute>& tensorConstants,
                 const std::vector<tosa::Tensor>& outputs,
                 const std::vector<tosa::Attribute>& attributes = {});

/// CheckModule overload taking an OperatorEnum instead of TOSAInstructions operator
inline bool CheckModule(const std::shared_ptr<spirv::Module>& module,
                        const tosa::OperatorEnum op,
                        const std::vector<tosa::Tensor>& inputs,
                        const std::vector<tosa::Tensor>& graphConstants,
                        const std::vector<tosa::Attribute>& tensorConstants,
                        const std::vector<tosa::Tensor>& outputs,
                        const std::vector<tosa::Attribute>& attributes = {})
{
    return CheckModule(module,
                       tosa::GetTOSAInstructions(op),
                       inputs,
                       graphConstants,
                       tensorConstants,
                       outputs,
                       attributes);
}

/// Operator Test Fixture, attempts to create a new operator using the GraphAPI defined using the given parameters, and
/// verifies that the produced SPIR-V contains an equivalent operator definition
bool CheckOperator(const tosa::OperatorEnum op,
                   const std::vector<tosa::Tensor>& inputs,
                   const std::vector<tosa::Tensor>& graphConstants,
                   const std::vector<tosa::Attribute>& tensorConstants,
                   const std::vector<tosa::Tensor>& outputs,
                   const std::vector<tosa::Attribute>& attributes);

} // namespace testutils
