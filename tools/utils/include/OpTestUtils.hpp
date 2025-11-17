//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <AssemblyUtils.hpp>
#include <Graph.hpp>
#include <Operand.hpp>
#include <OperatorDefinitions.hpp>
#include <Tensor.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <gtest/gtest.h>

#include <vector>

using tfsc::tosa::DataType;

namespace testutils
{

/// Checks that a given spirv Module instance contains an operator with the given parameters
void CheckModule(const std::shared_ptr<tfsc::spirv::Module>& module,
                 const TOSAInstructions op,
                 const std::vector<tfsc::tosa::Attribute>& inputs,
                 const std::vector<tfsc::tosa::Tensor>& outputs,
                 const std::vector<tfsc::tosa::Attribute>& attributes = {});

/// CheckModule overload taking an OperatorEnum instead of TOSAInstructions operator
inline void CheckModule(const std::shared_ptr<tfsc::spirv::Module>& module,
                        const tfsc::tosa::OperatorEnum op,
                        const std::vector<tfsc::tosa::Attribute>& inputs,
                        const std::vector<tfsc::tosa::Tensor>& outputs,
                        const std::vector<tfsc::tosa::Attribute>& attributes = {})
{
    CheckModule(module, tfsc::tosa::GetTOSAInstructions(op), inputs, outputs, attributes);
}

} // namespace testutils
