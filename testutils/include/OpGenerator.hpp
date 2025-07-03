//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Instruction.hpp>
#include <Module.hpp>
#include <Operand.hpp>
#include <OperatorDefinitions.hpp>
#include <OperatorEnum.hpp>
#include <Tensor.hpp>

#include <spirv/unified1/spirv.hpp>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

namespace testutils
{
using namespace tosa2spirv;

struct TosaOperator
{
    tosa::OperatorEnum op;
    std::vector<tosa::Tensor> inputs;
    std::vector<tosa::Tensor> outputs;
    std::vector<tosa::Attribute> attributes;
};

bool operator==(const TosaOperator& lhs, const TosaOperator& rhs);
bool operator!=(const TosaOperator& lhs, const TosaOperator& rhs);

using OperatorComparator = std::function<bool(const TosaOperator&, const TosaOperator&)>;
bool defaultOpComparator(const TosaOperator& lhs, const TosaOperator& rhs);

std::vector<std::string> Spirv2tosa(const std::shared_ptr<spirv::Module>& module, OperatorComparator filter);

std::vector<TosaOperator> Spirv2operators(const std::shared_ptr<spirv::Module>& module, OperatorComparator filter);

TosaOperator GetTosaOperator(const spirv::Instruction& instruction);

} // namespace testutils
