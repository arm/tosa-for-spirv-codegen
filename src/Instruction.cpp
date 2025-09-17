//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Module.hpp>

#include <algorithm>

namespace tfsc::spirv
{

bool InstructionComparator::operator()(const Instruction& lhs, const spv::Op rhs) const noexcept
{
    return lhs.m_Opcode < rhs;
}

bool InstructionComparator::operator()(const spv::Op lhs, const Instruction& rhs) const noexcept
{
    return lhs < rhs.m_Opcode;
}

bool InstructionComparator::operator()(const Instruction& lhs, const Instruction& rhs) const noexcept
{
    if (lhs.m_Opcode != rhs.m_Opcode)
    {
        return lhs.m_Opcode < rhs.m_Opcode;
    }
    if (lhs.m_Operands.size() != rhs.m_Operands.size())
    {
        return lhs.m_Operands.size() < rhs.m_Operands.size();
    }
    auto itRhs = rhs.m_Operands.crbegin();
    for (auto itLhs = lhs.m_Operands.crbegin(); itLhs != lhs.m_Operands.crend(); ++itLhs, ++itRhs)
    {
        if (itLhs->m_Type == RES_ID)
        {
            continue;
        }
        if (*itLhs != *itRhs)
        {
            return *itLhs < *itRhs;
        }
    }
    return false;
}

} // namespace tfsc::spirv
