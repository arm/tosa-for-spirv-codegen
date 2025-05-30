//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "EnumMaps.hpp"
#include <ModuleComparator.hpp>

#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace testutils
{
using namespace tosa2spirv::spirv;

struct InstructionSets
{
    std::unordered_set<const Instruction*> m_UnMatched_Lhs;
    std::unordered_set<const Instruction*> m_UnMatched_Rhs;
    std::unordered_map<const Instruction*, const Instruction*> m_Matches;
};

bool CompareInstructions(const Instruction* lhs, const Instruction* rhs, InstructionSets& matchingInstructions);

bool CompareOperands(const Operand& lhs, const Operand& rhs, InstructionSets& matchingInstructions)
{
    if (lhs.m_Type != rhs.m_Type)
    {
        return false;
    }
    switch (lhs.m_Type)
    {
        case RES_ID:
        {
            return true;
        }
        case INSTRUCTION_POINTER:
        {
            return CompareInstructions(lhs.m_InstructionPtr, rhs.m_InstructionPtr, matchingInstructions);
        }
        case LITERAL_WORD:
        {
            return lhs.m_LiteralWord == rhs.m_LiteralWord;
        }
        case LITERAL_STRING:
        {
            return *lhs.m_LiteralStr == *rhs.m_LiteralStr;
        }
        case UNINITIALIZED:
        {
            return false;
        }
    }
    return false;
}

bool CompareInstructions(const Instruction* lhs, const Instruction* rhs, InstructionSets& matchingInstructions)
{
    if (const auto it = matchingInstructions.m_Matches.find(lhs);
        it != matchingInstructions.m_Matches.end() && it->second == rhs)
    {
        return true;
    }

    if (lhs->m_Opcode != rhs->m_Opcode)
    {
        return false;
    }
    if (lhs->m_Operands.size() != rhs->m_Operands.size())
    {
        return false;
    }

    bool equal = true;
    auto itRhs = rhs->m_Operands.crbegin();
    for (auto itLhs = lhs->m_Operands.crbegin(); itLhs != lhs->m_Operands.crend(); ++itLhs, ++itRhs)
    {
        if (!CompareOperands(*itLhs, *itRhs, matchingInstructions))
        {
            equal = false;
        }
    }
    if (equal)
    {
        matchingInstructions.m_UnMatched_Lhs.erase(lhs);
        matchingInstructions.m_UnMatched_Rhs.erase(rhs);
        matchingInstructions.m_Matches.emplace(lhs, rhs);
    }
    return equal;
}

std::vector<Instruction> GetSortedOutputInstructions(const std::shared_ptr<Module>& module)
{
    const auto [outputBegin, outputEnd] = module->GetInstructionsOfType(spv::OpGraphSetOutputARM);
    // Sort output operators by their ResIds
    std::vector<Instruction> outputInstructions{outputBegin, outputEnd};
    std::sort(begin(outputInstructions), end(outputInstructions), [](const Instruction& lhs, const Instruction& rhs) {
        return lhs.m_Operands[1].m_InstructionPtr->m_Operands[2].m_LiteralWord <
               rhs.m_Operands[1].m_InstructionPtr->m_Operands[2].m_LiteralWord;
    });
    return outputInstructions;
}

std::string CompareModules(const std::shared_ptr<Module>& lhs, const std::shared_ptr<Module>& rhs)
{
    std::vector<Instruction> lhsOutputs = GetSortedOutputInstructions(lhs);
    std::vector<Instruction> rhsOutputs = GetSortedOutputInstructions(rhs);

    auto lhsIt = lhsOutputs.begin();
    auto rhsIt = rhsOutputs.begin();
    InstructionSets instructionSets;
    std::for_each(lhs->GetSpirvGraph().begin(), lhs->GetSpirvGraph().end(), [&](const Instruction& instruction) {
        instructionSets.m_UnMatched_Lhs.insert(&instruction);
    });
    std::for_each(rhs->GetSpirvGraph().begin(), rhs->GetSpirvGraph().end(), [&](const Instruction& instruction) {
        instructionSets.m_UnMatched_Rhs.insert(&instruction);
    });

    for (; lhsIt != lhsOutputs.end() && rhsIt != rhsOutputs.end(); ++lhsIt, ++rhsIt)
    {
        CompareInstructions(lhsIt->m_Operands[0].m_InstructionPtr,
                            rhsIt->m_Operands[0].m_InstructionPtr,
                            instructionSets);
    }

    std::vector<const Instruction*> remainingLhs = {instructionSets.m_UnMatched_Lhs.begin(),
                                                    instructionSets.m_UnMatched_Lhs.end()};
    std::vector<const Instruction*> remainingRhs = {instructionSets.m_UnMatched_Rhs.begin(),
                                                    instructionSets.m_UnMatched_Rhs.end()};
    for (const auto lhsInstruction : remainingLhs)
    {
        for (const auto rhsInstruction : remainingRhs)
        {
            CompareInstructions(lhsInstruction, rhsInstruction, instructionSets);
        }
    }

    std::stringstream res;
    if (!instructionSets.m_UnMatched_Lhs.empty())
    {
        res << "Instructions in lhs module not found in rhs module:\n";
        for (const auto instruction : instructionSets.m_UnMatched_Lhs)
        {
            res << "    " << InstructionToString(*instruction) << "\n";
        }
        res << std::endl;
    }

    if (!instructionSets.m_UnMatched_Rhs.empty())
    {
        res << "Instructions in rhs module not found in lhs module:\n";
        for (const auto instruction : instructionSets.m_UnMatched_Rhs)
        {
            res << "    " << InstructionToString(*instruction) << "\n";
        }
        res << std::endl;
    }
    return res.str();
}

} // namespace testutils
