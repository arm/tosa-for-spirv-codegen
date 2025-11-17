//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Module.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace testutils
{

std::string
DiffSPIRV(const std::vector<uint32_t> &rhsBinary, const std::vector<uint32_t> &lhsBinary, bool runValidation = true);
std::string DisassembleSPIRV(const std::vector<uint32_t> &binary, bool runValidation = true, bool friendlyNames = true);

std::shared_ptr<tfsc::spirv::Module> LoadSPIRVDisassembly(const std::string &text);

inline unsigned int GetInstructionCount(const std::string &spirvString)
{
    return std::count(spirvString.begin(), spirvString.end(), '\n');
}

} // namespace testutils
