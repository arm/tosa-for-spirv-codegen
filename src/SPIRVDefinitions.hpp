//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include <Module.hpp>
#include <Tensor.hpp>

namespace tfsc::spirv
{

// Definitions of SPIR-V instructions used to make the TOSA graph
Operand CreateDataType(tosa::DataType datatype, Module& module);
Operand CreateConstant(uint32_t value, tosa::DataType dataType, Module& module);
Operand CreateConstantDouble(uint32_t valueLow, uint32_t valueHigh, Module& module);
Operand CreateConstantComposite(const std::vector<uint32_t>& array,
                                const Operand& typeId,
                                Module& module,
                                tosa::DataType type = tosa::DataType::int32_t,
                                bool reduce = false);
Operand CreateTensor(const tosa::Tensor& tensor, Module& module);
Operand CreateAttribute(const tosa::Attribute& attribute, Module& module);

} // namespace tfsc::spirv
