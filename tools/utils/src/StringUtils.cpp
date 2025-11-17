//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "StringUtils.hpp"
#include "EnumMaps.hpp"

#include <Instruction.hpp>
#include <Operand.hpp>
#include <algorithm>
#include <stdexcept>

#include <iostream>
#include <sstream>
#include <vector>

namespace testutils
{
using namespace tfsc::spirv;

std::string ResIdToString(const tfsc::spirv::Instruction& instruction)
{
    std::string resId{};
    for (const auto& operand : instruction.m_Operands)
    {
        if (operand.m_Type == tfsc::spirv::RES_ID)
        {
            resId = "%" + std::to_string(operand.m_LiteralWord);
            break;
        }
    }
    return resId;
}

std::string OperandToString(const tfsc::spirv::Operand& operand)
{
    std::string operandString;
    switch (operand.m_Type)
    {
        case tfsc::spirv::INSTRUCTION_POINTER: operandString = ResIdToString(*operand.m_InstructionPtr); break;
        case tfsc::spirv::RES_ID: break;
        case tfsc::spirv::LITERAL_WORD: operandString = std::to_string(operand.m_LiteralWord); break;
        case tfsc::spirv::LITERAL_STRING: operandString = *operand.m_LiteralStr; break;
        default: throw std::runtime_error("Invalid operand type");
    }
    return operandString;
}

std::string InstructionToString(const tfsc::spirv::Instruction& instruction)
{
    std::string instructionString;

    instructionString += ResIdToString(instruction);

    if (!instructionString.empty())
    {
        instructionString += " = ";
    }

    instructionString += OpToString(instruction.m_Opcode);
    for (const auto& operand : instruction.m_Operands)
    {
        instructionString += " " + OperandToString(operand);
    }
    return instructionString;
}

std::string GetSubString(const std::string& dis, const std::string& lower, const std::string& upper)
{
    const size_t start = dis.find(lower);
    if (start == std::string::npos)
        return "";

    const size_t startPos = start + lower.length();
    const size_t end = dis.find(upper, startPos);
    if (end == std::string::npos)
    {
        return "";
    }

    return dis.substr(startPos, end - startPos);
}

std::size_t GetVisibleLength(const std::string& s)
{
    std::size_t count = 0;
    std::size_t i = 0;
    const std::size_t n = s.size();
    while (i < n)
    {
        if (s[i] == '\x1b' && (i + 1 < n) && s[i + 1] == '[')
        {
            // Skip over the entire CSI sequence "\x1b[ ... m"
            i += 2; // skip "\x1b["
            // advance until we hit 'm' (or run out of string)
            while (i < n && s[i] != 'm')
            {
                ++i;
            }
            if (i < n)
            {
                // skip the final 'm' as well
                ++i;
            }
            // now we've consumed one entire escape‐sequence; do NOT increment 'count'
        }
        else
        {
            // regular/printable character
            ++count;
            ++i;
        }
    }
    return count;
}
#include <cctype>
#include <string>

void HighlightString(std::string& str, const std::string& pattern, const std::string& pre, const std::string& post)
{
    if (pattern.empty())
        return; // nothing to do

    const std::string highlighted = pre + pattern + post;
    size_t scanPos = 0;

    auto is_word_char = [](unsigned char ch) -> bool { return std::isalnum(ch) || ch == '_' || ch == '%'; };

    while (true)
    {
        size_t found = str.find(pattern, scanPos);
        if (found == std::string::npos)
            break;

        size_t afterPos = found + pattern.size();

        // Check "word boundary" BEFORE
        bool validBefore = false;
        if (found == 0)
        {
            validBefore = true; // start of string is a boundary
        }
        else
        {
            unsigned char cPrev = static_cast<unsigned char>(str[found - 1]);
            validBefore = !is_word_char(cPrev);
        }

        // Check "word boundary" AFTER
        bool validAfter = false;
        if (afterPos == str.size())
        {
            validAfter = true; // end of string is a boundary
        }
        else
        {
            unsigned char cNext = static_cast<unsigned char>(str[afterPos]);
            validAfter = !is_word_char(cNext);
        }

        if (validBefore && validAfter)
        {
            str.replace(found, pattern.size(), highlighted);
            // Jump past the inserted highlight to avoid re-matching inside it
            scanPos = found + highlighted.size();
        }
        else
        {
            // Not a whole-word match; continue scanning after this position
            scanPos = found + 1;
        }
    }
}

std::string HighlightResIds(std::string inst, const std::string resId, const std::string& pre, const std::string& post)
{
    if (resId.empty())
    {
        return inst;
    }

    const std::string rep = pre + resId + post;
    std::size_t pos = 0;

    const auto hSpace = resId + " ";
    // Loop until no more occurrences
    while ((pos = inst.find(hSpace, pos)) != std::string::npos)
    {
        inst.replace(pos, resId.size(), rep);
        pos += rep.size(); // advance past the replacement
    }

    return inst;
}

std::string_view GetNextColour()
{
    static constexpr std::string_view colours[24] = {
        "\033[38;5;196m", // Red
        "\033[38;5;226m", // Yellow
        "\033[38;5;69m",  // Steel blue
        "\033[38;5;51m",  // Cyan
        "\033[38;5;201m", // Magenta
        "\033[38;5;46m",  // Green
        "\033[38;5;209m", // Pink
        "\033[38;5;135m", // Medium purple
        "\033[38;5;215m", // Light pink
        "\033[38;5;119m", // Light green
        "\033[38;5;221m", // Lavender
        "\033[38;5;191m", // Light yellow‐green
        "\033[38;5;155m", // Mint green
        "\033[38;5;84m",  // Aquamarine
        "\033[38;5;85m",  // Turquoise
        "\033[38;5;86m",  // Light cyan
        "\033[38;5;81m",  // Sky blue
        "\033[38;5;75m",  // Light blue
        "\033[38;5;21m",  // Blue
        "\033[38;5;99m",  // Violet
        "\033[38;5;171m", // Light purple
        "\033[38;5;206m", // Hot pink
        "\033[38;5;205m", // Pinkish
        "\033[38;5;204m"  // Salmon
    };
    static int index = 0;
    return colours[(index++) % 24];
}

std::string Trim(const std::string& s)
{
    const std::string whitespace = " \t\n\r\f\v";
    std::size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos)
    {
        // The string is all whitespace (or empty) → return empty string
        return "";
    }
    std::size_t end = s.find_last_not_of(whitespace);
    // substr(start, length) where length = (end – start + 1)
    return s.substr(start, end - start + 1);
}

