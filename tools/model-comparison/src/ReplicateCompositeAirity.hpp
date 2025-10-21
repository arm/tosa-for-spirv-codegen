// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Instruction.hpp>
#include <ModuleComparator.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>

namespace testutils
{
using namespace spv;

static const Instruction* AsInstrPtr(const Operand& op)
{
    return (op.m_Type == INSTRUCTION_POINTER) ? op.m_InstructionPtr : nullptr;
}

static bool TryReadUintLiteralFromOpConstant(const Instruction& inst, int64_t& out)
{
    // SPIR-V: OpConstant <result-type> <result-id> <value-literal>
    // defensive: require at least 3 operands and the last one to be a literal word
    if (inst.m_Operands.size() < 3)
        return false;
    const Operand& lit = inst.m_Operands.back();
    if (lit.m_Type != LITERAL_WORD)
        return false;
    out = static_cast<int64_t>(lit.m_LiteralWord);
    return true;
}

// For OpTypeArray: length is an <id> to a constant; read it.
static bool TryReadArrayLengthFromOpTypeArray(const Instruction& typeInst, int64_t& outLen)
{
    // OpTypeArray <elem-type> <length-id>
    if (typeInst.m_Operands.size() < 2)
        return false;
    const Instruction* lengthConst = AsInstrPtr(typeInst.m_Operands[1]);
    if (!lengthConst)
        return false;
    return TryReadUintLiteralFromOpConstant(*lengthConst, outLen);
}

// For OpTypeVector: component count is a literal operand.
static bool TryReadVectorCount(const Instruction& typeInst, int64_t& outCount)
{
    // OpTypeVector <component-type> <component-count>
    if (typeInst.m_Operands.size() < 2)
        return false;
    const Operand& cnt = typeInst.m_Operands[1];
    if (cnt.m_Type != LITERAL_WORD)
        return false;
    outCount = static_cast<int64_t>(cnt.m_LiteralWord);
    return true;
}

// For OpTypeMatrix: column count is a literal operand (each column is a vector).
static bool TryReadMatrixColumnCount(const Instruction& typeInst, int64_t& outCols)
{
    // OpTypeMatrix <column-type(vec)> <column-count>
    if (typeInst.m_Operands.size() < 2)
        return false;
    const Operand& cnt = typeInst.m_Operands[1];
    if (cnt.m_Type != LITERAL_WORD)
        return false;
    outCols = static_cast<int64_t>(cnt.m_LiteralWord);
    return true;
}

// For OpTypeTensorARM: operands are (by the examples):
//   OpTypeTensorARM <elem-type-id> <rank-const-id> <shape-const-composite-id>
// - <rank-const-id> is an OpConstant (uint) giving the rank R.
// - <shape-const-composite-id> is an OpConstantComposite holding R dimension sizes (each an OpConstant).
// We treat the "constituent count" as the PRODUCT of the dimensions (total elements).
static bool TryReadTensorArityFromOpTypeTensorARM(const Instruction& typeInst, int64_t& outCount)
{
    if (typeInst.m_Operands.size() < 3)
        return false;

    const Instruction* rankConst = AsInstrPtr(typeInst.m_Operands[2]);
    const Instruction* shapeComp = AsInstrPtr(typeInst.m_Operands[3]);
    if (!rankConst || !shapeComp)
        return false;

    int64_t rank = 0;
    if (!TryReadUintLiteralFromOpConstant(*rankConst, rank))
        return false;
    if (shapeComp->m_Opcode != OpConstantComposite)
        return false;

    // OpConstantComposite for shape: operands are:
    // [0]=result-type, [1]=result-id, [2..] = R dimension constants
    if (shapeComp->m_Operands.size() < 2 + static_cast<size_t>(rank))
        return false;

    int64_t product = 1;
    for (size_t i = 2; i < 2 + static_cast<size_t>(rank); ++i)
    {
        const Instruction* dimConst = AsInstrPtr(shapeComp->m_Operands[i]);
        if (!dimConst)
            return false;
        int64_t dimVal = 0;
        if (!TryReadUintLiteralFromOpConstant(*dimConst, dimVal))
            return false;
        product *= dimVal;
    }

    outCount = product;
    return true;
}

// Compute the number of constituents implied by a homogeneous composite result type.
static int64_t ConstituentCountFromResultType(const Instruction& resultType)
{
    int64_t n = 0;

    switch (resultType.m_Opcode)
    {
        case OpTypeVector:
        {
            if (!TryReadVectorCount(resultType, n))
                throw std::runtime_error("Malformed OpTypeVector: missing component count");
            return n;
        }

        case OpTypeMatrix:
        {
            if (!TryReadMatrixColumnCount(resultType, n))
                throw std::runtime_error("Malformed OpTypeMatrix: missing column count");
            return n;
        }

        case OpTypeArray:
        {
            if (!TryReadArrayLengthFromOpTypeArray(resultType, n))
                throw std::runtime_error("Malformed OpTypeArray: could not resolve length constant");
            return n;
        }

        case OpTypeTensorARM:
        {
            if (!TryReadTensorArityFromOpTypeTensorARM(resultType, n))
                throw std::runtime_error("Malformed OpTypeTensorARM: could not resolve rank/shape");
            return n;
        }

        default: throw std::runtime_error("Unsupported result type for ConstituentCountFromResultType");
    }
}
} // namespace testutils
