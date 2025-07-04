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

std::string ExpandValues(const std::vector<int>& values, const std::string& dataType = "uint");

std::string AddSpacers(int index);

std::string CreateEffceeCheckFromDataType(DataType dataType);

bool CheckInputTensor(const std::vector<int>& values,
                      DataType dataType,
                      const std::string& instruction,
                      const std::string& goldenSPIRV);

bool CheckOutputTensor(const std::vector<int>& values,
                       DataType dataType,
                       const std::string& instruction,
                       const std::string& goldenSPIRV);

bool CheckOutput(const std::vector<int>& values,
                 DataType dataType,
                 const std::string& instruction,
                 const std::string& spirv);

bool CheckConstCompositeTensor(const std::vector<int>& values,
                               const std::string& instruction,
                               const std::string& goldenSPIRV,
                               const int index,
                               const std::string& dataType = "uint");

bool CheckGraphConstant(const std::vector<int>& values,
                        DataType dataType,
                        const std::string& instruction,
                        const std::string& goldenSPIRV,
                        int index,
                        int identifier);

bool CheckConstant(DataType dataType,
                   const std::string& instruction,
                   const std::string& goldenSPIRV,
                   uint32_t value,
                   int index);

bool CheckBoolConstant(DataType dataType,
                       const std::string& instruction,
                       const std::string& goldenSPIRV,
                       bool flag,
                       int index);

} // namespace testutils