std::string ConcatStringLines(std::string_view lhs, std::string_view rhs, size_t pad)
{
    // 1) Split both inputs into vectors of lines
    std::istringstream lhsStream((std::string(lhs)));
    std::istringstream rhsStream((std::string(rhs)));
    std::vector<std::string> lhsLines;
    std::vector<std::string> rhsLines;
    std::string line;

    while (std::getline(lhsStream, line))
    {
        lhsLines.push_back(Trim(line));
    }
    while (std::getline(rhsStream, line))
    {
        rhsLines.push_back(Trim(line));
    }

    // 2) Remove any trailing empty lines from each side
    while (!lhsLines.empty() && lhsLines.back().empty())
    {
        lhsLines.pop_back();
    }
    while (!rhsLines.empty() && rhsLines.back().empty())
    {
        rhsLines.pop_back();
    }

    // 3) Compute the maximum width of any left‐hand line
    size_t maxLhsWidth = 0;
    size_t absoluteMaxWidth = 110;
    for (auto& l : lhsLines)
    {
        if (GetVisibleLength(l) <= absoluteMaxWidth)
            maxLhsWidth = std::max(maxLhsWidth, GetVisibleLength(l));
    }

    std::ostringstream result;
    size_t totalRows = std::max(lhsLines.size(), rhsLines.size());
    for (size_t i = 0; i < totalRows; ++i)
    {
        const std::string& lLine = (i < lhsLines.size() ? lhsLines[i] : "");
        const std::string& rLine = (i < rhsLines.size() ? rhsLines[i] : "");

        result << lLine;

        size_t needed = (GetVisibleLength(lLine) < maxLhsWidth) ? (maxLhsWidth - GetVisibleLength(lLine) + 4) : 4;
        result << std::string(needed, ' ');

        result << rLine << '\n';
    }
    result << '\n';

    return result.str();
}

} // namespace testutils
