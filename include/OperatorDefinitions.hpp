//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>
#include <vector>

namespace tosa2spirv::graphbuilder
{

enum class OperatorEnum;
/// Operators process input arguments to produce output arguments.
/// Their behavior can be configured using attribute arguments.
enum class Argument
{
    Attribute,
    Output,
    Input,
};

/// Category describes the characteristics of the tensor.
enum class Category
{
    /// Fixed shape and size of {n}
    Array,
    /// Enum
    Enum,
    /// General tensor of any size and shape
    Ranked,
    /// Fixed shape and size of {1}
    Scalar,
    /// Tensor describes the shape of another tensor. Fixed shape of {n},
    /// n being the number of dimensions of the described tensor
    Shaped,
    /// List of n homogenous tensors
    TensorList,
    /// Tensor representing a tosa graph
    TosaGraph,
};

/// MetaData, tensor used to describe the shape of another tensor
enum class Metadata
{
    None,
    Shape,
};

/// Collection of type information sufficient to describe a tensor
struct TensorDefinition
{
    Argument m_Argument;
    Category m_Category;
    Metadata m_Metadata;
};

using OperatorDefinition = std::vector<TensorDefinition>;

/// Method to get the name of a given Operator
/// @param[in] op OperatorType Operator enum
/// @return A string with the operator name
std::string GetOperatorName(OperatorEnum op);

/// Method to get the OperatorDefinitions of a given Operator
/// @param[in] op OperatorType Operator enum
/// @return OperatorDefinition describing the Operator
OperatorDefinition GetOperatorDefinition(OperatorEnum op);

// See tosa2spirv/python/source_generator.py and README
// THIS SECTION IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
enum class OperatorEnum
{
    Argmax,
    AvgPool2d,
    Conv2d,
    Conv3d,
    DepthwiseConv2d,
    Fft2d,
    FullyConnected,
    Matmul,
    MaxPool2d,
    Rfft2d,
    TransposeConv2d,
    Clamp,
    Erf,
    Sigmoid,
    Tanh,
    Add,
    ArithmeticRightShift,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    Intdiv,
    LogicalAnd,
    LogicalLeftShift,
    LogicalRightShift,
    LogicalOr,
    LogicalXor,
    Maximum,
    Minimum,
    Mul,
    Pow,
    Sub,
    Table,
    Abs,
    BitwiseNot,
    Ceil,
    Clz,
    Exp,
    Floor,
    Log,
    LogicalNot,
    Negate,
    Reciprocal,
    Rsqrt,
    Select,
    Equal,
    Greater,
    GreaterEqual,
    ReduceAll,
    ReduceAny,
    ReduceMax,
    ReduceMin,
    ReduceProduct,
    ReduceSum,
    Concat,
    Pad,
    Reshape,
    Reverse,
    Slice,
    Tile,
    Transpose,
    Gather,
    Scatter,
    Resize,
    Cast,
    Rescale,
};

//                           ==================  Generated Code End ==================
}// namespace graphbuilder::tosa2spirv
