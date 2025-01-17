//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Operand.hpp>
#include <Instruction.hpp>
#include <iostream>

namespace tosa2spirv
{

namespace graphbuilder
{

    /// Struct to hold PointerType, TensorType and Operator result Id operands.
    struct GraphIOInfo
    {
        // pointer operand, tensor operand and operator id operand
        GraphIOInfo(
                const spirvwriter::Operand& pointerOp,
                const spirvwriter::Operand& tensorOp,
                const spirvwriter::Operand& OperatorId)
                : m_Pointer(pointerOp), m_Tensor(tensorOp), m_Layer(OperatorId)
        {}
        spirvwriter::Operand m_Pointer;
        spirvwriter::Operand m_Tensor;
        spirvwriter::Operand m_Layer;
    };

    struct GraphInfo
    {
        private:
            int32_t m_InputIndex {0};
        public:
            std::vector<GraphIOInfo> m_GraphInputsInfo;
            std::vector<GraphIOInfo> m_GraphOutputsInfo;
            spirvwriter::InstructionList m_LayerList;
            spirvwriter::InstructionList m_GraphInputList;
            int32_t ReturnAndIncrementIndex()
            {
                return m_InputIndex++;
            }
    };

} // graphbuilder namespace

} // tosa2spirv namespace