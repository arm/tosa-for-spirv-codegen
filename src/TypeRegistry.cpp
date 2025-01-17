//
// Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "TypeRegistry.hpp"
#include <Writer.hpp>

#include <stdexcept>


namespace tosa2spirv
{

namespace spirvwriter
{

graphbuilder::ResId TypeRegistry::CreateType(spv::Op op, std::vector<Operand>& operands)
{
    switch (op)
    {
       case OpTypeInt:
       case OpTypeFloat:
       case OpTypeBool:
       case OpTypeTensorARM:
       case OpTypeVector:
       case OpTypeArray:
       case OpConstant:
       case OpConstantNull:
       case OpConstantComposite:
       case OpConstantTrue:
       case OpConstantFalse:
       case OpGraphInputARM:
       case OpTypePointer:
       case OpConstantCompositeReplicateEXT:
       case OpTypeStruct:
       case OpCompositeExtract:
          break;
       default: ;
          throw std::invalid_argument("TypeRegistry::CreateType: Unsupported Opcode.");
    }

    // Loop over all entries
    for (const auto& typeAndResIdPair : m_TypeEntries)
    {
        // Check for matching OpCode
        if (typeAndResIdPair.first.first == op)
        {
            if (op == spv::OpTypeBool)
            {
                return typeAndResIdPair.second;
            }
            // Check if all Operands are the same
            if (typeAndResIdPair.first.second == operands)
            {
                // Opcode with matching operands exists so return the previously created Instruction's ResId
                return typeAndResIdPair.second;
            }
        }
    }

    // No matching Opcode so create new entry
    graphbuilder::ResId id(m_IdGenerator.GetNextId());
    Instruction inst(op, id.Get(), operands);

    // Add new entry
    m_TypeEntries.emplace_back(std::make_pair(op, operands), id);

    // Write to binary
    m_Writer.WriteBinary({inst});
    return id;
}

} // spirvwriter namespace

} // tosa2spirv namespace