//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <EnumMaps.hpp>
#include <Instruction.hpp>
#include <OperatorDefinitions.hpp>
#include <Tensor.hpp>
#include <TosaOperator.hpp>

#include <stdexcept>

namespace spirv2tosa
{

/// Parses a tosa2spirv::tosa::DataType from a SPIR-V OpTypeInt, OpTypeFloat or OpTypeBool instruction
tosa2spirv::tosa::DataType GetDataTypeFromInstruction(const tosa2spirv::spirv::Instruction& typeInstruction);

/// Parses a tosa2spirv::tosa tensor shape vector from a SPIR-V OpConstantCompositeReplicateEXT or OpConstantComposite
/// instruction
std::vector<uint32_t> GetTensorShapeFromInstruction(const tosa2spirv::spirv::Instruction& shapeInstruction);

/// Parses a tosa2spirv::tosa::Tensor from a SPIR-V OpTypeTensorARM instruction
tosa2spirv::tosa::Tensor GetTensorFromInstruction(const tosa2spirv::spirv::Instruction& tensorInstruction);

/// Parses a tosa2spirv::tosa::Attribute from a SPIR-V constant instruction with a type of OpTypeTensorARM
/// (OpConstantTrue/OpConstantFalse/OpConstant/OpConstantNull/OpConstantComposite/OpConstantCompositeReplicateEXT)
tosa2spirv::tosa::Attribute GetAttributeFromInstruction(const tosa2spirv::spirv::Instruction& attributeInstruction);

/// Converts an OpExtInst tosa2spirv::spirv::Instruction instance to a TosaOperator instance, describing the operator
/// found in the given instruction
TosaOperator GetTosaOperator(const tosa2spirv::spirv::Instruction& instruction);

} // namespace spirv2tosa
