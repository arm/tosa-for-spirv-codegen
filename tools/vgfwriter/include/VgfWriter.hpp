//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <TosaForSpirvCodegen.hpp>

#include <memory>

namespace tfsc::vgfwriter
{

/// Write a given SPIR-V Module along with its Constants out as a .vgf (Vulkan Graph Format) file.
/// @param[in] module Module Object representing SPIR-V Module
/// @param[in] data Vector of void* pointers containing data passed to Graph Constants of Module.
/// @param[in] moduleName std::string containing name of module
/// @param[in] string Path to where file will be output
/// @param[in] string filename of the outputted Vulkan Graph Format file
void WriteVGF(std::shared_ptr<spirv::Module>& module,
              std::vector<std::pair<const void*, size_t>>&& data,
              const std::string& moduleName,
              const std::string& path,
              const std::string& filename);

} // namespace tfsc::vgfwriter
