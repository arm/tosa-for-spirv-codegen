//
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Tensor.hpp>
#include <utility>

namespace tosa2spirv
{

namespace graphbuilder
{

enum class DataType;

struct TensorInfo
{
    TensorInfo(DataType type,
               std::vector<unsigned int> shape,
               ResId resId,
               bool isGraphOutput,
               bool hasValue)
        : m_DataType(type)
        , m_Shape(std::move(shape))
        , m_ResId(resId)
        , m_IsGraphOutput(isGraphOutput)
        , m_HasValue(hasValue)
    {}

    DataType m_DataType;

    std::vector<unsigned int> m_Shape;

    ResId m_ResId;

    /// Allows the user to specify the output layer in the graph.
    bool m_IsGraphOutput;

    /// Flag to identify if the Tensor has values stored in the TensorValuePtr
    bool m_HasValue;

    DescSetBinding* m_DescSetBinding = nullptr;

    Category m_Category;
    Metadata m_Metadata;
};


} // namespace graphbuilder

} // namespace tosa2spirv