//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <memory>
#include <tosa2spirv.hpp>

std::shared_ptr<tosa2spirv::spirv::Module> LoadSPIRV(const std::string &filename);
std::shared_ptr<tosa2spirv::spirv::Module> LoadSPIRVAssembly(const std::string &filename);
std::shared_ptr<tosa2spirv::spirv::Module> LoadSPIRVDisassembly(const std::string &filename);
std::shared_ptr<tosa2spirv::spirv::Module> LoadTOSAFile(const std::string &filename);
std::shared_ptr<tosa2spirv::spirv::Module> LoadVGFFile(const std::string &filename);
