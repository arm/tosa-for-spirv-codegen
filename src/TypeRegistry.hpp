//
// Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include <Operand.hpp>
#include <ResId.hpp>
#include <IdGenerator.hpp>
#include <Instruction.hpp>

#include <spirv/unified1/spirv.hpp>

namespace tosa2spirv
{

namespace spirvwriter
{

/// Forward declare Writer to prevent circular dependency in includes.
class Writer;

/// Class used to register Type declarations that we create to prevent disallowed duplication.
class TypeRegistry
{
public:
    /// Constructor which initialises the TypeRegistry's reference to the Writer and IdGenerator.
    /// This IdGenerator& should be passed down from the Writer's member variable.
    /// @param[in] writer Writer reference to be saved as member variable.
    /// @param[in] idGenerator IdGenerator reference to be saved as member variable.
    TypeRegistry(Writer& writer, graphbuilder::IdGenerator& idGenerator)
        : m_Writer(writer)
        , m_IdGenerator(idGenerator)
    {};

    /// Create an instruction for a new type and return a ResId or if it already exists return the previous ResId.
    /// @param[in] op spv::Op Opcode used to create and instruction.
    /// @param[in] operands Vector of Operands required by the Op.
    /// @return ResId Result Id of the Instruction that has been added.
    graphbuilder::ResId CreateType(spv::Op op, std::vector<Operand>& operands);

private:
    using OpAndOperandsPair = std::pair<spv::Op, std::vector<Operand>>;
    using TypeAndResIdPair = std::pair<OpAndOperandsPair, graphbuilder::ResId>;
    std::vector<TypeAndResIdPair> m_TypeEntries;
    graphbuilder::IdGenerator& m_IdGenerator;
    Writer& m_Writer;
};

} // spirvwriter namespace

} // tosa2spirv namespace