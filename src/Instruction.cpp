//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "Instruction.hpp"

#include <iostream>
#include <utility>

namespace tosa2spirv
{

namespace spirvwriter
{

Instruction::Instruction(spv::Op opCode, graphbuilder::ResId id)
    : m_Opcode(static_cast<uint16_t>(opCode))
{
    SetResult(id);
}

Instruction::Instruction(spv::Op opCode, std::vector<Operand>& ops)
    : Instruction(opCode)
{
    SetOperands(ops);
}

Instruction::Instruction(spv::Op opCode, graphbuilder::ResId id, std::vector<Operand>& opVec)
    : Instruction(opCode, id)
{
    if (opCode == spv::OpTypeBool)
    {
        return;
    }

    SetOperands(opVec);
}

void Instruction::SetResult(graphbuilder::ResId id)
{
    m_WordCount = 1 + (id.IsValid() ? 1 : 0);
    m_ResultId = id;
}

void Instruction::SetOperands(std::vector<Operand> opVec)
{
    if (opVec.empty())
    {
        std::cerr << "Instruction(): Vector size must be > 0." << std::endl;
        throw std::exception();
    }

    m_Operands = std::move(opVec);
    for (auto& opd : m_Operands)
    {
        m_WordCount += uint16_t(opd.GetNumWords());
    }
}

} // spirvwriter namespace

} // tosa2spirv namespace