//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstdint>
#include <string>

// tosa-for-spirv-codegen's shorthand namespace
namespace tfsc::spirv
{

enum OperandType
{
    INSTRUCTION_POINTER,
    LITERAL_WORD,
    LITERAL_STRING,
    RES_ID,
    UNINITIALIZED,
};

class Instruction;
/// tosa-for-spirv-codegen's implementation of SPIR-V operand.
/// Operand is a typed union that can be either an uint32_t word, string or pointer to an Instruction
/// Operands are the default return value for most tosa-for-spirv-codegen functions
class Operand
{
    public:
    /// Default constructor for Operand with m_Type = UNINITIALIZED and m_LiteralWord = 0.
    Operand()
        : m_Type(UNINITIALIZED)
        , m_LiteralWord(0)
    {
    }

    /// LITERAL_WORD Constructor for Operand
    /// @param[in] word uint32_t value
    /// @param[in] type type either LITERAL_WORD or RES_ID defaulting to LITERAL_WORD
    explicit Operand(const uint32_t word, const OperandType type = LITERAL_WORD)
        : m_Type(type)
        , m_LiteralWord{word}
    {
    }

    /// INSTRUCTION_POINTER Constructor for Operand
    /// @param[in] instruction_ptr const Instruction* pointer
    explicit Operand(const Instruction* instruction_ptr)
        : m_Type(INSTRUCTION_POINTER)
        , m_InstructionPtr(instruction_ptr)
    {
    }

    /// LITERAL_STRING Constructor for Operand
    /// @param[in] str const std::string&
    explicit Operand(const std::string& str)
        : m_Type(LITERAL_STRING)
        , m_LiteralStr(new auto(str))
    {
    }

    /// LITERAL_STRING Constructor for Operand
    /// @param[in] str const char*  const std::string&
    explicit Operand(const char* str)
        : m_Type(LITERAL_STRING)
        , m_LiteralStr(new std::basic_string<char>(str))
    {
    }

    // Special member functions for typed enums have to be handwritten
    Operand& operator=(const Operand& other);
    /// Copy Constructor
    Operand(const Operand& rhs) noexcept;
    /// Move Constructor
    Operand(Operand&& rhs) noexcept;
    /// Move Assignment Operator
    Operand& operator=(Operand&& rhs) noexcept;

    bool operator<(const Operand& other) const;
    bool operator==(const Operand& other) const;
    bool operator!=(const Operand& operand) const { return !(*this == operand); }

    uint32_t WordSize() const;
    ~Operand();

    OperandType m_Type;
    union
    {
        uint32_t m_LiteralWord{};
        std::basic_string<char>* m_LiteralStr;
        const Instruction* m_InstructionPtr;
    };
};

} // namespace tfsc::spirv