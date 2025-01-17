//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <utility>

namespace tosa2spirv
{

namespace spirvwriter
{

/// SPIR-V operand types.
/// This includes number ID, single word, double word and string types.
enum OperandType
{
    NUMBER_ID,
    LITERAL_WORD,
    LITERAL_DWORD,
    LITERAL_STRING
};

/// Class which holds the operand type, string and single/double number value.
class Operand
{
public:
    /// Constructor which initialises operand type and single number.
    /// @param[in] type SPIR-V operand type to initialise.
    /// @param[in] num  Single number value to initialise.
    Operand(const OperandType type, const uint32_t num)
        : m_Type(type)
        , m_LiteralNum{num, 0}
    {}

    /// Constructor which initialises operand type and string.
    /// @param[in] type SPIR-V operand type to initialise.
    /// @param[in] str  String to initialise.
    Operand(const OperandType type, const char* str)
        : m_Type(type)
        , m_LiteralStr(str)
        , m_LiteralNum()
    {}

    /// Constructor which initialises operand as LITERAL_WORD type and single number.
    /// @param[in] num  Single number value to initialise.
    explicit Operand(const uint32_t num)
        : m_Type(LITERAL_WORD),
          m_LiteralNum{num, 0}
    {}

    /// Constructor which initialises operand as LITERAL_WORD type and two numbers.
    /// @param[in] num1  First number value to initialise.
    /// @param[in] num2  Second number value to initialise.
    Operand(const uint32_t num1, const u_int32_t num2)
        : m_Type(LITERAL_DWORD)
        , m_LiteralNum{num1, num2}
    {}

    /// Constructor which initialises operand type and string.
    /// @param[in] type SPIR-V operand type to initialise.
    /// @param[in] str  String to initialise.
    Operand(const OperandType type, std::string str)
        : m_Type(type)
        , m_LiteralStr(std::move(str))
        , m_LiteralNum()
    {}

    bool operator==(const Operand& other) const
    {
        return m_Type == other.m_Type &&
               m_LiteralStr == other.m_LiteralStr &&
               m_LiteralNum == other.m_LiteralNum;
    }

    bool operator!=(const Operand& other) const
    {
        return !(*this == other);
    }

    /// Default destructor.
    ~Operand() = default;

    /// Return the type to the user
    OperandType GetType() const { return m_Type; }

    /// Return the single number value to the user.
    uint32_t GetNumId() const { return m_LiteralNum[0]; }

    /// Return the string value to the user.
    std::string GetLiteralStr() const { return m_LiteralStr; }

    /// Return the number values array to the user.
    std::array<uint32_t, 2> GetLiteralNum() const { return m_LiteralNum; }

    /// Return the number of words used.
    uint32_t GetNumWords() const;

private:
    OperandType m_Type;
    std::string m_LiteralStr;
    std::array<uint32_t, 2> m_LiteralNum;
};

} // spirvwriter namespace

} // tosa2spirv namespace