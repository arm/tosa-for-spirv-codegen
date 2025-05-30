//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <AssemblyUtils.hpp>
#include <Module.hpp>

#include <memory>

namespace testutils
{
using namespace tosa2spirv::spirv;

std::string CompareModules(const std::shared_ptr<Module>& lhs, const std::shared_ptr<Module>& rhs);

inline std::string CompareModules(const std::shared_ptr<Module>& lhs, const std::string& rhs)
{
    return CompareModules(lhs, LoadSPIRVDisassembly(rhs));
}

} // namespace testutils