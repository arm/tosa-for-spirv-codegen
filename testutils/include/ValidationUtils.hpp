//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Instruction.hpp>
#include <Tensor.hpp>

#include <optional>
#include <sstream>

using namespace tosa2spirv;

void CheckDataType(const spirv::Instruction* instr, tosa::DataType expectedDataType);

void CheckResID(const spirv::Operand& operand);

void CheckConstant(const spirv::Instruction* instruction,
                   const tosa::DataType& expectedDataType,
                   std::optional<uint32_t> expectedValue);

void CheckConstantComposite(const spirv::Instruction* instruction,
                            const std::vector<uint32_t>& expectedValues,
                            tosa::DataType expectedType);

void CheckTensorType(const spirv::Instruction* instruction,
                     tosa::DataType expectedDataType,
                     uint32_t expectedRank,
                     const std::vector<uint32_t>& expectedShape);