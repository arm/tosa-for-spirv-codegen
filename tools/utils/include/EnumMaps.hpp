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

std::string OpToString(const spv::Op op);
TOSAInstructions GetTosaOpEnum(const std::string &instruction);
tfsc::tosa::OperatorEnum GetOperatorEnum(TOSAInstructions instructionType);
spv::Capability GetCapabilityEnum(const std::string &instruction);
spv::Op GetOpEnum(const std::string &instruction);
unsigned int GetResultIdPosition(spv::Op op);
unsigned int GetResId(const tfsc::spirv::Instruction &instruction);
