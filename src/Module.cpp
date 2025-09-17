//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Module.hpp>

#include <algorithm>

namespace tfsc::spirv
{

Operand Module::EmplaceInstruction(const spv::Op opCode, std::vector<Operand> operands)
{
    Instruction inst{opCode, std::move(operands)};
    const auto res = m_SPIRVGraph.find(inst);
    if (res == m_SPIRVGraph.end())
    {
        for (auto& operand : inst.m_Operands)
        {
            if (operand.m_Type == RES_ID)
            {
                operand = Operand{m_ResId++, RES_ID};
                break;
            }
        }
        return Operand{&(*m_SPIRVGraph.emplace(std::move(inst)))};
    }
    return Operand{&(*res)};
}

Operand Module::EmplaceInstructionNonUnique(const spv::Op opCode, std::vector<Operand> operands)
{
    Instruction inst{opCode, std::move(operands)};
    for (auto& operand : inst.m_Operands)
    {
        if (operand.m_Type == RES_ID)
        {
            operand = Operand{m_ResId++, RES_ID};
            break;
        }
    }
    return Operand{&(*m_SPIRVGraph.emplace(std::move(inst)))};
}

} // namespace tfsc::spirv
