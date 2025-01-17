//
// Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <ResId.hpp>

namespace tosa2spirv
{

namespace graphbuilder
{

/// Class which will generate an ID of type unsigned int that is unique within a Module starting at 1.
class IdGenerator
{
public:

    /// Method used to obtain the next ID to be use in Module.
    /// @return unsigned int m_NextID which is then post incremented.
    ResId GetNextId();

    /// Method used to restart the IdGenerator back to its initial state of the NextId being 1.
    void Restart();

    /// Method used to set the IdGenerator to a desired value.
    /// @param[in] id Unsigned integer that the IdGenerator's next ID will be set to.
    void SetNextId(unsigned int id);

private:
    unsigned int m_NextId = 1;
};

} // namespace graphbuilder

} // namespace tosa2spirv
