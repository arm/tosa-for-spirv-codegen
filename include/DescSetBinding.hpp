//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>
#include <utility>
#include <optional>
#include <vector>

namespace tosa2spirv
{

namespace graphbuilder
{

/// Forward declare Module
class Module;
enum class DataType;

/// Class which holds Descriptor Set and Binding values used by Module to communicate with pipeline
class DescSetBinding
{
public:

    ~DescSetBinding() = default;
    DescSetBinding(const tosa2spirv::graphbuilder::DescSetBinding&) = default;

    /// Method used to obtain the DescriptorSet value of the DescSetBinding.
    /// @return unsigned int representing the DescriptorSet.
    uint32_t GetDescriptorSet() const { return m_DescriptorSet; };

    /// Method used to obtain the Binding value of the DescSetBinding.
    /// @return unsigned int representing the Binding number.
    uint32_t GetBinding() const { return m_Binding; };

    /// Method used to obtain the Name of the DescSetBinding.
    /// @return std::string& to name of DescSetBinding.
    const std::string& GetName() const { return m_Name; };

    // Make Module a friend to allow usage of private constructor
    friend Module;

    uint32_t GetAssignedPointerId() const { return m_AssignedPointerId; }
    void SetAssignedPointerId(uint32_t id) { m_AssignedPointerId = id; }
    // Internal Getter and Setters used by ILayer

    std::optional<DataType> GetDataType() const { return m_DataType; };
    const std::optional<std::vector<unsigned int>>& GetTensorShape() const { return m_TensorShape; };

    void SetDataType(DataType dataType) { m_DataType = dataType; }
    void SetTensorShape(std::vector<unsigned int> shape) { m_TensorShape = shape; }

    uint32_t GetAssignedGraphInputId() const { return m_AssignedGraphInputId; }
    void SetAssignedGraphInputId(uint32_t id) { m_AssignedGraphInputId = id; }

private:
    DescSetBinding(uint32_t descSet, uint32_t binding, std::string name)
    : m_DescriptorSet(descSet)
    , m_Binding(binding)
    , m_Name(std::move(name)) {};

    uint32_t m_DescriptorSet;
    uint32_t m_Binding;
    std::string m_Name;
    std::optional<DataType> m_DataType;
    std::optional<std::vector<unsigned int>> m_TensorShape;

    uint32_t m_AssignedPointerId = 0U;
    uint32_t m_AssignedGraphInputId = 0U;
};

} // graphbuilder namespace

} // tosa2spirv namespace