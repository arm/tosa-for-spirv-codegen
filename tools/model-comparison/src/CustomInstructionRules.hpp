// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "ProblemTree.hpp"
#include "ReplicateCompositeAirity.hpp"
#include <Instruction.hpp>
#include <ModuleComparator.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <iterator>
#include <functional>
#include <unordered_map>
#include <optional>
#include <utility>
#include <vector>

namespace testutils
{

static std::optional<ProblemTree>
Compare_Composite_to_Replicate(const Instruction& comp, const Instruction& repl, int depth);

struct OpPair
{
    spv::Op a;
    spv::Op b;
    bool operator==(const OpPair& o) const noexcept { return a == o.a && b == o.b; }
};

struct OpPairHash
{
    size_t operator()(const OpPair& p) const noexcept
    {
        return (static_cast<size_t>(p.a) << 1) ^ static_cast<size_t>(p.b);
    }
};

using CustomComparator =
    std::function<std::optional<ProblemTree>(const Instruction& lhs, const Instruction& rhs, int depth)>;

struct CustomRegistry
{

    void Register(spv::Op lhsOp, spv::Op rhsOp, CustomComparator fn)
    {
        m_rules.emplace(OpPair{lhsOp, rhsOp}, std::move(fn));
        // also register the function with op codes swapped
        if (lhsOp != rhsOp)
        {
            m_rules.emplace(
                OpPair{rhsOp, lhsOp},
                [orig = m_rules.at(OpPair{lhsOp, rhsOp})](const Instruction& l, const Instruction& r, int d) {
                    return orig(r, l, d);
                });
        }
    }

    CustomRegistry()
    {
        Register(spv::OpConstantComposite, spv::OpConstantCompositeReplicateEXT, Compare_Composite_to_Replicate);
    }

    std::unordered_map<OpPair, CustomComparator, OpPairHash> m_rules;
};

std::optional<ProblemTree> CompareInstructions(const Instruction& lhs, const Instruction& rhs, const int depth);

static std::optional<ProblemTree>
Compare_Composite_to_Replicate(const Instruction& comp, const Instruction& repl, int depth)
{
    bool isProblem = false;
    std::vector<ProblemTree> leafs;

    const auto replConstituentType = *repl.m_Operands[0].m_InstructionPtr;
    const auto compConstituentType = *repl.m_Operands[0].m_InstructionPtr;
    auto constituentTypeCheck = CompareInstructions(replConstituentType, compConstituentType, depth);
    if (constituentTypeCheck)
    {
        isProblem = true;
        leafs.push_back(constituentTypeCheck.value());
    }

    auto consituentAirity = ConstituentCountFromResultType(replConstituentType);

    const auto compConstituentValueBegin = comp.m_Operands.cbegin() + 2;

    // Check the infered size of the replicateComposite against actual size of the constComposite
    if (std::distance(compConstituentValueBegin, comp.m_Operands.cend()) != consituentAirity)
    {
        isProblem = true;
    }
    else
    {
        const auto replConstituentValue = *repl.m_Operands[2].m_InstructionPtr;

        for (auto it = compConstituentValueBegin; it != comp.m_Operands.cend(); it++)
        {
            auto constituentValueCheck = CompareInstructions(*it->m_InstructionPtr, replConstituentValue, depth);
            if (constituentValueCheck)
            {
                leafs.push_back(constituentValueCheck.value());
            }
        }
    }

    return (isProblem || !leafs.empty()) ? std::make_optional<ProblemTree>(&comp, &repl, depth, isProblem, leafs)
                                         : std::nullopt;
}
} // namespace testutils
