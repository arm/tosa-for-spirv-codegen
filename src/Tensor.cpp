//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Tensor.hpp>
#include <TensorInfo.hpp>
#include <Module.hpp>

#include <numeric>
#include <functional>
#include <utility>

namespace tosa2spirv::graphbuilder
{

Tensor::Tensor(DataType dataType,
               TensorShape shape,
               ResId id,
               bool isGraphOutput,
               const std::vector<uint32_t>& tensorValuePtr)
    : m_TensorAttributes(tensorValuePtr)
{
    // Handle rank 0 tensors
    if (shape.empty())
    {
        shape = { 1 };
    }

    m_TensorInfo = std::make_unique<TensorInfo>(dataType, shape, id, isGraphOutput, !tensorValuePtr.empty());
}

Tensor::Tensor(const Tensor& other) noexcept
{
    this->m_TensorInfo = std::make_unique<TensorInfo>(*other.m_TensorInfo);
    this->m_TensorAttributes = other.m_TensorAttributes;
}

Tensor::Tensor(Tensor&& other) noexcept{
    this->m_TensorInfo = std::move(other.m_TensorInfo);
    this->m_TensorAttributes = std::move(other.m_TensorAttributes);
}

Tensor& Tensor::operator= (const Tensor& other) {
    if(this != &other)
    {
        this->m_TensorInfo = std::make_unique<TensorInfo>(*other.m_TensorInfo);
        this->m_TensorAttributes = other.m_TensorAttributes;
    }
    return *this;
}

Tensor Tensor::CreateAttribute(const DataType &dataType, const TensorShape &shape)
{
    return {dataType, shape};
}

Tensor Tensor::CreateAttribute(const DataType &dataType,
                               const TensorShape& shape,
                               const std::vector<uint32_t>& tensorValuePtr)
{
    return {dataType, shape, 0, false, tensorValuePtr};
}

Tensor Tensor::CreateInput(const DataType &dataType,
                           const TensorShape &shape,
                           ModulePtrRef module,
                           const std::string &name)
{
    auto input = Tensor(dataType, shape);

    // Check if this is input to multiple outputs or not.
    if (module->DescSetBindingExists(name))
    {
        input.SetDescSetBinding(module->GetDescSetBindingByName(name));
    }
    else
    {
        // Add a descriptor set binding and assign it to the created input tensor
        DescSetBinding& input_tensor_ptr = module->AddDescSetBindingPair(0, module->GetNextAvailableBinding(0), name);
        input.SetDescSetBinding(input_tensor_ptr);
    }
    return input;
}

Tensor Tensor::CreateInput(const DataType &dataType,
                           const TensorShape &shape,
                           ModulePtrRef module,
                           const std::pair<const std::string, uint32_t>& nameBindingPair)
{
    auto input = Tensor(dataType, shape);

    // Check if this is input to multiple outputs or not.
    if (module->DescSetBindingExists(nameBindingPair.first))
    {
        input.SetDescSetBinding(module->GetDescSetBindingByName(nameBindingPair.first));
    }
    else
    {
        // Add a descriptor set binding and assign it to the created input tensor
        DescSetBinding& input_tensor_ptr = module->AddDescSetBindingPair(0, nameBindingPair.second, nameBindingPair.first);
        input.SetDescSetBinding(input_tensor_ptr);
    }
    return input;
}

Tensor Tensor::CreateOutput(const DataType &dataType,
                            const TensorShape &shape,
                            ModulePtrRef module,
                            const std::string &name)
{
    auto output = Tensor(dataType, shape, 0, true);
    if (module->DescSetBindingExists(name))
    {
        output.SetDescSetBinding(module->GetDescSetBindingByName(name));
    }
    else
    {
        // Add a descriptor set binding and assign it to the created output tensor
        DescSetBinding& output_tensor_ptr = module->AddDescSetBindingPair(0,
                                                                          module->GetNextAvailableBinding(0),
                                                                          name);
        output.SetDescSetBinding(output_tensor_ptr);
    }
    return output;
}

Tensor Tensor::CreateOutput(const DataType &dataType,
                            const TensorShape &shape,
                            ModulePtrRef module,
                            const std::pair<const std::string, uint32_t>& nameBindingPair)
{
    auto output = Tensor(dataType, shape, 0, true);
    if (module->DescSetBindingExists(nameBindingPair.first))
    {
        output.SetDescSetBinding(module->GetDescSetBindingByName(nameBindingPair.first));
    }
    else
    {
        // Add a descriptor set binding and assign it to the created output tensor
        DescSetBinding& output_tensor_ptr = module->AddDescSetBindingPair(0,
                                                                          nameBindingPair.second,
                                                                          nameBindingPair.first);
        output.SetDescSetBinding(output_tensor_ptr);
    }
    return output;
}

Tensor Tensor::CreateInputFromLayer(const DataType &dataType, const TensorShape &shape, const ResId &id)
{
    return {dataType, shape, id, false};
}

Tensor Tensor::CreateOutputToLayer(const DataType &dataType, const TensorShape &shape)
{
    return {dataType, shape, 0, false};
}

const DataType& Tensor::GetDataType() const
{
    return m_TensorInfo->m_DataType;
}

const Tensor::TensorShape& Tensor::GetTensorShape() const
{
    return m_TensorInfo->m_Shape;
}

const ResId& Tensor::GetResId() const
{
    return m_TensorInfo->m_ResId;
}

bool Tensor::IsGraphOutput() const
{
    return m_TensorInfo->m_IsGraphOutput;
}

unsigned int Tensor::GetNumElements() const
{
    return std::accumulate(std::begin(m_TensorInfo->m_Shape),
                           std::end(m_TensorInfo->m_Shape),
                           1,
                           std::multiplies<>());
}

std::vector<uint32_t> Tensor::GetTensorValuesUint32t() const
{
    return m_TensorAttributes;
}

bool Tensor::HasValue() const
{
    return m_TensorInfo->m_HasValue;
}

Category Tensor::GetCategory() const
{
    return m_TensorInfo->m_Category;
}

void Tensor::SetCategory(const Category category)
{
    m_TensorInfo->m_Category = category;
}

Metadata Tensor::GetMetadata() const
{
    return m_TensorInfo->m_Metadata;
}

void Tensor::SetMetadata(const Metadata metadata)
{
    m_TensorInfo->m_Metadata = metadata;
}

template <typename T>
std::vector<uint32_t> Tensor::ConvertValuesToUint32t(const std::vector<T>& values,
                                                     DataType originalDatatype,
                                                     bool signless)
{
    std::vector<uint32_t> outputVector;

    for (auto value : values)
    {
        if (originalDatatype == DataType::float32_t ||
            originalDatatype == DataType::float16_t ||
            originalDatatype == DataType::bfloat16_t)
        {
            // All float values are stored as 32-bit float data in m_TensorValuePtr.
            // Then float and unsigned int has the same width, so we can change the type of the pointer
            // static_cast float values to unsigned int will change its value
            outputVector.push_back(*reinterpret_cast<uint32_t*>(&value));
        }
        else
        {
            // tosa2spirv uses signless integers where the high-order bits are zero
            // https://mlir.llvm.org/docs/Rationale/Rationale/#integer-signedness-semantics
            int intValue = static_cast<int>(value);

            // For int8, int16, int32, and bool
            if (signless)
            {
                outputVector.push_back(static_cast<uint32_t>(intValue & 0xFF));
            }
            else
            {
                outputVector.push_back(static_cast<uint32_t>(intValue));
            }
        }
    }
    return outputVector;
}

std::vector<uint32_t> Tensor::ConvertInt32tToUint32t(const std::vector<int32_t>& values, bool signless)
{
    return ConvertValuesToUint32t<int32_t>(values, DataType::int32_t, signless);
}

std::vector<uint32_t> Tensor::ConvertInt16tToUint32t(const std::vector<int16_t>& values, bool signless)
{
    return ConvertValuesToUint32t<int16_t>(values, DataType::int16_t, signless);
}

std::vector<uint32_t> Tensor::ConvertInt8tToUint32t(const std::vector<int8_t>& values, bool signless)
{
    return ConvertValuesToUint32t<int8_t>(values, DataType::int8_t, signless);
}

std::vector<uint32_t> Tensor::ConvertFloat32ToUint32t(const std::vector<float>& values)
{
    return ConvertValuesToUint32t<float>(values, DataType::float32_t, false);
}

std::vector<uint32_t> Tensor::ConvertFloat16ToUint32t(const std::vector<float>& values)
{
    return ConvertValuesToUint32t<float>(values, DataType::float16_t, false);
}

std::vector<uint32_t> Tensor::ConvertBoolToUint32t(const std::vector<bool>& values)
{
    return ConvertValuesToUint32t<bool>(values, DataType::bool_t, false);
}

DescSetBinding* Tensor::GetDescSetBinding() const { return m_TensorInfo->m_DescSetBinding; }

void Tensor::SetDescSetBinding(DescSetBinding& pair) { m_TensorInfo->m_DescSetBinding = &pair; }

Tensor::~Tensor() = default;

} // namespace graphbuilder::tosa2spirv