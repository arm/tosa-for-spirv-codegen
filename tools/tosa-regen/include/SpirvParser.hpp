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

namespace tosaregen
{

/// Parses a tfsc::tosa::DataType from a SPIR-V OpTypeInt, OpTypeFloat or OpTypeBool instruction
tfsc::tosa::DataType GetDataTypeFromInstruction(const tfsc::spirv::Instruction& typeInstruction);

/// Parses a tfsc::tosa tensor shape vector from a SPIR-V OpConstantCompositeReplicateEXT or OpConstantComposite
/// instruction
std::vector<uint32_t> GetTensorShapeFromInstruction(const tfsc::spirv::Instruction& shapeInstruction);

/// Parses a tfsc::tosa::Tensor from a SPIR-V OpTypeTensorARM instruction
tfsc::tosa::Tensor GetTensorFromInstruction(const tfsc::spirv::Instruction& tensorInstruction);

/// Parses a tfsc::tosa::Attribute from a SPIR-V constant instruction with a type of OpTypeTensorARM
/// (OpConstantTrue/OpConstantFalse/OpConstant/OpConstantNull/OpConstantComposite/OpConstantCompositeReplicateEXT)
tfsc::tosa::Attribute GetAttributeFromInstruction(const tfsc::spirv::Instruction& attributeInstruction);

/// Converts an OpExtInst tfsc::spirv::Instruction instance to a TosaOperator instance, describing the operator
/// found in the given instruction
TosaOperator GetTosaOperator(const tfsc::spirv::Instruction& instruction);

} // namespace tosaregen
