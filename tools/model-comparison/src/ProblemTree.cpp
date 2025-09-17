//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "ProblemTree.hpp"
#include <StringUtils.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <optional>
#include <sstream>
#include <vector>

namespace testutils
{
using namespace tfsc::spirv;

std::optional<ProblemTree> CollectAndSeperateProblems(const ProblemTree& tree, std::vector<ProblemTree>& tree2)
{
    std::vector<ProblemTree> leafs;
    if (tree.m_IsAProblem)
    {
        tree2.push_back(tree);
        return std::nullopt;
    }

    for (const auto& leaf : tree.m_Leafs)
    {
        const auto newLeaf = CollectAndSeperateProblems(leaf, tree2);
        if (newLeaf)
        {
            // Some times a new leaf is much like the old one
            leafs.push_back(newLeaf.value());
        }
    }
    return std::make_optional<ProblemTree>(tree.m_Root.m_Lhs, tree.m_Root.m_Rhs, tree.m_Depth, false, leafs);
}

void CollectResIds(const ProblemTree& pt, std::vector<std::string>& lhsResIds, std::vector<std::string>& rhsResIds)
{
    lhsResIds.push_back(ResIdToString(*pt.m_Root.m_Lhs));
    rhsResIds.push_back(ResIdToString(*pt.m_Root.m_Rhs));

    for (const auto& leaf : pt.m_Leafs)
    {
        CollectResIds(leaf, lhsResIds, rhsResIds);
    }
}

std::string PrintProblemTreeRhs(const ProblemTree& pt)
{
    std::stringstream ss;
    const std::string indent(pt.m_Depth * 4, ' ');
    ss << indent << InstructionToString(*pt.m_Root.m_Rhs) << "\n";

    for (const auto& leaf : pt.m_Leafs)
    {
        ss << PrintProblemTreeRhs(leaf);
    }
    return ss.str();
}

std::string PrintProblemTreeLhs(const ProblemTree& pt)
{
    std::stringstream ss;
    const std::string indent(pt.m_Depth * 4, ' ');
    ss << indent << InstructionToString(*pt.m_Root.m_Lhs) << "\n";

    for (const auto& leaf : pt.m_Leafs)
    {
        ss << PrintProblemTreeLhs(leaf);
    }
    return ss.str();
}

std::string
PrintInstructionPair(const InstructionPair& pair, unsigned int depth, bool isProblem, const HighlightCtx& highlightCtx)
{
    auto lhsStr = InstructionToString(*pair.m_Lhs);
    auto rhsStr = InstructionToString(*pair.m_Rhs);

    for (const auto& lhs : highlightCtx.m_lhs)
    {
        lhsStr = HighlightResIds(lhsStr, lhs, highlightCtx.m_pre, highlightCtx.m_post);
    }

    for (const auto& rhs : highlightCtx.m_rhs)
    {
        rhsStr = HighlightResIds(rhsStr, rhs, highlightCtx.m_pre, highlightCtx.m_post);
    }

    const std::string rootStr = isProblem ? " <<<" : "";
    const std::string indent(depth * 4, ' ');

    const size_t padSize =
        GetVisibleLength(lhsStr) + rootStr.size() < 80 ? 80 - GetVisibleLength(lhsStr) - rootStr.size() : 1;
    const std::string pad(padSize, ' ');

    std::stringstream ss;
    ss << indent << lhsStr << rootStr << pad << rhsStr << rootStr << "\n";
    return ss.str();
}
} // namespace testutils
