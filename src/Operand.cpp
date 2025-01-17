//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "Operand.hpp"


namespace tosa2spirv
{

namespace spirvwriter
{

uint32_t Operand::GetNumWords() const
{
    switch (m_Type)
    {
        case NUMBER_ID:
        case LITERAL_WORD:
            return 1;
        case LITERAL_DWORD:
            return 2;
        case LITERAL_STRING:
            // Get the string size accounting for the terminating null character.
            return static_cast<uint32_t>((m_LiteralStr.size() + 4) / 4);
        default:
            return 0;
    }
}

} // spirvwriter namespace

} // tosa2spirv namespace