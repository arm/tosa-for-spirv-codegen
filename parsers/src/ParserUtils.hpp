//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA 1.0. DO NOT EDIT!
// See tosa2spirv/python/source_generator.py and README

#pragma once

#include <TosaSerializationParser.hpp>
#include <iostream>

namespace tosa2spirv::parsers
{
using namespace ::tosa;
using namespace tosa2spirv::tosa;

inline DataType GetDataTypeFromDType(const DType dType)
{
    switch (dType)
    {
        case DType_BOOL: return DataType::bool_t;
        case DType_INT4: return DataType::int4_t;
        case DType_INT8: return DataType::int8_t;
        case DType_INT16: return DataType::int16_t;
        case DType_INT32: return DataType::int32_t;
        case DType_INT48: return DataType::int48_t;
        case DType_BF16: return DataType::bfloat16_t;
        case DType_FP16: return DataType::float16_t;
        case DType_FP32: return DataType::float32_t;
        case DType_SHAPE: return DataType::int64_t;
        default:
            throw std::invalid_argument("ERROR: No Corresponding DataType for DType " +
                                        std::string(EnumNamesDType()[dType]) + ".");
    }
}

inline std::vector<uint8_t> UnpackInt4Signed(const std::vector<uint8_t>& input, const Tensor& tensor)
{
    std::vector<uint8_t> output;
    const size_t numElements = tensor.GetNumElements();
    output.reserve(numElements);

    if (input.size() * 2 < numElements)
    {

        throw std::invalid_argument("uint8_t vector too small for in4 tensor size");
    }

    for (size_t i = 0; i < numElements / 2; ++i)
    {
        uint8_t packed = input[i];
        uint8_t low = packed & 0x0F;
        uint8_t high = (packed >> 4) & 0x0F;

        // Sign-extend 4-bit to 8-bit
        if (low & 0x8)
            low |= 0xF0;
        if (high & 0x8)
            high |= 0xF0;

        output.push_back(low);
        output.push_back(high);
    }

    if (numElements % 2 != 0)
    {
        uint8_t last = input[numElements / 2] & 0x0F;
        if (last & 0x8)
            last |= 0xF0;
        output.push_back(last);
    }

    return output;
}

inline std::vector<int64_t> ConvertToInt64(const std::vector<uint8_t>& input, const Tensor& tensor)
{
    constexpr size_t int48Size = 6;
    const size_t numElements = tensor.GetNumElements();
    assert(input.size() >= numElements * int48Size);

    std::vector<int64_t> output;
    output.reserve(numElements);

    for (size_t i = 0; i < numElements; ++i)
    {
        const size_t idx = i * int48Size;
        uint64_t val = static_cast<uint64_t>(input[idx]) | (static_cast<uint64_t>(input[idx + 1]) << 8) |
                       (static_cast<uint64_t>(input[idx + 2]) << 16) | (static_cast<uint64_t>(input[idx + 3]) << 24) |
                       (static_cast<uint64_t>(input[idx + 4]) << 32) | (static_cast<uint64_t>(input[idx + 5]) << 40);

        if (val & (1ull << 47))
        {
            val |= 0xFFFF'000000000000ULL;
        }
        output.push_back(static_cast<int64_t>(val));
    }
    return output;
}

inline std::vector<uint32_t> ConvertToUint32(const std::vector<uint8_t>& input, const Tensor& tensor)
{
    unsigned int paddingBytes = 0;
    unsigned int stepSize = 1;
    auto increment = 4;
    switch (tensor.GetDataType())
    {
        case DataType::bool_t:
        case DataType::int8_t:
        case DataType::uint8_t: paddingBytes = 3; break;
        case DataType::int16_t:
        case DataType::float16_t:
        case DataType::bfloat16_t:
        case DataType::uint16_t:
            stepSize = 2;
            paddingBytes = 2;
            break;
        case DataType::int32_t:
        case DataType::uint32_t:
        case DataType::float32_t: stepSize = 4; break;
        case DataType::int64_t:
            stepSize = 8;
            increment = 8;
            break;
        default: break;
    }

    if (input.size() < tensor.GetNumElements() * stepSize)
    {

        throw std::invalid_argument("input vector too small for infered tensor size");
    }

    std::vector<uint8_t> reducedInput{input.begin(), input.begin() + (tensor.GetNumElements() * stepSize)};
    std::vector<uint8_t> paddedInput;
    for (auto inputIt = reducedInput.begin(); inputIt != reducedInput.end(); std::advance(inputIt, stepSize))
    {
        paddedInput.insert(paddedInput.end(), inputIt, inputIt + stepSize);
        paddedInput.insert(paddedInput.end(), paddingBytes, 0u);
    }

    size_t i = 0;
    std::vector<uint32_t> output;
    while (i + increment <= paddedInput.size())
    {
        uint32_t value = 0;
        value |= static_cast<uint32_t>(paddedInput[i]) | (static_cast<uint32_t>(paddedInput[i + 1]) << 8) |
                 (static_cast<uint32_t>(paddedInput[i + 2]) << 16) | (static_cast<uint32_t>(paddedInput[i + 3]) << 24);
        output.push_back(value);
        i += increment;
    }

    return output;
}

inline std::vector<uint32_t> ConvertTensorShape(std::vector<int32_t> shape)
{
    std::vector<uint32_t> unsignedShape;
    // Handle rank 0 tensors
    if (shape.empty())
    {
        shape = {1};
    }

    unsignedShape.reserve(shape.size());
    for (auto& elementNum : shape)
    {
        unsignedShape.emplace_back(static_cast<uint32_t>(elementNum));
    }

    return unsignedShape;
}

inline std::vector<uint32_t> ConvertTensorShape(std::vector<int16_t> shape)
{
    std::vector<uint32_t> unsignedShape;
    // Handle rank 0 tensors
    if (shape.empty())
    {
        shape = {1};
    }

    unsignedShape.reserve(shape.size());
    for (auto& elementNum : shape)
    {
        unsignedShape.emplace_back(static_cast<uint32_t>(elementNum));
    }

    return unsignedShape;
}

inline OperatorEnum OperatorEnumMap(const Op op)
{
    switch (op)
    {
        // OPERATOR NAME MAP SECTION BEGIN
        case Op_ARGMAX: return OperatorEnum::ArgMax;
        case Op_AVG_POOL2D: return OperatorEnum::AvgPool2d;
        case Op_CONV2D: return OperatorEnum::Conv2d;
        case Op_CONV3D: return OperatorEnum::Conv3d;
        case Op_DEPTHWISE_CONV2D: return OperatorEnum::DepthwiseConv2d;
        case Op_FFT2D: return OperatorEnum::Fft2d;
        case Op_MATMUL: return OperatorEnum::Matmul;
        case Op_MAX_POOL2D: return OperatorEnum::MaxPool2d;
        case Op_RFFT2D: return OperatorEnum::Rfft2d;
        case Op_TRANSPOSE_CONV2D: return OperatorEnum::TransposeConv2d;
        case Op_CLAMP: return OperatorEnum::Clamp;
        case Op_ERF: return OperatorEnum::Erf;
        case Op_SIGMOID: return OperatorEnum::Sigmoid;
        case Op_TANH: return OperatorEnum::Tanh;
        case Op_ADD: return OperatorEnum::Add;
        case Op_ARITHMETIC_RIGHT_SHIFT: return OperatorEnum::ArithmeticRightShift;
        case Op_BITWISE_AND: return OperatorEnum::BitwiseAnd;
        case Op_BITWISE_OR: return OperatorEnum::BitwiseOr;
        case Op_BITWISE_XOR: return OperatorEnum::BitwiseXor;
        case Op_INTDIV: return OperatorEnum::IntDiv;
        case Op_LOGICAL_AND: return OperatorEnum::LogicalAnd;
        case Op_LOGICAL_LEFT_SHIFT: return OperatorEnum::LogicalLeftShift;
        case Op_LOGICAL_RIGHT_SHIFT: return OperatorEnum::LogicalRightShift;
        case Op_LOGICAL_OR: return OperatorEnum::LogicalOr;
        case Op_LOGICAL_XOR: return OperatorEnum::LogicalXor;
        case Op_MAXIMUM: return OperatorEnum::Maximum;
        case Op_MINIMUM: return OperatorEnum::Minimum;
        case Op_MUL: return OperatorEnum::Mul;
        case Op_POW: return OperatorEnum::Pow;
        case Op_SUB: return OperatorEnum::Sub;
        case Op_TABLE: return OperatorEnum::Table;
        case Op_ABS: return OperatorEnum::Abs;
        case Op_BITWISE_NOT: return OperatorEnum::BitwiseNot;
        case Op_CEIL: return OperatorEnum::Ceil;
        case Op_CLZ: return OperatorEnum::Clz;
        case Op_COS: return OperatorEnum::Cos;
        case Op_EXP: return OperatorEnum::Exp;
        case Op_FLOOR: return OperatorEnum::Floor;
        case Op_LOG: return OperatorEnum::Log;
        case Op_LOGICAL_NOT: return OperatorEnum::LogicalNot;
        case Op_NEGATE: return OperatorEnum::Negate;
        case Op_RECIPROCAL: return OperatorEnum::Reciprocal;
        case Op_RSQRT: return OperatorEnum::Rsqrt;
        case Op_SIN: return OperatorEnum::Sin;
        case Op_SELECT: return OperatorEnum::Select;
        case Op_EQUAL: return OperatorEnum::Equal;
        case Op_GREATER: return OperatorEnum::Greater;
        case Op_GREATER_EQUAL: return OperatorEnum::GreaterEqual;
        case Op_REDUCE_ALL: return OperatorEnum::ReduceAll;
        case Op_REDUCE_ANY: return OperatorEnum::ReduceAny;
        case Op_REDUCE_MAX: return OperatorEnum::ReduceMax;
        case Op_REDUCE_MIN: return OperatorEnum::ReduceMin;
        case Op_REDUCE_PRODUCT: return OperatorEnum::ReduceProduct;
        case Op_REDUCE_SUM: return OperatorEnum::ReduceSum;
        case Op_CONCAT: return OperatorEnum::Concat;
        case Op_PAD: return OperatorEnum::Pad;
        case Op_RESHAPE: return OperatorEnum::Reshape;
        case Op_REVERSE: return OperatorEnum::Reverse;
        case Op_SLICE: return OperatorEnum::Slice;
        case Op_TILE: return OperatorEnum::Tile;
        case Op_TRANSPOSE: return OperatorEnum::Transpose;
        case Op_GATHER: return OperatorEnum::Gather;
        case Op_SCATTER: return OperatorEnum::Scatter;
        case Op_RESIZE: return OperatorEnum::Resize;
        case Op_CAST: return OperatorEnum::Cast;
        case Op_RESCALE: return OperatorEnum::Rescale;
        // OPERATOR NAME MAP SECTION END
        default:
            std::cerr << "ERROR: Operator " << EnumNameOp(op) << " is currently unsupported" << std::endl;
            throw std::exception();
    }
}
} // namespace tosa2spirv::parsers
