//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <memory>
#include <TosaForSpirvCodegen.hpp>

std::shared_ptr<tfsc::spirv::Module> LoadSPIRV(const std::string &filename);
std::shared_ptr<tfsc::spirv::Module> LoadSPIRVAssembly(const std::string &filename);
std::shared_ptr<tfsc::spirv::Module> LoadSPIRVDisassembly(const std::string &filename);
std::shared_ptr<tfsc::spirv::Module> LoadTOSAFile(const std::string &filename);
std::shared_ptr<tfsc::spirv::Module> LoadVGFFile(const std::string &filename);
