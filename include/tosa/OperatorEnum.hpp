//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>

// tosa-for-spirv-codegen's shorthand namespace
namespace tfsc::tosa
{
// See tosa-for-spirv-codegen/python/source_generator.py and README
// THIS SECTION IS GENERATED WITH TOSA 1.0. DO NOT EDIT!
// OPERATOR ENUM SECTION BEGIN
enum class OperatorEnum
{
    ArgMax,
    AvgPool2d,
    Conv2d,
    Conv3d,
    DepthwiseConv2d,
    Fft2d,
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
    IntDiv,
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
    Cos,
    Exp,
    Floor,
    Log,
    LogicalNot,
    Negate,
    Reciprocal,
    Rsqrt,
    Sin,
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
}; // OPERATOR ENUM SECTION END

} // namespace tfsc::tosa
