//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Instruction.hpp>
#include <ModuleComparator.hpp>
#include <StringUtils.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <map>
#include <optional>
#include <utility>
#include <vector>

namespace testutils
{
using namespace tfsc::spirv;

struct InstructionPair
{
    InstructionPair(const Instruction* lhs, const Instruction* rhs)
        : m_Lhs(lhs)
        , m_Rhs(rhs)
    {
    }
    const Instruction* m_Lhs;
    const Instruction* m_Rhs;
    bool operator<(InstructionPair const& other) const
    {
        return std::tie(m_Lhs, m_Rhs) < std::tie(other.m_Lhs, other.m_Rhs);
    }
};

struct ProblemTree
{
    ProblemTree(const Instruction* lhs,
                const Instruction* rhs,
                unsigned int d,
                bool isProblem,
                std::vector<ProblemTree> leafs = {},
                std::vector<std::pair<Operand, Operand>> operands = {})
        : m_Root{lhs, rhs}
        , m_Depth(d)
        , m_IsAProblem(isProblem)
        , m_Leafs(std::move(leafs))
        , m_ProblemOperands(std::move(operands))
    {
    }
    bool operator==(ProblemTree const& other) const noexcept
    {
        return m_Root.m_Lhs == other.m_Root.m_Lhs && m_Root.m_Rhs == other.m_Root.m_Rhs;
    }
    bool operator<(ProblemTree const& other) const { return m_Root < other.m_Root; }
    InstructionPair m_Root;
    unsigned int m_Depth;
    bool m_IsAProblem;
    std::vector<ProblemTree> m_Leafs;
    std::vector<std::pair<Operand, Operand>> m_ProblemOperands;
};

struct ProblemTreeHash
{
    std::size_t operator()(const ProblemTree& pt) const noexcept
    {
        const auto lhs = reinterpret_cast<std::size_t>(pt.m_Root.m_Lhs);
        const auto rhs = reinterpret_cast<std::size_t>(pt.m_Root.m_Rhs);
        return lhs ^ (rhs << 1);
    }
};

using ProblemTreeMap = std::map<std::vector<ProblemTree>, std::set<ProblemTree>>;
std::optional<ProblemTree> CollectAndSeperateProblems(const ProblemTree& tree, std::vector<ProblemTree>& pairs);

// Group together problems based on how they show up in the dependant instructions
template <typename Range> ProblemTreeMap CanonicalizeProblems(const Range& problemTrees)
{
    ProblemTreeMap grouped;

    for (const ProblemTree& tree : problemTrees)
    {
        std::vector<ProblemTree> problems;

        // Seperate the actual problems from the tree, to make printing them easier later
        const auto newTree = CollectAndSeperateProblems(tree, problems);

        std::sort(problems.begin(), problems.end(), [](const ProblemTree& a, const ProblemTree& b) {
            return a.m_Root < b.m_Root;
        });

        problems.erase(std::unique(problems.begin(),
                                   problems.end(),
                                   [](const ProblemTree& a, const ProblemTree& b) {
                                       return a.m_Root.m_Lhs == b.m_Root.m_Lhs && a.m_Root.m_Rhs == b.m_Root.m_Rhs;
                                   }),
                       problems.end());

        if (newTree)
        {
            grouped[std::move(problems)].insert(newTree.value());
        }
        else
        {
            grouped[std::move(problems)].insert({});
        }
    }

    return grouped;
}

void CollectResIds(const ProblemTree& pt, std::vector<std::string>& lhsResIds, std::vector<std::string>& rhsResIds);
std::string PrintProblemTreeRhs(const ProblemTree& pt);
std::string PrintProblemTreeLhs(const ProblemTree& pt);

std::string
PrintInstructionPair(const InstructionPair& pair, unsigned int depth, bool isProblem, const HighlightCtx& highlightCtx);

} // namespace testutils
