//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Tensor.hpp>

#include <string>
#include <utility>

namespace tosa2spirv
{

namespace spirvwriter
{
    /// Forward declare Writer
    class Writer;
}

namespace graphbuilder
{

/// Forward declare Module
class Module;

///    Graph constants are made available to ML programs in SPIR-V modules. Graph constants are declared in
///    SPIR-V module with a type and assigned an integer identifier that must be unique within the module. The initialiser for
///    graph constants is provided at graph pipeline creation time via a pointer to application-managed memory and related to a
///    specific graph constant using the integer identifier.

/// Class which encapsulates a Graph Constant which is constant data provided from outside the Module.
class GraphConstant
{
public:

    ~GraphConstant() = default;
    GraphConstant(const GraphConstant&) = default;
    uint32_t GetGraphConstantId() const { return m_Identifier; };
    uint32_t GetGraphConstantResId() const { return m_ResId; };

    std::optional<DataType> GetDataType() const { return m_DataType; };
    const std::optional<std::vector<unsigned int>>& GetTensorShape() const { return m_TensorShape; };

    void SetDataType(DataType dataType) { m_DataType = dataType; }
    void SetTensorShape(const std::vector<unsigned int>& shape) { m_TensorShape = shape; }
    const Tensor& GetTensor() const { return m_Tensor; }

    /// Make Module a friend to allow usage of private constructor
    friend Module;

    /// Make Writer a friend to allow usage of private SetGraphConstantResId
    friend spirvwriter::Writer;

private:
    GraphConstant(uint32_t identifier, std::string name, Tensor tensor)
            : m_Identifier(identifier)
            , m_Name(std::move(name))
            , m_Tensor(std::move(tensor))
    {}

    GraphConstant(uint32_t identifier, std::string  name, DataType type, std::vector<unsigned int> shape, Tensor tensor)
            : m_Identifier(identifier)
            , m_Name(std::move(name))
            , m_DataType(type)
            , m_TensorShape(shape)
            , m_Tensor(std::move(tensor))
    {}


    void SetGraphConstantResId(uint32_t resId) { m_ResId = resId; }

    uint32_t m_Identifier;
    std::string m_Name;
    uint32_t m_ResId = 0;
    Tensor m_Tensor;

    std::optional<DataType> m_DataType;
    std::optional<std::vector<unsigned int>> m_TensorShape;

};

} // graphbuilder namespace

} // tosa2spirv namespace