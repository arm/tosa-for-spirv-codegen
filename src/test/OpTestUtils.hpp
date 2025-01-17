//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <vector>
#include <string>

#include <Tensor.hpp>

using tosa2spirv::graphbuilder::DataType;

namespace tosa2spirv::spirvwriter {

static std::string ExpandValues(const std::vector<int>& values);

static std::string AddSpacers(const int index);

static std::string CreateEffceeCheckFromDataType(DataType dataType);

void CheckInputTensor(const std::vector<int>& values,
                      DataType dataType,
                      const std::string& instruction,
                      const std::string& goldenSPIRV);

void CheckOutputTensor(const std::vector<int>& values,
                       DataType dataType,
                       const std::string& instruction,
                       const std::string& goldenSPIRV);

void CheckConstCompositeTensor(const std::vector<int>& values,
                              const std::string& instruction,
                              const std::string& goldenSPIRV,
                              const int index);

void CheckGraphConstant(const std::vector<int>& values,
                        DataType dataType,
                        const std::string& instruction,
                        const std::string& goldenSPIRV);

void CheckConstant(DataType dataType,
                   const std::string& instruction,
                   const std::string& goldenSPIRV,
                   const uint32_t value,
                   const int index);

void CheckBoolConstant(DataType dataType,
                       const std::string& instruction,
                       const std::string& goldenSPIRV,
                       bool flag,
                       int index);

} // namespace tosa2spirv::spirvwriter
