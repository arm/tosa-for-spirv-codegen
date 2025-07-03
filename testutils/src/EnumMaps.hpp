//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Instruction.hpp>
#include <OperatorEnum.hpp>

#include <spirv/unified1/TOSA.001000.1.h>
#include <spirv/unified1/spirv.hpp>
#include <string>

std::string GetOpString(const spv::Op op);
std::string InstructionToString(const tosa2spirv::spirv::Instruction &instruction);

TOSAInstructions GetTosaOpEnum(const std::string &instruction);
tosa2spirv::tosa::OperatorEnum GetOperatorEnum(TOSAInstructions instructionType);
spv::Capability GetCapabilityEnum(const std::string &instruction);
spv::Op GetOpEnum(const std::string &instruction);

unsigned int GetResultIdPosition(spv::Op op);
unsigned int GetResId(const tosa2spirv::spirv::Instruction &instruction);
