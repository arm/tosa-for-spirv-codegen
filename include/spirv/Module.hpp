//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "Instruction.hpp"

#include <algorithm>
#include <set>

// tosa-for-spirv-codegen's shorthand namespace
namespace tfsc::spirv
{
/// tosa-for-spirv-codegen's implementation of SPIR-V module.
class Module
{
    public:
    /// Conditionally emplace an Instruction into the SPIR-V module
    /// Function moves the given operands into an Instruction and adds it to the module if
    /// no matching function is found within the Module
    /// The ResId of an Instruction will only be assigned once it has been successfully placed into the Module
    /// The operands vector is moved and must not be reused after this call.
    /// @param[in] opCode opcode of the Instruction
    /// @param[in] operands operands of the instruction, ownership is moved.
    /// @return An operand pointing to either the newly created Instruction or
    /// to a matching Instruction already existing in the Module
    Operand EmplaceInstruction(spv::Op opCode, std::vector<Operand> operands);

    /// Emplace an Instruction into the SPIR-V module
    /// Function will always create a new Instruction in the Module, moving the provided operands into it.
    /// The operands vector is moved and must not be reused after this call.
    /// @param[in] opCode opcode of the Instruction
    /// @param[in] operands operands of the instruction, ownership is moved.
    /// @return An operand pointing the newly created Instruction
    Operand EmplaceInstructionNonUnique(spv::Op opCode, std::vector<Operand> operands);

    using InstructionIterator = std::multiset<Instruction, InstructionComparator>::const_iterator;
    using InstructionRange = std::pair<InstructionIterator, InstructionIterator>;

    /// Get all Instructions of a given opCode from the Module
    /// @param opCode opCode to match
    /// @return An iterator range of those Instructions
    InstructionRange GetInstructionsOfType(const spv::Op opCode) const
    {
        auto begin = m_SPIRVGraph.begin();
        auto end = m_SPIRVGraph.end();

        // Find the first instruction with a matching opcode
        auto lower = std::find_if(begin, end, [&](const Instruction& instr) { return instr.GetOpCode() == opCode; });

        if (lower == end)
        {
            // No matching instruction found
            return {end, end};
        }

        // Finds the first instruction after the matching block (with a different opcode)
        auto upper = std::find_if(lower, end, [&](const Instruction& instr) { return instr.GetOpCode() != opCode; });

        // Returns a range of all instructions with the given opcode
        return {lower, upper};
    }

    /// Get spirv graph, containing all Instructions in the module
    const std::multiset<Instruction, InstructionComparator>& GetSpirvGraph() const { return m_SPIRVGraph; }

    private:
    uint32_t m_ResId = 1;
    std::multiset<Instruction, InstructionComparator> m_SPIRVGraph;
};

/// Operand Instance used to specify the position of an Instructions ResId
static const auto RESID = Operand{0u, RES_ID};

} // namespace tfsc::spirv