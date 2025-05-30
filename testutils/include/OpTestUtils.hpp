//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>
#include <vector>

#include <Tensor.hpp>

using tosa2spirv::tosa::DataType;

namespace testutils
{

std::string ExpandValues(const std::vector<int>& values);

std::string AddSpacers(int index);

std::string CreateEffceeCheckFromDataType(DataType dataType);

void CheckInputTensor(const std::vector<int>& values,
                      DataType dataType,
                      const std::string& instruction,
                      const std::string& goldenSPIRV);

void CheckOutputTensor(const std::vector<int>& values,
                       DataType dataType,
                       const std::string& instruction,
                       const std::string& goldenSPIRV);

void CheckOutput(const std::vector<int>& values,
                 DataType dataType,
                 const std::string& instruction,
                 const std::string& spirv);

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
                   uint32_t value,
                   int index);

void CheckBoolConstant(DataType dataType,
                       const std::string& instruction,
                       const std::string& goldenSPIRV,
                       bool flag,
                       int index);

} // namespace testutils
