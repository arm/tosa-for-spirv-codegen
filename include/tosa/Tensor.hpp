//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstdint>
#include <cstring>
#include <numeric>
#include <vector>

namespace tosa2spirv::spirv
{
class Instruction;
}

namespace tosa2spirv::tosa
{
/// Most state in tosa2spirv is represented as SPIRV instructions and operands
/// ResIds are pointers to the SPIRV instructions representing the inputs, outputs, operators and constants
using ResId = const spirv::Instruction*;

/// Enum class to encapsulate the different data types of Tensors.
enum class DataType
{
    int4_t = 0,
    int8_t = 1,
    int16_t = 2,
    int32_t = 3,
    uint8_t = 4,
    uint16_t = 5,
    uint32_t = 6,
    float16_t = 7,
    float32_t = 8,
    bfloat16_t = 9,
    bool_t = 10,
    null_t = 11,
};

/// tosa2spirv's implementation of TOSA tensor.
class Tensor
{
    public:
    using TensorShape = std::vector<unsigned int>;

    /// Default Constructor for Tensor
    Tensor() = default;

    /// Constructor for Tensor
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    Tensor(const DataType dataType, TensorShape shape)
        : m_Shape(std::move(shape))
        , m_DataType(dataType)
    {
        // Handle Rank 0 Tensors
        if (m_Shape.empty())
        {
            m_Shape = {1};
        }
    }

    /// Getter for DataType.
    /// @return DataType
    const DataType& GetDataType() const { return m_DataType; }

    /// Getter for TensorShape.
    /// @return TensorShape, which is a vector of unsigned integers
    const TensorShape& GetTensorShape() const { return m_Shape; }

    /// Getter for Tensor Elements
    /// @return unsigned int for number of elements in Tensor
    unsigned int GetNumElements() const
    {
        return std::accumulate(std::begin(m_Shape), std::end(m_Shape), 1, std::multiplies<>());
    }

    private:
    TensorShape m_Shape;
    DataType m_DataType{DataType::null_t};
};

template <typename T> DataType GetDataType();

/// Attribute is a small vector, which is stored locally within the spirv module, and a Tensor describing that vector
class Attribute
{
    public:
    template <typename T>
    explicit Attribute(const std::vector<T>& data,
                       const DataType dataType = GetDataType<T>(),
                       const Tensor::TensorShape& shape = {})
        : m_Tensor(dataType, shape.empty() ? Tensor::TensorShape{static_cast<unsigned>(data.size())} : shape)
    {
        m_AttributeData.reserve(data.size());
        for (const auto val : data)
        {
            // constexpr used to prevent compilation issues for floating point types and binary AND operator below.
            if constexpr (std::is_floating_point<T>::value)
            {
                uint32_t bits = 0;
                std::memcpy(&bits, &val, sizeof(float));
                m_AttributeData.emplace_back(bits);
            }
            else
            {
                switch (dataType)
                {
                    case DataType::int8_t: m_AttributeData.emplace_back(static_cast<uint32_t>(val & 0xFF)); break;
                    case DataType::int16_t: m_AttributeData.emplace_back(static_cast<uint32_t>(val & 0xFFFF)); break;
                    case DataType::int32_t: m_AttributeData.emplace_back(static_cast<uint32_t>(val)); break;
                    default: m_AttributeData.emplace_back(static_cast<uint32_t>(val));
                }
            }
        }
    }

    /// Helper ctor for single values
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    explicit Attribute(T data, const DataType dataType = GetDataType<T>())
        : Attribute(std::vector{data}, dataType, Tensor::TensorShape{1u})
    {
    }

    /// Helper ctor for initializer lists
    template <typename T>
    Attribute(std::initializer_list<T> data,
              const DataType dataType = GetDataType<T>(),
              const Tensor::TensorShape& shape = {})
        : Attribute(std::vector(data), dataType, shape)
    {
    }

    Tensor GetTensor() const { return m_Tensor; }

    const std::vector<uint32_t>& GetData() const { return m_AttributeData; }

    explicit Attribute(ResId resId)
        : m_ResId(std::move(resId)){};

    ResId GetResId() const { return m_ResId; }

    private:
    Tensor m_Tensor;
    std::vector<uint32_t> m_AttributeData;
    ResId m_ResId;
};

template <> inline DataType GetDataType<uint32_t>() { return DataType::uint32_t; }
template <> inline DataType GetDataType<int32_t>() { return DataType::int32_t; }
template <> inline DataType GetDataType<int16_t>() { return DataType::int16_t; };
template <> inline DataType GetDataType<int8_t>() { return DataType::int8_t; }
template <> inline DataType GetDataType<uint8_t>() { return DataType::uint8_t; };
template <> inline DataType GetDataType<float>() { return DataType::float32_t; }
template <> inline DataType GetDataType<bool>() { return DataType::bool_t; }

} // namespace tosa2spirv::tosa
