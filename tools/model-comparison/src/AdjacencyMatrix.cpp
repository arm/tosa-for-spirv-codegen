//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "AdjacencyMatrix.hpp"

#include <list>

namespace testutils
{
using namespace tfsc::spirv;

void RecursiveTraverse(const Instruction *instruction, AdjacencyMatrix &adjMatrix, unsigned int &index)
{
    adjMatrix.m_Instructions.emplace_back(*instruction);
    adjMatrix.m_InstructionToIndex.emplace(instruction, index);

    if (instruction->m_Opcode == spv::OpGraphInputARM)
    {
        adjMatrix.m_InputIndexes.emplace_back(index);
    }
    else if (instruction->m_Opcode == spv::OpGraphConstantARM)
    {
        adjMatrix.m_GraphConstants.emplace_back(index);
    }
    else if (instruction->m_Opcode == spv::OpGraphSetOutputARM)
    {
        adjMatrix.m_OutputIndexes.emplace_back(index);
    }

    std::vector<unsigned int> &connectedIndexes = adjMatrix.m_AdjacencyMatrix[index];

    for (const auto &operand : instruction->m_Operands)
    {
        if (operand.m_Type == INSTRUCTION_POINTER && isAdjacencyMatrixOp(operand.m_InstructionPtr->m_Opcode) &&
            adjMatrix.m_InstructionToIndex.find(operand.m_InstructionPtr) == adjMatrix.m_InstructionToIndex.end())
        {
            RecursiveTraverse(operand.m_InstructionPtr, adjMatrix, ++index);
            connectedIndexes.push_back(adjMatrix.m_InstructionToIndex.at(operand.m_InstructionPtr));
        }
    }
}

AdjacencyMatrix::AdjacencyMatrix(const Module &module)
{
    // AdjacencyMatrix is a high level representation of the structure of the
    // SPIRV graph Where the graph only consists of the 4 SPIRV Op instructions
    // below
    const auto [inputsBegin, inputsEnd] = module.GetInstructionsOfType(spv::OpGraphInputARM);
    const auto [operatorsBegin, operatorsEnd] = module.GetInstructionsOfType(spv::OpExtInst);
    const auto [outputsBegin, outputsEnd] = module.GetInstructionsOfType(spv::OpGraphSetOutputARM);

    const auto inputSize = std::distance(inputsBegin, inputsEnd);
    const auto operatorSize = std::distance(operatorsBegin, operatorsEnd);
    const auto outputSize = std::distance(outputsBegin, outputsEnd);
    const auto totalSize = inputSize + operatorSize + outputSize;

    m_AdjacencyMatrix = std::vector(totalSize, std::vector<unsigned int>{});
    m_Instructions.reserve(totalSize);

    std::list<const Instruction *> sortedOutputs;
    for (auto it = outputsBegin; it != outputsEnd; ++it)
    {
        sortedOutputs.push_back(&(*it));
    }
    sortedOutputs.sort([](const Instruction *lhs, const Instruction *rhs) {
        return lhs->m_Operands[1].m_InstructionPtr->m_Operands[2].m_LiteralWord <
               rhs->m_Operands[1].m_InstructionPtr->m_Operands[2].m_LiteralWord;
    });

    // Traverse the graph from each output in order
    // If the graphs are equal
    unsigned int index = 0u;
    for (const auto instruction : sortedOutputs)
    {
        RecursiveTraverse(instruction, *this, index);
    }
}

std::vector<Instruction> AdjacencyMatrix::GetConnectedInstructions(const unsigned int index) const
{
    std::vector<Instruction> instructions;
    for (const auto &vec : m_AdjacencyMatrix)
    {
        for (const auto &instructionIndex : vec)
        {
            if (instructionIndex == index)
            {
                instructions.push_back(m_Instructions[index]);
            }
        }
    }
    return instructions;
}
} // namespace testutils
