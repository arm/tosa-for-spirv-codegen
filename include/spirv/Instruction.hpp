//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "Operand.hpp"

#include <spirv/unified1/spirv.hpp>

#include <vector>

// tosa-for-spirv-codegen's shorthand namespace
namespace tfsc::spirv
{
/// tosa-for-spirv-codegen's implementation of SPIR-V Instruction.
class Instruction
{
    public:
    Instruction() = default;
    Instruction(const spv::Op opCode, const std::vector<Operand>& operands)
        : m_Opcode(opCode)
        , m_Operands(operands)
    {
        for (const Operand& operand : m_Operands)
        {
            m_WordCount += operand.WordSize();
        }
    }

    Instruction(const spv::Op opCode, std::vector<Operand>&& operands)
        : m_Opcode(opCode)
        , m_Operands(std::move(operands))
    {
        for (const Operand& operand : m_Operands)
        {
            m_WordCount += operand.WordSize();
        }
    }

    /// Getter for m_Opcode.
    /// @return spv::Op
    spv::Op GetOpCode() const { return m_Opcode; }

    /// m_Opcode denotes instruction type
    spv::Op m_Opcode{};
    /// m_WordCount is the total size of the Instruction in uint32_t words
    uint32_t m_WordCount{1};
    /// m_Operands are the operands can constitute the Instruction
    std::vector<Operand> m_Operands;
};

/// Custom Instruction comparator, used to sort Instructions and define when an Instruction is unique
/// Also enables partial sorts of the Instructions by opCode
struct InstructionComparator
{
    // mark comparator as transparent, allowing for equal range search
    using is_transparent = void;
    bool operator()(const Instruction& lhs, spv::Op rhs) const noexcept;
    bool operator()(spv::Op lhs, const Instruction& rhs) const noexcept;
    bool operator()(const Instruction& lhs, const Instruction& rhs) const noexcept;
};

} // namespace tfsc::spirv