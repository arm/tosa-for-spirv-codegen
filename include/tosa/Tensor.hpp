//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstdint>
#include <cstring>
#include <numeric>
#include <string>
#include <vector>

namespace tfsc::spirv
{
class Instruction;
}

namespace tfsc::tosa
{
/// Most state in tosa_for_spirv_codegen is represented as SPIRV instructions and operands
/// ResIds are pointers to the SPIRV instructions representing the inputs, outputs, operators and constants
using ResId = const spirv::Instruction*;

/// Enum class to encapsulate the different data types of Tensors.
enum class DataType
{
    int4_t = 0,
    int8_t = 1,
    int16_t = 2,
    int32_t = 3,
    int48_t = 4,
    int64_t = 5,
    uint8_t = 6,
    uint16_t = 7,
    uint32_t = 8,
    float16_t = 9,
    float32_t = 10,
    bfloat16_t = 11,
    bool_t = 12,
    null_t = 13,
};

inline std::string GetDataTypeName(DataType type)
{
    const char* names[] = {
        "int4_t",
        "int8_t",
        "int16_t",
        "int32_t",
        "int48_t",
        "int64_t",
        "uint8_t",
        "uint16_t",
        "uint32_t",
        "float16_t",
        "float32_t",
        "bfloat16_t",
        "bool_t",
        "null_t",
    };
    return names[static_cast<size_t>(type)];
}

/// tosa_for_spirv_codegen's implementation of TOSA tensor.
class Tensor
{
    public:
    using TensorShape = std::vector<uint32_t>;

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

    bool operator==(const Tensor& other) const
    {
        if (this->m_DataType != other.m_DataType)
            return false;
        if (this->m_Shape.size() != other.m_Shape.size())
            return false;
        for (size_t dimIdx = 0; dimIdx < this->m_Shape.size(); ++dimIdx)
        {
            if (this->m_Shape[dimIdx] != other.m_Shape[dimIdx])
                return false;
        }
        return true;
    }

    bool operator!=(const Tensor& other) const { return !(*this == other); }

    private:
    TensorShape m_Shape;
    DataType m_DataType{DataType::null_t};
};

template <typename T> DataType GetDataType();

template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
void CastAndEmplace(T val, DataType, std::vector<uint32_t>& attributeData)
{
    attributeData.emplace_back(*reinterpret_cast<uint32_t*>(&val));
}

template <typename T, std::enable_if_t<(std::is_integral_v<T> && sizeof(T) == 8), bool> = true>
void CastAndEmplace(const T val, DataType, std::vector<uint32_t>& attributeData)
{
    attributeData.emplace_back(static_cast<uint32_t>(val));
    attributeData.emplace_back(static_cast<uint32_t>(val >> 32));
}

template <typename T, std::enable_if_t<(std::is_integral_v<T> && sizeof(T) <= 4), bool> = true>
void CastAndEmplace(const T val, const DataType dataType, std::vector<uint32_t>& attributeData)
{
    switch (dataType)
    {
        // Assume int4 nibbles are un packed, i.e. one int4 per T Val
        case DataType::int4_t:
            attributeData.emplace_back(static_cast<uint32_t>((val & 0x8 ? val | 0xF0 : val) & 0xFF));
            break;
        case DataType::int8_t: attributeData.emplace_back(static_cast<uint32_t>(val & 0xFF)); break;
        case DataType::int16_t: attributeData.emplace_back(static_cast<uint32_t>(val & 0xFFFF)); break;
        case DataType::int48_t:
        {
            const auto uval = static_cast<uint64_t>(val);
            attributeData.emplace_back(static_cast<uint32_t>(uval));
            attributeData.emplace_back(static_cast<uint32_t>(0));
            break;
        }
        default: attributeData.emplace_back(static_cast<uint32_t>(val));
    }
}

/// Attribute is a small vector, which is stored locally within the spirv module, and a Tensor describing that vector
class Attribute
{
    public:
    template <typename T, const DataType DT = GetDataType<T>()>
    explicit Attribute(const std::vector<T>& data, const DataType dataType = DT, const Tensor::TensorShape& shape = {})
        : m_Tensor(dataType, shape.empty() ? Tensor::TensorShape{static_cast<unsigned>(data.size())} : shape)
    {
        m_AttributeData.reserve(data.size());
        for (const auto val : data)
        {
            CastAndEmplace<T>(val, dataType, m_AttributeData);
        }
    }

    /// Helper ctor for single values
    template <typename T, const DataType DT = GetDataType<T>(), std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    explicit Attribute(T data, const DataType dataType = DT)
        : Attribute(std::vector{data}, dataType, Tensor::TensorShape{1u})
    {
    }

    /// Helper ctor for initializer lists
    template <typename T,
              const DataType DT = GetDataType<T>(),
              std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>, bool> = true>
    Attribute(std::initializer_list<T> data, const DataType dataType = DT, const Tensor::TensorShape& shape = {})
        : Attribute(std::vector(data), dataType, shape)
        {}

    explicit Attribute(const ResId resId)
        : m_ResId(resId) {};

    const std::vector<uint32_t>& GetData() const { return m_AttributeData; }
    Tensor GetTensor() const { return m_Tensor; }
    ResId GetResId() const { return m_ResId; }

    bool operator==(const Attribute& other) const
    {
        if (this->m_Tensor != other.m_Tensor)
            return false;
        if (this->m_AttributeData.size() != other.m_AttributeData.size())
            return false;
        for (size_t dataIdx = 0; dataIdx < this->m_AttributeData.size(); ++dataIdx)
        {
            if (this->m_AttributeData[dataIdx] != other.m_AttributeData[dataIdx])
                return false;
        }
        return true;
    }

    bool operator!=(const Attribute& other) const { return !(*this == other); }

    private:
    Tensor m_Tensor;
    std::vector<uint32_t> m_AttributeData;
    ResId m_ResId = nullptr;
};

template <> constexpr DataType GetDataType<int64_t>() { return DataType::int48_t; }
template <> constexpr DataType GetDataType<uint32_t>() { return DataType::uint32_t; }
template <> constexpr DataType GetDataType<int32_t>() { return DataType::int32_t; }
template <> constexpr DataType GetDataType<int16_t>() { return DataType::int16_t; };
template <> constexpr DataType GetDataType<int8_t>() { return DataType::int8_t; }
template <> constexpr DataType GetDataType<uint8_t>() { return DataType::uint8_t; };
template <> constexpr DataType GetDataType<float>() { return DataType::float32_t; }
template <> constexpr DataType GetDataType<bool>() { return DataType::bool_t; }

} // namespace tfsc::tosa
