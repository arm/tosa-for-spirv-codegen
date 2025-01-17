//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <GraphInfo.hpp>
#include <Operand.hpp>
#include <Operator.hpp>
#include <Tensor.hpp>
#include <Writer.hpp>

#include <spirv/unified1/TOSA.000080.2.h>

namespace tosa2spirv::graphbuilder
{

// Validates the tensors of the operator and adds any needed additional information (e.g. if a tensor is a shape type)
void ValidateAndAnnotateOp(Operator& op);

spirvwriter::Operand AddLayerInputInstruction(spirvwriter::Writer& writer,
                                              GraphInfo& graphInfo,
                                              const Tensor& inputTensor,
                                              spirvwriter::InstructionList& layerInstList,
                                              const std::string& prefix,
                                              ResId layerId);

void SetBindings(const Tensor& tensor, uint32_t pointerTypeId, spirvwriter::Writer& writer);

void HandleGraphOutput(spirvwriter::Writer& writer,
                       const ResId& layerId,
                       GraphInfo& graphInfo,
                       const Tensor& outputTensor,
                       spirvwriter::Operand& outputTensorTypeOp,
                       spirvwriter::InstructionList& layerInstList,
                       const std::string& prefix);

/// Helper function to handle the Graph input for Layer::Write() method
spirvwriter::Operand HandleGraphInput(spirvwriter::Writer& writer,
                                      const ResId& layerId,
                                      GraphInfo& graphInfo,
                                      const Tensor& inputTensor,
                                      spirvwriter::InstructionList& layerInstList,
                                      const std::string& prefix);

spirvwriter::Operand HandleAttribute(spirvwriter::Writer& writer, const Tensor& outputTensor);

TOSAInstructions GetTOSAInstructions(OperatorEnum operatorType);

}// namespace graphbuilder::tosa2spirv
