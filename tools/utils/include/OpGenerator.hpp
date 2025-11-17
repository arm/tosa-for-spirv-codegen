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
#include <string>

namespace testutils
{
using namespace tfsc;

/// TOSA Operator Definition structure, collection of data describing a single TOSA Operator
struct TosaOperator
{
    /// The type of operator
    tosa::OperatorEnum op;
    /// The regular, dynamic tensor inputs
    std::vector<tosa::Tensor> inputs;
    /// Graph constant tensor inputs
    std::vector<tosa::Tensor> graphConstants;
    /// Tensor constant inputs
    std::vector<tosa::Attribute> tensorConstants;
    /// Regular tensor outputs
    std::vector<tosa::Tensor> outputs;
    /// Additional attributes for the given operator
    std::vector<tosa::Attribute> attributes;
};

bool operator==(const TosaOperator& lhs, const TosaOperator& rhs);
bool operator!=(const TosaOperator& lhs, const TosaOperator& rhs);

/// Comparison function type compatible with std::less for TosaOperator instances, to make TosaOperators usable
/// with an std::set data structure
using OperatorComparator = std::function<bool(const TosaOperator&, const TosaOperator&)>;
bool defaultOpComparator(const TosaOperator& lhs, const TosaOperator& rhs);

/// Generates an std::vector of C/C++ strings describing all operators found in the given Module instance, using
/// parameters that the TosaOperator structure consists of
std::vector<std::string> Spirv2tosa(const std::shared_ptr<spirv::Module>& module, OperatorComparator filter);

/// Generates an std::vector of TosaOperator instances describing all operators found in the given Module instance
std::vector<TosaOperator> Spirv2operators(const std::shared_ptr<spirv::Module>& module, OperatorComparator filter);

/// Converts a single TosaOperator instance to a C/C++ compatible string definition of the operator
std::string OperatorToString(const TosaOperator& op);

/// Converts a single TosaOperator instance to a C/C++ compatible string adding the operator to a graph
/// If inputsName, tensorConstsName, graphConstsName, outputsName or attributesName specified, the tensors/attributes
/// are taken from vectors with the corresponding names, otherwise new instances are made
std::string OperatorToGraphDefinition(const TosaOperator& op,
                                      std::string graphVarName = "graph",
                                      std::string inputsName = "",
                                      std::string graphConstsName = "",
                                      std::string tensorConstsName = "",
                                      std::string outputsName = "",
                                      std::string attributesName = "");

/// Converts an OpExtInst Instruction instance to a TosaOperator instance, describing the operator found in the given
/// Instruction
TosaOperator GetTosaOperator(const spirv::Instruction& instruction);

} // namespace testutils
