//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Writer.hpp>

#include <spirv/unified1/spirv.hpp>

#include <string>
#include <vector>


namespace tosa2spirv
{

void WriteBoilerplate(spirvwriter::Writer& writer);

void WriteBoilerplateExtended(spirvwriter::Writer& writer,
                              std::vector<spv::Capability>* capabilityTypeList = nullptr);


std::string GetPreSPIRVStr();


// This method is the same as Tensor::ConvertValuesToUint32t but is visible for tests as the original is private.
template <typename T>
std::shared_ptr<std::vector<uint32_t>> ConvertValuesToUint32t(const std::vector<T>& values,
                                                              tosa2spirv::graphbuilder::DataType originalDatatype)
{
    auto outputVector = std::make_shared<std::vector<uint32_t>>();

    for (auto value : values)
    {
        if (originalDatatype == tosa2spirv::graphbuilder::DataType ::float32_t ||
            originalDatatype == tosa2spirv::graphbuilder::DataType ::float16_t ||
            originalDatatype == tosa2spirv::graphbuilder::DataType ::bfloat16_t)
        {
            // All float values are stored as 32-bit float data in m_TensorValuePtr.
            // Then float and unsigned int has the same width, so we can change the type of the pointer
            // static_cast float values to unsigned int will change its value
            outputVector->push_back(*reinterpret_cast<uint32_t*>(&value));
        }
        else
        {
            // For int8, int16, int32, and bool
            outputVector->push_back(static_cast<uint32_t>(value));
        }
    }
    return outputVector;
}

namespace spirvwriter
{

std::string DisassembleSPIRV(const std::vector<uint32_t>& binary, bool runValidation = true);

} // spirvwriter namespace

} // tosa2spirv namespace