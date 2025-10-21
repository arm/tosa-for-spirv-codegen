//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <ModuleComparator.hpp>
#include <TosaForSpirvCodegen.hpp>

#include "AdjacencyMatrix.hpp"
#include "AssemblyUtils.hpp"
#include "CustomInstructionRules.hpp"
#include "Instruction.hpp"
#include "ProblemTree.hpp"
#include "StringUtils.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace testutils
{
using namespace tfsc::spirv;

std::string CompareInstructionsOfType(const spv::Op op,
                                      const std::shared_ptr<Module>& lhsModule,
                                      const std::shared_ptr<Module>& rhsModule)
{

    const auto [lhsBegin, lhsEnd] = lhsModule->GetInstructionsOfType(op);
    const auto [rhsBegin, rhsEnd] = rhsModule->GetInstructionsOfType(op);

    std::set<const Instruction*> lhsInstructions;
    std::for_each(lhsBegin, lhsEnd, [&](const Instruction& instruction) { lhsInstructions.insert(&instruction); });
    std::set<const Instruction*> rhsInstructions;
    std::for_each(rhsBegin, rhsEnd, [&](const Instruction& instruction) { rhsInstructions.insert(&instruction); });

    std::stringstream diff;

    return diff.str();
}

std::string CompareModuleHeaders(const std::shared_ptr<Module>& lhsModule, const std::shared_ptr<Module>& rhsModule)
{
    std::stringstream diff;
    diff << CompareInstructionsOfType(spv::OpCapability, lhsModule, rhsModule);
    diff << CompareInstructionsOfType(spv::OpExtension, lhsModule, rhsModule);
    diff << CompareInstructionsOfType(spv::OpExtInstImport, lhsModule, rhsModule);
    diff << CompareInstructionsOfType(spv::OpMemoryModel, lhsModule, rhsModule);
    std::string out;
    if (!diff.str().empty())
    {
        out += "Difference found in Module headers:\n";
        out += boundary;
        out += diff.str();
        out += boundary + "\n";
    }
    return out;
}

static std::unique_ptr<CustomRegistry> customRegistry;

CustomRegistry* GetCustomRegisry()
{
    if (!customRegistry)
    {
        customRegistry = std::make_unique<CustomRegistry>();
    }

    return customRegistry.get();
}

std::optional<ProblemTree> CompareInstructions(const Instruction& lhs, const Instruction& rhs, const int depth)
{
    auto customRule = GetCustomRegisry()->m_rules.find({lhs.m_Opcode, rhs.m_Opcode});
    if (customRule != GetCustomRegisry()->m_rules.end())
    {
        return customRule->second(lhs, rhs, depth);
    }

    if (lhs.m_Opcode != rhs.m_Opcode || lhs.m_Operands.size() != rhs.m_Operands.size())
    {
        return ProblemTree(&lhs, &rhs, depth, true);
    }

    bool isProblem = false;
    std::vector<std::pair<Operand, Operand>> problemOperands;
    std::vector<ProblemTree> leafs;
    for (auto itRhs = rhs.m_Operands.crbegin(), itLhs = lhs.m_Operands.crbegin(); itLhs != lhs.m_Operands.crend();
         ++itLhs, ++itRhs)
    {
        if (itLhs->m_Type != itRhs->m_Type)
        {
            isProblem = true;
            continue;
        }

        switch (itLhs->m_Type)
        {
            case RES_ID: break;
            case INSTRUCTION_POINTER:
            {
                auto leaf = !isAdjacencyMatrixOp(itLhs->m_InstructionPtr->m_Opcode)
                                ? CompareInstructions(*itLhs->m_InstructionPtr, *itRhs->m_InstructionPtr, depth + 1)
                                : std::nullopt;
                if (leaf)
                {
                    leafs.push_back(leaf.value());
                }
            }
            break;
            case LITERAL_WORD:
            {
                if (itLhs->m_LiteralWord != itRhs->m_LiteralWord)
                {
                    isProblem = true;
                    problemOperands.emplace_back(*itLhs, *itRhs);
                }
            }
            break;
            case LITERAL_STRING:
            {
                if (*itLhs->m_LiteralStr != *itRhs->m_LiteralStr)
                {
                    isProblem = true;
                    problemOperands.emplace_back(*itLhs, *itRhs);
                }
            }
            break;
            case UNINITIALIZED:
            {
                isProblem = true;
            }
            break;
        }
    }

    return (isProblem || !leafs.empty())
               ? std::make_optional<ProblemTree>(&lhs, &rhs, depth, isProblem, leafs, problemOperands)
               : std::nullopt;
}

std::string CompareModules(const std::shared_ptr<Module>& lhs, const std::shared_ptr<Module>& rhs, ComparatorOptions co)
{
    const auto lhsAdjMatrix = AdjacencyMatrix(*lhs);
    const auto rhsAdjMatrix = AdjacencyMatrix(*rhs);

    const auto lhsInputSize = lhsAdjMatrix.m_InputIndexes.size();
    const auto lhsOutputSize = lhsAdjMatrix.m_OutputIndexes.size();
    const auto rhsInputSize = rhsAdjMatrix.m_InputIndexes.size();
    const auto rhsOutputSize = rhsAdjMatrix.m_OutputIndexes.size();

    std::stringstream headerDiff;
    headerDiff << CompareModuleHeaders(lhs, rhs);

    std::stringstream res;
    if (lhsInputSize != rhsInputSize)
    {
        res << "Module input size not equal:\n";
        res << "Lhs module input size: " << lhsAdjMatrix.m_InputIndexes.size() << "\n";
        res << "Rhs module input size: " << rhsAdjMatrix.m_InputIndexes.size() << "\n";
    }
    if (lhsOutputSize != rhsOutputSize)
    {
        res << "Module output size not equal:\n";
        res << "Lhs module output size: " << lhsAdjMatrix.m_OutputIndexes.size() << "\n";
        res << "Rhs module output size: " << rhsAdjMatrix.m_OutputIndexes.size() << "\n";
    }

    const auto lhsOpSize = lhsAdjMatrix.m_AdjacencyMatrix.size() - (lhsInputSize + lhsOutputSize);
    const auto rhsOpSize = rhsAdjMatrix.m_AdjacencyMatrix.size() - (rhsInputSize + rhsOutputSize);
    if (lhsOpSize != rhsOpSize)
    {
        res << "Module TOSA Operator size does not match:\n";

        res << "Lhs module output size: " << lhsOpSize << "\n";
        res << "Rhs module output size: " << rhsOpSize << "\n\n";

        if (lhsOpSize > rhsOpSize)
        {
            res << "Additional TOSA Operators in lhs:\n";
            for (size_t i = lhsInputSize + rhsOpSize; i < lhsOpSize; ++i)
            {
                res << InstructionToString(lhsAdjMatrix.m_Instructions[i]) << "\n";
            }
        }
        else
        {
            res << "Additional TOSA Operators in rhs:\n";
            for (size_t i = rhsInputSize + lhsOpSize; i < rhsOpSize; ++i)
            {
                res << InstructionToString(rhsAdjMatrix.m_Instructions[i]) << "\n";
            }
        }
    }

    if (!res.str().empty())
    {
        res << "\n";
    }

    for (size_t i = 0; i < lhsAdjMatrix.m_AdjacencyMatrix.size(); ++i)
    {
        const auto& lhsEdges = lhsAdjMatrix.m_AdjacencyMatrix[i];
        const auto& rhsEdges = rhsAdjMatrix.m_AdjacencyMatrix[i];

        if (lhsEdges.size() != rhsEdges.size() || !std::equal(lhsEdges.begin(), lhsEdges.end(), rhsEdges.begin()))
        {
            res << "TOSA Operator graph not equal:\n";
            res << "Lhs Operator: " << InstructionToString(lhsAdjMatrix.m_Instructions[i]) << " connects to "
                << lhsEdges.size() << "TOSA Operators:\n";
            for (const auto& edge : lhsEdges)
            {
                res << "    " << InstructionToString(lhsAdjMatrix.m_Instructions[edge]) << "\n";
            }

            res << "\n\nRhs Operator: " << InstructionToString(rhsAdjMatrix.m_Instructions[i]) << " connects to "
                << rhsEdges.size() << "TOSA Operators:\n";
            for (const auto& edge : rhsEdges)
            {
                res << "    " << InstructionToString(rhsAdjMatrix.m_Instructions[edge]);
            }
            break;
        }
    }

    // If the model structures don't match, we stop the comparison as it won't be meaningfull to compare in more detail
    if (!res.str().empty())
    {
        return headerDiff.str() + res.str();
    }

    // Structure of the SPIRV model is equal. We now complete the comparison.
    std::vector<InstructionPair> problemOperators;
    std::vector<ProblemTree> commonProblems;
    for (size_t i = 0; i < lhsAdjMatrix.m_AdjacencyMatrix.size(); ++i)
    {
        const auto& lhsInstruction = lhsAdjMatrix.m_Instructions[i];
        const auto& rhsInstruction = rhsAdjMatrix.m_Instructions[i];

        if (const auto problemTree = CompareInstructions(lhsInstruction, rhsInstruction, 0))
        {
            problemOperators.emplace_back(&rhsInstruction, &lhsInstruction);
            for (const auto& leaf : problemTree->m_Leafs)
            {
                commonProblems.push_back(leaf);
            }
        }
    }

    if (problemOperators.empty() && commonProblems.empty())
    {
        return headerDiff.str();
    }

    std::string lhsDis;
    std::string rhsDis;
    if (co.m_modelView == ModelView::problemsOnly)
    {
        for (const auto problem : problemOperators)
        {
            lhsDis += InstructionToString(*problem.m_Lhs);
            lhsDis += "\n";
            rhsDis += InstructionToString(*problem.m_Rhs);
            rhsDis += "\n";
        }
    }
    else
    {
        const auto lhsBinary = tfsc::WriteToBinary(lhs);
        lhsDis = DisassembleSPIRV(lhsBinary, false, false);
        lhsDis =
            co.m_modelView == ModelView::module ? lhsDis : GetSubString(lhsDis, "OpTypeGraphARM ", "OpGraphEndARM");

        const auto rhsBinary = tfsc::WriteToBinary(rhs);
        rhsDis = DisassembleSPIRV(rhsBinary, false, false);
        rhsDis =
            co.m_modelView == ModelView::module ? rhsDis : GetSubString(rhsDis, "OpTypeGraphARM ", "OpGraphEndARM");
    }

    // Group problems together
    const auto canonicalizedProblems = CanonicalizeProblems(commonProblems);

    std::vector<HighlightCtx> highlightList;
    std::stringstream lhsProblemSummary;
    std::stringstream rhsSStream;

    for (const auto& problemGroup : canonicalizedProblems)
    {
        HighlightCtx highlightCtx;
        const auto colour = std::string(GetNextColour());

        highlightCtx.m_pre = colour;
        highlightCtx.m_post = resetCode;

        lhsProblemSummary << "\n" << colour << boundary << resetCode << "\n";
        rhsSStream << "\n\n";

        // First group, instructions where terminal operands or instruction type are mismatched
        for (const auto& problem : problemGroup.first)
        {
            highlightCtx.m_lhs.emplace_back(ResIdToString(*problem.m_Root.m_Lhs));
            highlightCtx.m_rhs.emplace_back(ResIdToString(*problem.m_Root.m_Rhs));

            auto lhsProblemString = "           " + InstructionToString(*problem.m_Root.m_Lhs) + "\n";
            auto rhsProblemString = "           " + InstructionToString(*problem.m_Root.m_Rhs) + "\n";

            for (const auto& operand : problem.m_ProblemOperands)
            {
                // Problem is a terminal operand, lets highlight it
                std::string problemOperandStringLhs = OperandToString(operand.first);
                std::string problemOperandStringRhs = OperandToString(operand.second);

                HighlightString(lhsProblemString, problemOperandStringLhs, colour, resetCode);
                HighlightString(rhsProblemString, problemOperandStringRhs, colour, resetCode);
            }

            lhsProblemSummary << lhsProblemString;
            rhsSStream << rhsProblemString;
        }
        lhsProblemSummary << boundaryLight << "\n";
        rhsSStream << "\n";

        // second group intermediate instructions partially or wholly composed of the first group
        for (const auto& problem : problemGroup.second)
        {
            CollectResIds(problem, highlightCtx.m_lhs, highlightCtx.m_rhs);

            lhsProblemSummary << PrintProblemTreeLhs(problem);
            rhsSStream << PrintProblemTreeRhs(problem);
        }

        lhsProblemSummary << "\n" << colour << boundary << resetCode << "\n";
        rhsSStream << "\n\n";
        highlightList.push_back(highlightCtx);
    }

    std::string lhsS = lhsProblemSummary.str();
    std::string rhsS = rhsSStream.str();
    for (const auto& highLightCtx : highlightList)
    {
        for (unsigned int i = 0; i < highLightCtx.m_lhs.size(); i++)
        {
            HighlightString(lhsS, highLightCtx.m_lhs[i], highLightCtx.m_pre, highLightCtx.m_post);
            HighlightString(rhsS, highLightCtx.m_rhs[i], highLightCtx.m_pre, highLightCtx.m_post);

            HighlightString(lhsDis, highLightCtx.m_lhs[i], highLightCtx.m_pre, highLightCtx.m_post);
            HighlightString(rhsDis, highLightCtx.m_rhs[i], highLightCtx.m_pre, highLightCtx.m_post);
        }
    }

    res << ConcatStringLines(lhsS, rhsS, 20);
    res << ConcatStringLines(lhsDis, rhsDis, 20);
    return headerDiff.str() + res.str();
}
} // namespace testutils
