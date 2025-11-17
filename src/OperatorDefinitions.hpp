//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>

#include <spirv/unified1/TOSA.001000.1.h>

namespace tfsc::tosa
{

enum class OperatorEnum;

// Category describes the characteristics of the tensor.
enum class Category
{
    // Fixed shape and size of {n}
    Array,
    // Enum
    Enum,
    // General tensor of any size and shape
    Ranked,
    // Fixed shape and size of {1}
    Scalar,
    // Tensor describes the shape of another tensor. Fixed shape of {n},
    // n being the number of dimensions of the described tensor
    Shape,
    // List of n homogenous tensors
    TensorList,
    // Tensor representing a tosa graph
    TosaGraph,

    GraphConstant,
};

// MetaData, tensor used to describe the shape of another tensor
enum class Metadata
{
    None,
    Shape,
};

struct AttributeDefinition
{
    Category m_Category;
    Metadata m_Metadata;
};

// Collection of type information sufficient to describe a tensor
struct OperatorDefinition
{
    const unsigned int m_InputSize;
    const unsigned int m_OutputSize;
    const unsigned int m_AttributeSize;
    const AttributeDefinition* m_AttributeDefinitions;
};

TOSAInstructions GetTOSAInstructions(OperatorEnum operatorType);
OperatorDefinition GetOperatorDefinition(OperatorEnum op);
std::string GetOperatorName(const OperatorEnum op);

} // namespace tfsc::tosa
