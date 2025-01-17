//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstdint>

namespace tosa2spirv
{

namespace graphbuilder
{

/// Class which holds the result ID used in SPIR-V generation.
class ResId
{
public:
    /// Constructor which initialises ID specified by the user.
    /// @param[in] id Result ID to initialise.
    ResId(uint32_t id = 0)
        : m_Id(id)
    {}

    /// Overloaded equals operator to validate if two ID's are equal.
    bool operator==(const ResId& other) const { return m_Id == other.m_Id; }

    /// Overloaded less than operator to validate if ID is less than the other.
    bool operator<(const ResId& other) const { return m_Id < other.m_Id; }

    /// Return the ID to the user.
    uint32_t Get() const { return m_Id; }

    /// Check if the result ID is valid.
    bool IsValid() const { return m_Id != 0; }

private:
    uint32_t m_Id;
};

} // graphbuilder namespace

} // tosa2spirv namespace