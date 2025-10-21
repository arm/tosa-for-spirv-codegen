//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <SPIRVDefinitions.hpp>

#include <algorithm>
#include <stdexcept>

namespace tfsc::spirv
{

using namespace spv;
using namespace tfsc::tosa;

Operand CreateDataType(const DataType datatype, Module& module)
{
    using tosa::DataType;
    switch (datatype)
    {
        case DataType::int4_t:
        case DataType::int8_t:
        case DataType::uint8_t: return module.EmplaceInstruction(OpTypeInt, {RESID, Operand{8U}, Operand{0U}});
        case DataType::int16_t:
        case DataType::uint16_t: return module.EmplaceInstruction(OpTypeInt, {RESID, Operand{16U}, Operand{0U}});
        case DataType::int32_t:
        case DataType::uint32_t: return module.EmplaceInstruction(OpTypeInt, {RESID, Operand{32U}, Operand{0U}});
        case DataType::int64_t:
        case DataType::int48_t: return module.EmplaceInstruction(OpTypeInt, {RESID, Operand{64U}, Operand{0U}});
        case DataType::float16_t: return module.EmplaceInstruction(OpTypeFloat, {RESID, Operand{16U}});
        case DataType::float32_t: return module.EmplaceInstruction(OpTypeFloat, {RESID, Operand{32U}});
        case DataType::bfloat16_t: return module.EmplaceInstruction(OpTypeFloat, {RESID, Operand{16U}, Operand{2U}});
        case DataType::bool_t: return module.EmplaceInstruction(OpTypeBool, {RESID});
        case DataType::null_t:
        {
            throw std::runtime_error("Datatype cannot be null_t");
        }
        default: throw std::runtime_error("Unknown Datatype");
    }
}

Operand CreateConstant(const uint32_t value, const DataType dataType, Module& module)
{
    auto constTypeId = CreateDataType(dataType, module);
    if (dataType == DataType::bool_t)
    {
        const auto opConstant = value != 0 ? OpConstantTrue : OpConstantFalse;
        return module.EmplaceInstruction(opConstant, {constTypeId, RESID});
    }
    return module.EmplaceInstruction(OpConstant, {constTypeId, RESID, Operand{value}});
}

Operand CreateConstantDouble(const uint32_t valueLow, const uint32_t valueHigh, Module& module)
{
    auto constTypeId = CreateDataType(DataType::int48_t, module);
    return module.EmplaceInstruction(OpConstant, {constTypeId, RESID, Operand{valueLow}, Operand{valueHigh}});
}

Operand CreateConstantComposite(const std::vector<uint32_t>& array,
                                const Operand& resultType,
                                Module& module,
                                tosa::DataType constituentType,
                                const bool tryReduce)
{
    std::vector operands{resultType, RESID};

    if (array.empty())
    {
        return module.EmplaceInstruction(OpConstantNull, operands);
    }

    if (constituentType == DataType::int48_t)
    {
        auto constTypeId = CreateDataType(DataType::int48_t, module);
        for (auto val = array.begin(); val != array.end();)
        {
            const auto low = *val++;
            const auto high = *val++;
            operands.emplace_back(
                module.EmplaceInstruction(OpConstant, {constTypeId, RESID, Operand{low}, Operand{high}}));
        }
        return module.EmplaceInstruction(OpConstantComposite, operands);
    }

    if (tryReduce)
    {
        const bool uniformValue =
            (std::adjacent_find(array.begin(), array.end(), std::not_equal_to<>()) == array.end());
        if (uniformValue)
        {
            if (array[0] == 0u)
            {
                return module.EmplaceInstruction(OpConstantNull, operands);
            }

            if (array.size() > 1)
            {
                operands.push_back(CreateConstant(array[0], constituentType, module));
                return module.EmplaceInstruction(OpConstantCompositeReplicateEXT, operands);
            }
        }
    }

    for (const auto val : array)
    {
        operands.push_back(CreateConstant(val, constituentType, module));
    }
    return module.EmplaceInstruction(OpConstantComposite, operands);
}

Operand CreateTensor(const Tensor& tensor, Module& module)
{
    const Operand rankValue(static_cast<unsigned int>(tensor.GetTensorShape().size()));
    const auto U32Type = CreateDataType(DataType::uint32_t, module);
    const auto dataType = CreateDataType(tensor.GetDataType(), module);
    const auto tensorRank = module.EmplaceInstruction(OpConstant, {U32Type, RESID, rankValue});
    const auto arrayTypeId = module.EmplaceInstruction(OpTypeArray, {RESID, U32Type, tensorRank});
    const auto TensorShapeId = CreateConstantComposite(tensor.GetTensorShape(), arrayTypeId, module);
    return module.EmplaceInstruction(OpTypeTensorARM, {RESID, dataType, tensorRank, TensorShapeId});
}

Operand CreateAttribute(const Attribute& attribute, Module& module)
{
    const auto tensor = CreateTensor(attribute.GetTensor(), module);
    return CreateConstantComposite(attribute.GetData(), tensor, module, attribute.GetTensor().GetDataType(), true);
}

} // namespace tfsc::spirv
