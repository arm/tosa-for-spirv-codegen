//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Version.hpp>

namespace tosa2spirv
{

namespace graphbuilder
{

/// Class which will store TOSA configuration.
class Context
{
public:
    void SetTOSAVersion(TOSAVersion version);
    TOSAVersion GetTOSAVersion();
private:
    TOSAVersion m_TosaVersion = TOSAVersion::v0_80;
};

} // namespace graphbuilder

} // namespace tosa2spirv