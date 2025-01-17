//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#include <OperatorDefinitions.hpp>
#include <stdexcept>

namespace tosa2spirv::graphbuilder
{

// See tosa2spirv/python/source_generator.py and README
// THIS SECTION IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
std::string GetOperatorName(const OperatorEnum op)
{
    switch(op)
    {
        case OperatorEnum::Argmax: { return {"Argmax"};}
        case OperatorEnum::AvgPool2d: { return {"Avgpool2d"};}
        case OperatorEnum::Conv2d: { return {"Conv2d"};}
        case OperatorEnum::Conv3d: { return {"Conv3d"};}
        case OperatorEnum::DepthwiseConv2d: { return {"Depthwiseconv2d"};}
        case OperatorEnum::Fft2d: { return {"Fft2d"};}
        case OperatorEnum::FullyConnected: { return {"Fullyconnected"};}
        case OperatorEnum::Matmul: { return {"Matmul"};}
        case OperatorEnum::MaxPool2d: { return {"Maxpool2d"};}
        case OperatorEnum::Rfft2d: { return {"Rfft2d"};}
        case OperatorEnum::TransposeConv2d: { return {"Transposeconv2d"};}
        case OperatorEnum::Clamp: { return {"Clamp"};}
        case OperatorEnum::Erf: { return {"Erf"};}
        case OperatorEnum::Sigmoid: { return {"Sigmoid"};}
        case OperatorEnum::Tanh: { return {"Tanh"};}
        case OperatorEnum::Add: { return {"Add"};}
        case OperatorEnum::ArithmeticRightShift: { return {"Arithmeticrightshift"};}
        case OperatorEnum::BitwiseAnd: { return {"Bitwiseand"};}
        case OperatorEnum::BitwiseOr: { return {"Bitwiseor"};}
        case OperatorEnum::BitwiseXor: { return {"Bitwisexor"};}
        case OperatorEnum::Intdiv: { return {"Intdiv"};}
        case OperatorEnum::LogicalAnd: { return {"Logicaland"};}
        case OperatorEnum::LogicalLeftShift: { return {"Logicalleftshift"};}
        case OperatorEnum::LogicalRightShift: { return {"Logicalrightshift"};}
        case OperatorEnum::LogicalOr: { return {"Logicalor"};}
        case OperatorEnum::LogicalXor: { return {"Logicalxor"};}
        case OperatorEnum::Maximum: { return {"Maximum"};}
        case OperatorEnum::Minimum: { return {"Minimum"};}
        case OperatorEnum::Mul: { return {"Mul"};}
        case OperatorEnum::Pow: { return {"Pow"};}
        case OperatorEnum::Sub: { return {"Sub"};}
        case OperatorEnum::Table: { return {"Table"};}
        case OperatorEnum::Abs: { return {"Abs"};}
        case OperatorEnum::BitwiseNot: { return {"Bitwisenot"};}
        case OperatorEnum::Ceil: { return {"Ceil"};}
        case OperatorEnum::Clz: { return {"Clz"};}
        case OperatorEnum::Exp: { return {"Exp"};}
        case OperatorEnum::Floor: { return {"Floor"};}
        case OperatorEnum::Log: { return {"Log"};}
        case OperatorEnum::LogicalNot: { return {"Logicalnot"};}
        case OperatorEnum::Negate: { return {"Negate"};}
        case OperatorEnum::Reciprocal: { return {"Reciprocal"};}
        case OperatorEnum::Rsqrt: { return {"Rsqrt"};}
        case OperatorEnum::Select: { return {"Select"};}
        case OperatorEnum::Equal: { return {"Equal"};}
        case OperatorEnum::Greater: { return {"Greater"};}
        case OperatorEnum::GreaterEqual: { return {"Greaterequal"};}
        case OperatorEnum::ReduceAll: { return {"Reduceall"};}
        case OperatorEnum::ReduceAny: { return {"Reduceany"};}
        case OperatorEnum::ReduceMax: { return {"Reducemax"};}
        case OperatorEnum::ReduceMin: { return {"Reducemin"};}
        case OperatorEnum::ReduceProduct: { return {"Reduceproduct"};}
        case OperatorEnum::ReduceSum: { return {"Reducesum"};}
        case OperatorEnum::Concat: { return {"Concat"};}
        case OperatorEnum::Pad: { return {"Pad"};}
        case OperatorEnum::Reshape: { return {"Reshape"};}
        case OperatorEnum::Reverse: { return {"Reverse"};}
        case OperatorEnum::Slice: { return {"Slice"};}
        case OperatorEnum::Tile: { return {"Tile"};}
        case OperatorEnum::Transpose: { return {"Transpose"};}
        case OperatorEnum::Gather: { return {"Gather"};}
        case OperatorEnum::Scatter: { return {"Scatter"};}
        case OperatorEnum::Resize: { return {"Resize"};}
        case OperatorEnum::Cast: { return {"Cast"};}
        case OperatorEnum::Rescale: { return {"Rescale"};}
        default: throw std::invalid_argument("Invalid OperatorEnum");
    }
}

OperatorDefinition GetOperatorDefinition(const OperatorEnum op)
{
    switch(op)
    {
        case OperatorEnum::Argmax:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::AvgPool2d:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Enum, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Conv2d:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Conv3d:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::DepthwiseConv2d:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Fft2d:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::FullyConnected:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Matmul:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::MaxPool2d:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Rfft2d:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::TransposeConv2d:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Clamp:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Erf:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Sigmoid:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Tanh:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Add:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::ArithmeticRightShift:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::BitwiseAnd:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::BitwiseOr:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::BitwiseXor:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Intdiv:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::LogicalAnd:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::LogicalLeftShift:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::LogicalRightShift:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::LogicalOr:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::LogicalXor:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Maximum:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Minimum:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Mul:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Pow:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Sub:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Table:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Abs:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::BitwiseNot:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Ceil:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Clz:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Exp:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Floor:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Log:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::LogicalNot:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Negate:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Reciprocal:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Rsqrt:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Select:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Equal:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Greater:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::GreaterEqual:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::ReduceAll:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::ReduceAny:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::ReduceMax:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::ReduceMin:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::ReduceProduct:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::ReduceSum:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Concat:
        {
            return {{Argument::Input, Category::TensorList, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Pad:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::Shape},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Reshape:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Ranked, Metadata::Shape},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Reverse:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Slice:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Tile:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Ranked, Metadata::Shape},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Transpose:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Gather:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Scatter:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Resize:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Array, Metadata::Shape},
                   {Argument::Attribute, Category::Array, Metadata::Shape},
                   {Argument::Attribute, Category::Array, Metadata::Shape},
                   {Argument::Attribute, Category::Enum, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Cast:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        case OperatorEnum::Rescale:
        {
            return {{Argument::Input, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Ranked, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Attribute, Category::Scalar, Metadata::None},
                   {Argument::Output, Category::Ranked, Metadata::None}};
        }
        default: return {};
        }
    }

//                           ==================  Generated Code End ==================
}// namespace graphbuilder::tosa2spirv
