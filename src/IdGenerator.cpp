//
// Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <IdGenerator.hpp>

namespace tosa2spirv
{

namespace graphbuilder
{

ResId IdGenerator::GetNextId()
{
    return m_NextId++;
}

void IdGenerator::Restart()
{
    m_NextId = 1;
}

void IdGenerator::SetNextId(unsigned int id)
{
    m_NextId = id;
}

} // namespace graphbuilder

} // namespace tosa2spirv