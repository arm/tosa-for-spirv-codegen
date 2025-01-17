//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Operand.hpp>
#include <ResId.hpp>

#include <spirv/unified1/spirv.hpp>

#include <cstdint>
#include <list>
#include <vector>

namespace tosa2spirv
{

namespace spirvwriter
{

/// Class which holds the word count, operand code, result ID and list of operands.
class Instruction
{
public:
    /// Primary constructor which must have Opcode.
    /// The WordCount is initialized based on result ID.
    /// @param[in] opCode SPIR-V operator code to initialise.
    /// @param[in] id     Result ID to initialise.
    Instruction(spv::Op opCode, graphbuilder::ResId id = 0);

    /// Creates an instruction with an opcode, list of operands and no result ID.
    /// This calls primary constructor to initialize Opcode and WordCount.
    /// This takes ownership of the operands.
    /// @param[in] opCode   SPIR-V operator code to initialise.
    /// @param[in] operands Vector of operands to be initialised.
    Instruction(spv::Op opCode, std::vector<Operand>& operands);

    /// Creates an instruction with an opcode, result ID and list of operands.
    /// This calls primary constructor to initialize Opcode and WordCount.
    /// This takes ownership of the operands.
    /// @param[in] opCode   SPIR-V operator code to initialise.
    /// @param[in] id       Result ID to initialise.
    /// @param[in] operands Vector of operands to be initialised.
    Instruction(spv::Op opCode, graphbuilder::ResId id, std::vector<Operand>& operands);

    /// Return the word count to the user.
    uint32_t GetWordCount() const { return m_WordCount; }

    /// Return the operator code to the user.
    uint16_t GetOpcode() const { return m_Opcode; }

    /// Return the result ID to the user.
    graphbuilder::ResId GetResultId() const { return m_ResultId; }

    /// Return the vector of operands to the user.
    const std::vector<Operand>& GetOperands() const { return m_Operands; }

private:
    void SetResult(graphbuilder::ResId id = 0);
    void SetOperands(std::vector<Operand> opVec);

    uint32_t m_WordCount;
    uint16_t m_Opcode;
    graphbuilder::ResId m_ResultId;
    std::vector<Operand> m_Operands;
};

typedef std::list<Instruction> InstructionList;

} // spirvwriter namespace

} // tosa2spirv namespace