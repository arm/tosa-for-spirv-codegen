//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <SPIRVDefinitions.hpp>

#include <algorithm>
#include <stdexcept>

namespace tosa2spirv::spirv
{

using namespace spv;
using namespace tosa2spirv::tosa;

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
        case DataType::uint32_t:
        case DataType::int64_t: return module.EmplaceInstruction(OpTypeInt, {RESID, Operand{32U}, Operand{0U}});
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
        const auto opConstant = value == 1 ? OpConstantTrue : OpConstantFalse;
        return module.EmplaceInstruction(opConstant, {constTypeId, RESID});
    }
    return module.EmplaceInstruction(OpConstant, {constTypeId, RESID, Operand{value}});
}

Operand CreateConstantDouble(const uint32_t valueLow, const uint32_t valueHigh, Module& module)
{
    auto constTypeId = CreateDataType(DataType::int48_t, module);
    return module.EmplaceInstruction(OpConstant, {constTypeId, RESID, Operand{valueLow}, Operand{valueHigh}});
}

Operand CreateConstantCompositeDouble(const std::vector<uint32_t>& array, const Operand& typeId, Module& module)
{
    std::vector operands{typeId, RESID};
    for (auto val = array.begin(); val != array.end();)
    {
        const auto low = *val++;
        const auto high = *val++;
        operands.push_back(CreateConstantDouble(low, high, module));
    }
    return module.EmplaceInstruction(OpConstantComposite, operands);
}

Operand
CreateConstantComposite(const std::vector<uint32_t>& array, const Operand& typeId, Module& module, const bool tryReduce)
{
    std::vector operands{typeId, RESID};
    if (tryReduce)
    {
        const auto uniformValue =
            std::all_of(array.cbegin(), array.cend(), [&](const uint32_t value) { return value == array[0]; });
        if (uniformValue)
        {
            if (array.empty() || array[0] == 0)
            {
                return module.EmplaceInstruction(OpConstantNull, operands);
            }

            operands.push_back(CreateConstant(array[0], DataType::uint32_t, module));
            return module.EmplaceInstruction(OpConstantCompositeReplicateEXT, operands);
        }
    }

    for (const auto val : array)
    {
        operands.push_back(CreateConstant(val, DataType::uint32_t, module));
    }
    return module.EmplaceInstruction(OpConstantComposite, operands);
}

Operand CreateConstantCompositeTyped(const std::vector<uint32_t>& array,
                                     const Operand& typeId,
                                     Module& module,
                                     const DataType type)
{
    std::vector<Operand> operands{typeId, RESID};
    const auto uniformValue =
        std::all_of(array.cbegin(), array.cend(), [&](const uint32_t value) { return value == array[0]; });
    if (uniformValue)
    {
        if (array.empty() || array[0] == 0)
        {
            return module.EmplaceInstruction(OpConstantNull, {typeId, RESID});
        }

        const auto constant = CreateConstant(array[0], type, module);
        return module.EmplaceInstruction(OpConstantCompositeReplicateEXT, {typeId, RESID, constant});
    }

    const auto scalarType = CreateDataType(type, module);
    for (const auto val : array)
    {
        // Create Constant value instruction for scalar value inside the vector e.g the 2 in {2,2}.
        operands.push_back(module.EmplaceInstruction(OpConstant, {scalarType, RESID, Operand{val}}));
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
    const auto TensorShapeId = CreateConstantComposite(tensor.GetTensorShape(), arrayTypeId, module, false);
    return module.EmplaceInstruction(OpTypeTensorARM, {RESID, dataType, tensorRank, TensorShapeId});
}

Operand CreateAttribute(const Attribute& attribute, Module& module)
{
    const auto tensor = CreateTensor(attribute.GetTensor(), module);
    if (attribute.GetTensor().GetDataType() == DataType::int48_t)
    {
        return CreateConstantCompositeDouble(attribute.GetData(), tensor, module);
    }
    return CreateConstantCompositeTyped(attribute.GetData(), tensor, module, attribute.GetTensor().GetDataType());
}

} // namespace tosa2spirv::spirv
