//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <spirv/Operand.hpp>

#include <cstdint>
#include <string>

namespace tfsc::spirv
{

uint32_t Operand::WordSize() const
{
    switch (m_Type)
    {
        case INSTRUCTION_POINTER:
        case RES_ID:
        case LITERAL_WORD: return 1;
        case LITERAL_STRING:
            // Get the string size accounting for the terminating null character.
            return static_cast<uint32_t>((m_LiteralStr->size() + 4) / 4);
        default: return 0;
    }
}

bool Operand::operator<(const Operand& other) const
{
    if (m_Type != other.m_Type)
    {
        return m_Type < other.m_Type;
    }
    switch (m_Type)
    {
        case INSTRUCTION_POINTER: return m_InstructionPtr < other.m_InstructionPtr;
        case RES_ID:
        case LITERAL_WORD: return m_LiteralWord < other.m_LiteralWord;
        case LITERAL_STRING: return *m_LiteralStr < *other.m_LiteralStr;
        default:; return false;
    }
}

bool Operand::operator==(const Operand& rhs) const
{
    if (m_Type != rhs.m_Type)
    {
        return false;
    }
    switch (m_Type)
    {
        case INSTRUCTION_POINTER: return this->m_InstructionPtr == rhs.m_InstructionPtr;
        case RES_ID:
        case LITERAL_WORD: return this->m_LiteralWord == rhs.m_LiteralWord;
        case LITERAL_STRING: return *this->m_LiteralStr == *rhs.m_LiteralStr;
        default:; return false;
    }
}

// Copy Constructor
Operand::Operand(const Operand& rhs) noexcept
{
    this->m_Type = rhs.m_Type;
    switch (m_Type)
    {
        case INSTRUCTION_POINTER: this->m_InstructionPtr = rhs.m_InstructionPtr; break;
        case RES_ID:
        case LITERAL_WORD: this->m_LiteralWord = rhs.m_LiteralWord; break;
        case LITERAL_STRING: this->m_LiteralStr = new auto(*rhs.m_LiteralStr); break;
        default:;
    }
}

// Copy Assignment
Operand& Operand::operator=(const Operand& other)
{
    // Prevent self assignment
    if (this == &other)
    {
        return *this;
    }

    if (m_Type == LITERAL_STRING)
    {
        delete m_LiteralStr;
    }

    this->m_Type = other.m_Type;
    switch (m_Type)
    {
        case INSTRUCTION_POINTER: m_InstructionPtr = other.m_InstructionPtr; break;
        case UNINITIALIZED:
        case RES_ID:
        case LITERAL_WORD: m_LiteralWord = other.m_LiteralWord; break;
        case LITERAL_STRING: m_LiteralStr = new auto(*other.m_LiteralStr); break;
        default:;
    }
    return *this;
}

// Move Assignment Operator
Operand& Operand::operator=(Operand&& rhs) noexcept
{
    // Prevent self assignment
    if (this == &rhs)
    {
        return *this;
    }

    if (m_Type == LITERAL_STRING)
    {
        delete m_LiteralStr;
    }

    this->m_Type = rhs.m_Type;
    switch (m_Type)
    {
        case INSTRUCTION_POINTER:
            m_InstructionPtr = rhs.m_InstructionPtr;
            rhs.m_InstructionPtr = nullptr;
            break;
        case UNINITIALIZED:
        case RES_ID:
        case LITERAL_WORD:
            m_LiteralWord = rhs.m_LiteralWord;
            rhs.m_LiteralWord = 0;
            break;
        case LITERAL_STRING:
            m_LiteralStr = rhs.m_LiteralStr;
            rhs.m_LiteralStr = nullptr;
            break;
        default:;
    }
    return *this;
}

// Move Constructor
Operand::Operand(Operand&& rhs) noexcept
{
    this->m_Type = rhs.m_Type;
    switch (m_Type)
    {
        case INSTRUCTION_POINTER:
            m_InstructionPtr = rhs.m_InstructionPtr;
            rhs.m_InstructionPtr = nullptr;
            break;
        case RES_ID:
        case LITERAL_WORD:
            m_LiteralWord = rhs.m_LiteralWord;
            rhs.m_LiteralWord = 0;
            break;
        case LITERAL_STRING:
            m_LiteralStr = rhs.m_LiteralStr;
            rhs.m_LiteralStr = nullptr;
            break;
        default:;
    }
}

Operand::~Operand()
{
    if (m_Type == LITERAL_STRING)
    {
        delete m_LiteralStr;
    }
}

} // namespace tfsc::spirv