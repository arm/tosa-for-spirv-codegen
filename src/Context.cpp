//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Context.hpp>

namespace tosa2spirv
{

namespace graphbuilder
{

void Context::SetTOSAVersion(TOSAVersion version)
{
    m_TosaVersion = version;
}

TOSAVersion Context::GetTOSAVersion()
{
    return m_TosaVersion;
}

} // namespace graphbuilder

} // namespace tosa2spirv