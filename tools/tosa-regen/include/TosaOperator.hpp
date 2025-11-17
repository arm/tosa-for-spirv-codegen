//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <OperatorDefinitions.hpp>
#include <OperatorEnum.hpp>
#include <Tensor.hpp>

#include <functional>

namespace tosaregen
{

/// The type of an input tensor to a TosaOperator structure
enum class TosaInputType
{
    /// Dynamic input tensor - values provided by user
    Dynamic,
    /// Constant tensor with externally supplied constant data (uniform)
    GraphConstant,
    /// Constant tensor with constant data directly in it
    TensorConstant,
};

/// Structure defining an input to a TosaOperator structure
struct TosaInput
{
    /// Tensor (shape/type) and optionally data (if TensorConstant type)
    tfsc::tosa::Attribute data;
    /// The type of the input tensor
    TosaInputType type;
    /// Tensor unique identifier number/name
    uint32_t id;
    /// Tensor subidentifier for tensors with the same ID, used in multiple output operators
    uint8_t idx;
    /// Tensor input binding ID in the graph if any exists
    uint32_t bindingId;

    /// Utility constructor with default values, allowing users to construct an instance with just an attribute
    TosaInput(tfsc::tosa::Attribute attribute,
              TosaInputType type = TosaInputType::TensorConstant,
              uint32_t id = 0,
              uint8_t idx = 0,
              uint32_t bindingId = 0)
        : data(attribute)
        , type(type)
        , id(id)
        , idx(idx)
        , bindingId(bindingId) {};

    /// Utility constructor with default values, allowing users to construct an instance out of a
    /// tfsc::tosa::Tensor
    TosaInput(tfsc::tosa::Tensor tensor,
              TosaInputType type = TosaInputType::Dynamic,
              uint32_t id = 0,
              uint8_t idx = 0,
              uint32_t bindingId = 0)
        : data(std::initializer_list<uint32_t>{}, tensor.GetDataType(), tensor.GetTensorShape())
        , type(type)
        , id(id)
        , idx(idx)
        , bindingId(bindingId) {};

    tfsc::tosa::Tensor GetTensor() const { return data.GetTensor(); };
    tfsc::tosa::Attribute GetAttribute() const { return data; };
};

/// Structure defining an output from a TosaOperator structure
struct TosaOutput
{
    /// Tensor shape/type of the output
    tfsc::tosa::Tensor tensor;
    /// Output unique identifier number
    uint32_t id;
    /// Output subidentifier for tensors with the same ID, used in multiple output operators
    uint8_t idx;

    TosaOutput(tfsc::tosa::Tensor tensor, uint32_t id = 0, uint8_t idx = 0)
        : tensor(tensor)
        , id(id)
        , idx(idx) {};

    tfsc::tosa::Tensor GetTensor() const { return tensor; };
};

/// TOSA Operator Definition structure, collection of data describing a single TOSA Operator
struct TosaOperator
{
    /// The type of operator
    tfsc::tosa::OperatorEnum op;
    /// Input tensors
    std::vector<TosaInput> inputs;
    /// Output tensors
    std::vector<TosaOutput> outputs;
    /// Additional attributes for the given operator
    std::vector<tfsc::tosa::Attribute> attributes;
};

bool operator==(const TosaOperator& lhs, const TosaOperator& rhs);
bool operator!=(const TosaOperator& lhs, const TosaOperator& rhs);

/// Compares TosaOperator data, ignoring the id and idx loaded from SPIR-V ResIds
bool CompareTosaOperatorData(const TosaOperator& lhs, const TosaOperator& rhs);

bool operator<(const TosaInput& lhs, const TosaInput& rhs);
bool operator<=(const TosaInput& lhs, const TosaInput& rhs);
bool operator>(const TosaInput& lhs, const TosaInput& rhs);
bool operator>=(const TosaInput& lhs, const TosaInput& rhs);
bool operator==(const TosaInput& lhs, const TosaInput& rhs);
bool operator!=(const TosaInput& lhs, const TosaInput& rhs);

bool operator<(const TosaOutput& lhs, const TosaOutput& rhs);
bool operator<=(const TosaOutput& lhs, const TosaOutput& rhs);
bool operator>(const TosaOutput& lhs, const TosaOutput& rhs);
bool operator>=(const TosaOutput& lhs, const TosaOutput& rhs);
bool operator==(const TosaOutput& lhs, const TosaOutput& rhs);
bool operator!=(const TosaOutput& lhs, const TosaOutput& rhs);

/// Comparison function type compatible with std::less for TosaOperator instances, to make TosaOperators usable
/// with an std::set data structure
using OperatorComparator = std::function<bool(const TosaOperator&, const TosaOperator&)>;
bool defaultOpComparator(const TosaOperator& lhs, const TosaOperator& rhs);

/// Converts a single TosaOperator instance to a C/C++ compatible string definition of the operator
std::string OperatorToString(const TosaOperator& op);

/// Converts a single TosaOperator instance to a C/C++ compatible string adding the operator to a graph
/// If inputsName, tensorConstsName, graphConstsName, outputsName or attributesName specified, the tensors/attributes
/// are taken from vectors with the corresponding names, otherwise new instances are made
std::string OperatorToGraphDefinition(const TosaOperator& op,
                                      std::string graphVarName = "graph",
                                      std::string inputsName = "",
                                      std::string outputsName = "",
                                      std::string attributesName = "");

} // namespace tosaregen
