//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#include <OperatorDefinitions.hpp>
#include <OperatorEnum.hpp>

#include <array>
#include <stdexcept>
#include <string>

namespace tosa2spirv::tosa
{

// See tosa2spirv/python/source_generator.py and README
// THIS SECTION IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
std::string GetOperatorName(const OperatorEnum op)
{
    // OPERATOR ENUM NAME SECTION BEGIN
    switch (op)
    {
        case OperatorEnum::Argmax:
        {
            return {"Argmax"};
        }
        case OperatorEnum::AvgPool2d:
        {
            return {"Avgpool2d"};
        }
        case OperatorEnum::Conv2d:
        {
            return {"Conv2d"};
        }
        case OperatorEnum::Conv3d:
        {
            return {"Conv3d"};
        }
        case OperatorEnum::DepthwiseConv2d:
        {
            return {"Depthwiseconv2d"};
        }
        case OperatorEnum::Fft2d:
        {
            return {"Fft2d"};
        }
        case OperatorEnum::FullyConnected:
        {
            return {"Fullyconnected"};
        }
        case OperatorEnum::Matmul:
        {
            return {"Matmul"};
        }
        case OperatorEnum::MaxPool2d:
        {
            return {"Maxpool2d"};
        }
        case OperatorEnum::Rfft2d:
        {
            return {"Rfft2d"};
        }
        case OperatorEnum::TransposeConv2d:
        {
            return {"Transposeconv2d"};
        }
        case OperatorEnum::Clamp:
        {
            return {"Clamp"};
        }
        case OperatorEnum::Erf:
        {
            return {"Erf"};
        }
        case OperatorEnum::Sigmoid:
        {
            return {"Sigmoid"};
        }
        case OperatorEnum::Tanh:
        {
            return {"Tanh"};
        }
        case OperatorEnum::Add:
        {
            return {"Add"};
        }
        case OperatorEnum::ArithmeticRightShift:
        {
            return {"Arithmeticrightshift"};
        }
        case OperatorEnum::BitwiseAnd:
        {
            return {"Bitwiseand"};
        }
        case OperatorEnum::BitwiseOr:
        {
            return {"Bitwiseor"};
        }
        case OperatorEnum::BitwiseXor:
        {
            return {"Bitwisexor"};
        }
        case OperatorEnum::Intdiv:
        {
            return {"Intdiv"};
        }
        case OperatorEnum::LogicalAnd:
        {
            return {"Logicaland"};
        }
        case OperatorEnum::LogicalLeftShift:
        {
            return {"Logicalleftshift"};
        }
        case OperatorEnum::LogicalRightShift:
        {
            return {"Logicalrightshift"};
        }
        case OperatorEnum::LogicalOr:
        {
            return {"Logicalor"};
        }
        case OperatorEnum::LogicalXor:
        {
            return {"Logicalxor"};
        }
        case OperatorEnum::Maximum:
        {
            return {"Maximum"};
        }
        case OperatorEnum::Minimum:
        {
            return {"Minimum"};
        }
        case OperatorEnum::Mul:
        {
            return {"Mul"};
        }
        case OperatorEnum::Pow:
        {
            return {"Pow"};
        }
        case OperatorEnum::Sub:
        {
            return {"Sub"};
        }
        case OperatorEnum::Table:
        {
            return {"Table"};
        }
        case OperatorEnum::Abs:
        {
            return {"Abs"};
        }
        case OperatorEnum::BitwiseNot:
        {
            return {"Bitwisenot"};
        }
        case OperatorEnum::Ceil:
        {
            return {"Ceil"};
        }
        case OperatorEnum::Clz:
        {
            return {"Clz"};
        }
        case OperatorEnum::Exp:
        {
            return {"Exp"};
        }
        case OperatorEnum::Floor:
        {
            return {"Floor"};
        }
        case OperatorEnum::Log:
        {
            return {"Log"};
        }
        case OperatorEnum::LogicalNot:
        {
            return {"Logicalnot"};
        }
        case OperatorEnum::Negate:
        {
            return {"Negate"};
        }
        case OperatorEnum::Reciprocal:
        {
            return {"Reciprocal"};
        }
        case OperatorEnum::Rsqrt:
        {
            return {"Rsqrt"};
        }
        case OperatorEnum::Select:
        {
            return {"Select"};
        }
        case OperatorEnum::Equal:
        {
            return {"Equal"};
        }
        case OperatorEnum::Greater:
        {
            return {"Greater"};
        }
        case OperatorEnum::GreaterEqual:
        {
            return {"Greaterequal"};
        }
        case OperatorEnum::ReduceAll:
        {
            return {"Reduceall"};
        }
        case OperatorEnum::ReduceAny:
        {
            return {"Reduceany"};
        }
        case OperatorEnum::ReduceMax:
        {
            return {"Reducemax"};
        }
        case OperatorEnum::ReduceMin:
        {
            return {"Reducemin"};
        }
        case OperatorEnum::ReduceProduct:
        {
            return {"Reduceproduct"};
        }
        case OperatorEnum::ReduceSum:
        {
            return {"Reducesum"};
        }
        case OperatorEnum::Concat:
        {
            return {"Concat"};
        }
        case OperatorEnum::Pad:
        {
            return {"Pad"};
        }
        case OperatorEnum::Reshape:
        {
            return {"Reshape"};
        }
        case OperatorEnum::Reverse:
        {
            return {"Reverse"};
        }
        case OperatorEnum::Slice:
        {
            return {"Slice"};
        }
        case OperatorEnum::Tile:
        {
            return {"Tile"};
        }
        case OperatorEnum::Transpose:
        {
            return {"Transpose"};
        }
        case OperatorEnum::Gather:
        {
            return {"Gather"};
        }
        case OperatorEnum::Scatter:
        {
            return {"Scatter"};
        }
        case OperatorEnum::Resize:
        {
            return {"Resize"};
        }
        case OperatorEnum::Cast:
        {
            return {"Cast"};
        }
        case OperatorEnum::Rescale:
        {
            return {"Rescale"};
        }
        // OPERATOR ENUM NAME SECTION END
        default: throw std::invalid_argument("Invalid OperatorEnum");
    }
}

OperatorDefinition GetOperatorDefinition(const OperatorEnum op)
{
    // OPERATOR DEFINITION SECTION BEGIN
    switch (op)
    {
        case OperatorEnum::Argmax:
        case OperatorEnum::ReduceAll:
        case OperatorEnum::ReduceAny:
        case OperatorEnum::ReduceMax:
        case OperatorEnum::ReduceMin:
        case OperatorEnum::ReduceProduct:
        case OperatorEnum::ReduceSum:
        case OperatorEnum::Concat:
        case OperatorEnum::Reverse:
        {
            static constexpr std::array<AttributeDefinition, 1> definitions{{{Category::Scalar, Metadata::None}}};
            return {1, 1, 1, definitions.data()};
        }
        case OperatorEnum::AvgPool2d:
        {
            static constexpr std::array<AttributeDefinition, 6> definitions{{{Category::Array, Metadata::None},
                                                                             {Category::Array, Metadata::None},
                                                                             {Category::Array, Metadata::None},
                                                                             {Category::Enum, Metadata::None},
                                                                             {Category::Scalar, Metadata::None},
                                                                             {Category::Scalar, Metadata::None}}};
            return {1, 1, 6, definitions.data()};
        }
        case OperatorEnum::Conv2d:
        case OperatorEnum::Conv3d:
        case OperatorEnum::DepthwiseConv2d:
        case OperatorEnum::TransposeConv2d:
        {
            static constexpr std::array<AttributeDefinition, 6> definitions{{{Category::Array, Metadata::None},
                                                                             {Category::Array, Metadata::None},
                                                                             {Category::Array, Metadata::None},
                                                                             {Category::Scalar, Metadata::None},
                                                                             {Category::Scalar, Metadata::None},
                                                                             {Category::Scalar, Metadata::None}}};
            return {3, 1, 6, definitions.data()};
        }
        case OperatorEnum::Fft2d:
        {
            static constexpr std::array<AttributeDefinition, 2> definitions{
                {{Category::Scalar, Metadata::None}, {Category::Scalar, Metadata::None}}};
            return {2, 2, 2, definitions.data()};
        }
        case OperatorEnum::FullyConnected:
        {
            static constexpr std::array<AttributeDefinition, 2> definitions{
                {{Category::Scalar, Metadata::None}, {Category::Scalar, Metadata::None}}};
            return {3, 1, 2, definitions.data()};
        }
        case OperatorEnum::Matmul:
        {
            static constexpr std::array<AttributeDefinition, 2> definitions{
                {{Category::Scalar, Metadata::None}, {Category::Scalar, Metadata::None}}};
            return {2, 1, 2, definitions.data()};
        }
        case OperatorEnum::MaxPool2d:
        {
            static constexpr std::array<AttributeDefinition, 3> definitions{{{Category::Array, Metadata::None},
                                                                             {Category::Array, Metadata::None},
                                                                             {Category::Array, Metadata::None}}};
            return {1, 1, 3, definitions.data()};
        }
        case OperatorEnum::Rfft2d:
        {
            return {1, 2, 0, nullptr};
        }
        case OperatorEnum::Clamp:
        case OperatorEnum::Negate:
        {
            static constexpr std::array<AttributeDefinition, 2> definitions{
                {{Category::Scalar, Metadata::None}, {Category::Scalar, Metadata::None}}};
            return {1, 1, 2, definitions.data()};
        }
        case OperatorEnum::Erf:
        case OperatorEnum::Sigmoid:
        case OperatorEnum::Tanh:
        case OperatorEnum::Abs:
        case OperatorEnum::BitwiseNot:
        case OperatorEnum::Ceil:
        case OperatorEnum::Clz:
        case OperatorEnum::Exp:
        case OperatorEnum::Floor:
        case OperatorEnum::Log:
        case OperatorEnum::LogicalNot:
        case OperatorEnum::Reciprocal:
        case OperatorEnum::Rsqrt:
        case OperatorEnum::Cast:
        {
            return {1, 1, 0, nullptr};
        }
        case OperatorEnum::Add:
        case OperatorEnum::BitwiseAnd:
        case OperatorEnum::BitwiseOr:
        case OperatorEnum::BitwiseXor:
        case OperatorEnum::Intdiv:
        case OperatorEnum::LogicalAnd:
        case OperatorEnum::LogicalLeftShift:
        case OperatorEnum::LogicalRightShift:
        case OperatorEnum::LogicalOr:
        case OperatorEnum::LogicalXor:
        case OperatorEnum::Maximum:
        case OperatorEnum::Minimum:
        case OperatorEnum::Pow:
        case OperatorEnum::Sub:
        case OperatorEnum::Equal:
        case OperatorEnum::Greater:
        case OperatorEnum::GreaterEqual:
        case OperatorEnum::Gather:
        {
            return {2, 1, 0, nullptr};
        }
        case OperatorEnum::ArithmeticRightShift:
        case OperatorEnum::Mul:
        {
            static constexpr std::array<AttributeDefinition, 1> definitions{{{Category::Scalar, Metadata::None}}};
            return {2, 1, 1, definitions.data()};
        }
        case OperatorEnum::Table:
        {
            static constexpr std::array<AttributeDefinition, 1> definitions{
                {{Category::GraphConstant, Metadata::None}}};
            return {1, 1, 1, definitions.data()};
        }
        case OperatorEnum::Select:
        case OperatorEnum::Scatter:
        {
            return {3, 1, 0, nullptr};
        }
        case OperatorEnum::Pad:
        {
            static constexpr std::array<AttributeDefinition, 2> definitions{
                {{Category::GraphConstant, Metadata::Shape}, {Category::Scalar, Metadata::None}}};
            return {1, 1, 2, definitions.data()};
        }
        case OperatorEnum::Reshape:
        case OperatorEnum::Tile:
        {
            static constexpr std::array<AttributeDefinition, 1> definitions{{{Category::Ranked, Metadata::Shape}}};
            return {1, 1, 1, definitions.data()};
        }
        case OperatorEnum::Slice:
        {
            static constexpr std::array<AttributeDefinition, 2> definitions{
                {{Category::Ranked, Metadata::None}, {Category::Ranked, Metadata::None}}};
            return {1, 1, 2, definitions.data()};
        }
        case OperatorEnum::Transpose:
        {
            static constexpr std::array<AttributeDefinition, 1> definitions{{{Category::Ranked, Metadata::None}}};
            return {1, 1, 1, definitions.data()};
        }
        case OperatorEnum::Resize:
        {
            static constexpr std::array<AttributeDefinition, 4> definitions{{{Category::Array, Metadata::Shape},
                                                                             {Category::Array, Metadata::Shape},
                                                                             {Category::Array, Metadata::Shape},
                                                                             {Category::Enum, Metadata::None}}};
            return {1, 1, 4, definitions.data()};
        }
        case OperatorEnum::Rescale:
        {
            static constexpr std::array<AttributeDefinition, 9> definitions{{{Category::Scalar, Metadata::None},
                                                                             {Category::Scalar, Metadata::None},
                                                                             {Category::GraphConstant, Metadata::None},
                                                                             {Category::GraphConstant, Metadata::None},
                                                                             {Category::Scalar, Metadata::None},
                                                                             {Category::Scalar, Metadata::None},
                                                                             {Category::Scalar, Metadata::None},
                                                                             {Category::Scalar, Metadata::None},
                                                                             {Category::Scalar, Metadata::None}}};
            return {1, 1, 9, definitions.data()};
        }
        // OPERATOR DEFINITION SECTION END
        default: return {};
    }
}

TOSAInstructions GetTOSAInstructions(const OperatorEnum operatorType)
{
    // OPERATOR ENUM MAP SECTION BEGIN
    switch (operatorType)
    {
        case OperatorEnum::Argmax: return TOSAARGMAX;
        case OperatorEnum::AvgPool2d: return TOSAAVG_POOL2D;
        case OperatorEnum::Conv2d: return TOSACONV2D;
        case OperatorEnum::Conv3d: return TOSACONV3D;
        case OperatorEnum::DepthwiseConv2d: return TOSADEPTHWISE_CONV2D;
        case OperatorEnum::Fft2d: return TOSAFFT2D;
        case OperatorEnum::FullyConnected: return TOSAFULLY_CONNECTED;
        case OperatorEnum::Matmul: return TOSAMATMUL;
        case OperatorEnum::MaxPool2d: return TOSAMAX_POOL2D;
        case OperatorEnum::Rfft2d: return TOSARFFT2D;
        case OperatorEnum::TransposeConv2d: return TOSATRANSPOSE_CONV2D;
        case OperatorEnum::Clamp: return TOSACLAMP;
        case OperatorEnum::Erf: return TOSAERF;
        case OperatorEnum::Sigmoid: return TOSASIGMOID;
        case OperatorEnum::Tanh: return TOSATANH;
        case OperatorEnum::Add: return TOSAADD;
        case OperatorEnum::ArithmeticRightShift: return TOSAARITHMETIC_RIGHT_SHIFT;
        case OperatorEnum::BitwiseAnd: return TOSABITWISE_AND;
        case OperatorEnum::BitwiseOr: return TOSABITWISE_OR;
        case OperatorEnum::BitwiseXor: return TOSABITWISE_XOR;
        case OperatorEnum::Intdiv: return TOSAINTDIV;
        case OperatorEnum::LogicalAnd: return TOSALOGICAL_AND;
        case OperatorEnum::LogicalLeftShift: return TOSALOGICAL_LEFT_SHIFT;
        case OperatorEnum::LogicalRightShift: return TOSALOGICAL_RIGHT_SHIFT;
        case OperatorEnum::LogicalOr: return TOSALOGICAL_OR;
        case OperatorEnum::LogicalXor: return TOSALOGICAL_XOR;
        case OperatorEnum::Maximum: return TOSAMAXIMUM;
        case OperatorEnum::Minimum: return TOSAMINIMUM;
        case OperatorEnum::Mul: return TOSAMUL;
        case OperatorEnum::Pow: return TOSAPOW;
        case OperatorEnum::Sub: return TOSASUB;
        case OperatorEnum::Table: return TOSATABLE;
        case OperatorEnum::Abs: return TOSAABS;
        case OperatorEnum::BitwiseNot: return TOSABITWISE_NOT;
        case OperatorEnum::Ceil: return TOSACEIL;
        case OperatorEnum::Clz: return TOSACLZ;
        case OperatorEnum::Exp: return TOSAEXP;
        case OperatorEnum::Floor: return TOSAFLOOR;
        case OperatorEnum::Log: return TOSALOG;
        case OperatorEnum::LogicalNot: return TOSALOGICAL_NOT;
        case OperatorEnum::Negate: return TOSANEGATE;
        case OperatorEnum::Reciprocal: return TOSARECIPROCAL;
        case OperatorEnum::Rsqrt: return TOSARSQRT;
        case OperatorEnum::Select: return TOSASELECT;
        case OperatorEnum::Equal: return TOSAEQUAL;
        case OperatorEnum::Greater: return TOSAGREATER;
        case OperatorEnum::GreaterEqual: return TOSAGREATER_EQUAL;
        case OperatorEnum::ReduceAll: return TOSAREDUCE_ALL;
        case OperatorEnum::ReduceAny: return TOSAREDUCE_ANY;
        case OperatorEnum::ReduceMax: return TOSAREDUCE_MAX;
        case OperatorEnum::ReduceMin: return TOSAREDUCE_MIN;
        case OperatorEnum::ReduceProduct: return TOSAREDUCE_PRODUCT;
        case OperatorEnum::ReduceSum: return TOSAREDUCE_SUM;
        case OperatorEnum::Concat: return TOSACONCAT;
        case OperatorEnum::Pad: return TOSAPAD;
        case OperatorEnum::Reshape: return TOSARESHAPE;
        case OperatorEnum::Reverse: return TOSAREVERSE;
        case OperatorEnum::Slice: return TOSASLICE;
        case OperatorEnum::Tile: return TOSATILE;
        case OperatorEnum::Transpose: return TOSATRANSPOSE;
        case OperatorEnum::Gather: return TOSAGATHER;
        case OperatorEnum::Scatter: return TOSASCATTER;
        case OperatorEnum::Resize: return TOSARESIZE;
        case OperatorEnum::Cast: return TOSACAST;
        case OperatorEnum::Rescale: return TOSARESCALE;
        // OPERATOR ENUM MAP SECTION END
        default: throw std::invalid_argument("Invalid OperatorEnum");
    }
}
} // namespace tosa2spirv::tosa
