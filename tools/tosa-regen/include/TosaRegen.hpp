//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Module.hpp>
#include <Operand.hpp>
#include <OperatorEnum.hpp>
#include <SpirvParser.hpp>
#include <Tensor.hpp>
#include <TosaOperator.hpp>

#include <algorithm>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <attribute.h>
#include <tosa_generated.h>
#include <tosa_serialization_handler.h>

namespace tosaregen
{

/// Generates an std::vector of C/C++ strings describing all operators found in the given Module instance, using
/// parameters that the TosaOperator structure consists of
std::vector<std::string> Spirv2tosa(const std::shared_ptr<tfsc::spirv::Module>& module,
                                    OperatorComparator filter);

/// Generates an std::vector of TosaOperator instances describing all operators found in the given Module instance
std::vector<TosaOperator> Spirv2operators(const std::shared_ptr<tfsc::spirv::Module>& module,
                                          OperatorComparator filter);

/// Returns the actual ResId number from a tfsc::tosa::ResId instance (masked spirv::Instruction pointer)
inline uint32_t GetResIdNumber(const tfsc::tosa::ResId& resId)
{
    if (resId->m_Operands.size() > 0 && resId->m_Operands[0].m_Type == tfsc::spirv::RES_ID)
    {
        return resId->m_Operands[0].m_LiteralWord;
    }
    if (resId->m_Operands.size() > 1 && resId->m_Operands[1].m_Type == tfsc::spirv::RES_ID)
    {
        return resId->m_Operands[1].m_LiteralWord;
    }
    throw std::runtime_error{"GetResIdNumber: Did not find valid RES_ID number"};
};

/// Parses a TosaOperator instance and creates corresponding TosaSerializationOperator instances (including necessary
/// Op_CONST) and TosaSerializationTensor instances, which are appended to operators and tensors respectively.
/// The graphConstants vector allows users to specify external graph constant data, where the index in the vector
/// corresponds to the graph constant ID. The tensorNameMap option allows users to explicitly name specific tensors in
/// the resulting Handler, with a key of the ResId number of the tensor instruction.
void GetTosaSerializationOperator(const TosaOperator& op,
                                  std::vector<std::unique_ptr<tosa::TosaSerializationOperator>>& operators,
                                  std::map<std::string, std::unique_ptr<tosa::TosaSerializationTensor>>& tensors,
                                  std::set<std::string>& dynamicInputNames,
                                  std::set<std::string>& dynamicOutputNames,
                                  const std::vector<std::vector<uint32_t>>& graphConstants = {},
                                  const std::map<uint32_t, std::string>& tensorNameMap = {},
                                  std::string tensorNamePrefix = "tensor_");

/// Parses a spirv::Module instance and creates a corresponding TosaSerializationHandler structure.
/// The graphConstants vector allows users to specify external graph constant data, where the index in the vector
/// coresponds to the graph constant ID. The tensorNameMap option allows users to explicitly name specific tensors in
/// the resulting Handler, with a key of the ResId number of the tensor instruction.
std::unique_ptr<tosa::TosaSerializationHandler>
GetTosaSerializationHandler(const std::shared_ptr<tfsc::spirv::Module>& module,
                            const std::vector<std::vector<uint32_t>>& graphConstants = {},
                            const std::map<uint32_t, std::string>& tensorNameMap = {});

} // namespace tosaregen
