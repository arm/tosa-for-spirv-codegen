//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include <Module.hpp>
#include <Tensor.hpp>

namespace tosa2spirv::spirv
{

// Definitions of SPIR-V instructions used to make the TOSA graph
Operand CreateDataType(tosa::DataType datatype, Module& module);
Operand CreateConstant(uint32_t value, tosa::DataType dataType, Module& module);
Operand CreateConstantCompositeBasic(const std::vector<uint32_t>& array, const Operand& typeId, Module& module);
Operand CreateConstantComposite(const std::vector<uint32_t>& array, const Operand& typeId, Module& module);
Operand CreateTensor(const tosa::Tensor& tensor, Module& module);
Operand CreateAttribute(const tosa::Attribute& attribute, Module& module);

} // namespace tosa2spirv::spirv