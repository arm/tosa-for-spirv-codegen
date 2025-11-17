//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <vector>
#include <cstdint>

namespace tfsc
{

namespace spirv
{
class Module;
}

/// Enum class to specify different TOSA versions.
/// Refer to https://www.mlplatform.org/tosa/roadmap.html
enum class TOSAVersion
{
    v1_0
};

/// Factory function creating a tosa_for_spirv_codegen Module
/// Module is used to construct the tfsc::tosa::Graph
/// Sets default SPIRV-V Capabilities, Extensions and Memory model
/// @param[in] version version of TOSA
/// @return shared pointer of the Module
std::shared_ptr<spirv::Module> CreateModule(TOSAVersion version);

/// Write the Module to a binary representation of spirv
/// @param module the spirv Module
/// @return uint32_t binary spirv vector
std::vector<uint32_t> WriteToBinary(const std::shared_ptr<spirv::Module>& module);

} // namespace tfsc