
//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Instruction.hpp>

#include <cstddef>
#include <string>

namespace testutils
{
using namespace tfsc::spirv;

struct HighlightCtx
{
    std::vector<std::string> m_lhs = {};
    std::vector<std::string> m_rhs = {};
    std::string m_pre = {};
    std::string m_post = {};
    std::string m_tosaOp = {};
};

const unsigned int boundaryWidth = 120;
const std::string boundary(boundaryWidth, '=');
const std::string boundaryLight(boundaryWidth, '-');

std::size_t GetVisibleLength(const std::string& s);

std::string ResIdToString(const tfsc::spirv::Instruction& instruction);
std::string OperandToString(const tfsc::spirv::Operand& operand);
std::string InstructionToString(const tfsc::spirv::Instruction& instruction);
std::string HighlightResIds(std::string inst, const std::string resId, const std::string& pre, const std::string& post);

std::string ConcatStringLines(std::string_view lhs, std::string_view rhs, size_t pad = 4);
std::string GetSubString(const std::string& dis, const std::string& lower, const std::string& upper);
void HighlightString(std::string& str, const std::string& pattern, const std::string& pre, const std::string& post);

std::string_view GetNextColour();
static const std::string resetCode = "\x1b[0m";

std::string Trim(const std::string& s);
inline size_t GetLineWidth(const std::string& s)
{
    const size_t newline = s.find('\n');
    return (newline == std::string::npos) ? s.size() : newline;
}
} // namespace testutils
