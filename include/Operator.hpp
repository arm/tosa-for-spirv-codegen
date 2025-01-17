//
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <ResId.hpp>
#include <OperatorDefinitions.hpp>
#include <Tensor.hpp>
#include <vector>

namespace tosa2spirv::graphbuilder
{

/// Struct that represents a TOSA operator
struct Operator
{
     /// Operator constructor
     /// @param[in] TOSAEnum enum of the operator
     /// @param[in] resIds vector of ResIds matching the output tensor resIds of the operator
     /// @param[in] inputTensors input tensors of the Operator
     /// @param[in] outputTensors output tensors of the Operator
     /// @param[in] attributeTensors attribute tensors of the Operator
     Operator(const OperatorEnum operatorType,
              const std::vector<ResId>& resIds,
              std::vector<Tensor> inputTensors,
              std::vector<Tensor> outputTensors,
              std::vector<Tensor> attributeTensors = {})
      : m_OperatorEnum(operatorType)
      , m_ResIds(resIds)
      , m_InputTensors(std::move(inputTensors))
      , m_OutputTensors(std::move(outputTensors))
      , m_AttributeTensors(std::move(attributeTensors))
      {}

    /// TOSAEnum enum of the operator
    OperatorEnum m_OperatorEnum;
     /// ResIds vector of ResIds matching the output tensor resIds of the operator
    std::vector<ResId> m_ResIds;
     /// Input tensors of the Operator
    std::vector<Tensor> m_InputTensors;
     /// Output tensors of the Operator
    std::vector<Tensor> m_OutputTensors;
     /// Attribute tensors of the Operator
    std::vector<Tensor> m_AttributeTensors;
};

} // namespace tosa2spirv::graphbuilder