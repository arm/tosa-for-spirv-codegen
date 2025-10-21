//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Module.hpp>

#include <unordered_map>
#include <vector>

namespace testutils
{

struct AdjacencyMatrix
{
    explicit AdjacencyMatrix(const tfsc::spirv::Module &module);
    std::vector<tfsc::spirv::Instruction> GetConnectedInstructions(unsigned int index) const;

    std::vector<tfsc::spirv::Instruction> m_Instructions;
    std::vector<std::vector<unsigned int>> m_AdjacencyMatrix;
    std::unordered_map<const tfsc::spirv::Instruction *, unsigned int> m_InstructionToIndex;
    std::vector<unsigned int> m_InputIndexes;
    std::vector<unsigned int> m_OutputIndexes;
    std::vector<unsigned int> m_GraphConstants;
};

inline bool isAdjacencyMatrixOp(const spv::Op op)
{
    return op == spv::OpGraphInputARM || op == spv::OpExtInst || op == spv::OpGraphSetOutputARM;
}

} // namespace testutils
