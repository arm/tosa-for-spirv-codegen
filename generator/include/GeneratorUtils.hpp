//
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <IParser.hpp>
#include <vgf/encoder.hpp>
#include <nlohmann/json.hpp>

namespace tosa2spirv::generator
{
/// Method used to create a .vgf file containing SPIR-V binary (.spv) file and companion file (.json).
/// @param[in] parser IParser from which to obtain the Module and ResourseInfo from.
/// @param[in] spirv Spir-V Binary, vector of uint32_t words, obtained by calling GenerateSPIRV.
/// @param[in] modelName String containing name to be given to the model within the JSON.
/// @param[in] outputDir String containing the path where .vgf file will be output.
/// @param[in] filename String containing name to be given to outputted .vgf file.
/// @return JSON object with the resource info of the module.
void WriteVgfFile(tosa2spirv::parsers::IParser* parser,
                  std::vector<uint32_t>& spirv,
                  const std::string& modelName,
                  const std::string& outputDir = ".",
                  const std::string& zipName = "output.vgf");

} // tosa2spirv::generator