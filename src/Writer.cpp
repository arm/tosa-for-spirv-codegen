//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Writer.hpp>

#include <GraphConstant.hpp>
#include <GraphInfo.hpp>
#include <Instruction.hpp>
#include <Operand.hpp>

#include <limits>
#include <map>

namespace tosa2spirv
{

namespace spirvwriter
{

void Writer::WriteWord(const uint32_t& word)
{
    m_Binary.push_back(word);
}

void Writer::WriteWordAtPos(const uint32_t& word, const uint32_t& position)
{
    m_Binary.insert(m_Binary.begin() + position, word);
}

void Writer::WriteOperand(const Operand& operand)
{
    OperandType opType = operand.GetType();
    switch (opType)
    {
        case OperandType::NUMBER_ID:
        {
            WriteWord(operand.GetNumId());
            break;
        }
        case OperandType::LITERAL_STRING:
        {
            std::string str = operand.GetLiteralStr();
            const char* data = str.c_str();
            size_t numWords = str.size() / 4;

            for (unsigned Idx = 0; Idx < numWords; Idx++)
            {
                WriteWord(*reinterpret_cast<const uint32_t*>(&data[4 * Idx]));
            }

            uint32_t remainder = str.size() % 4;
            uint32_t lastWord = 0;
            if (remainder)
            {
                for (unsigned Idx = 0; Idx < remainder; Idx++)
                {
                    // This takes the remaining bytes that don't take up a full word.
                    // Then combines it using << 8 to place them into an uint32_t.
                    lastWord |= data[4 * numWords + Idx] << 8 * Idx;
                }
            }

            WriteWord(lastWord);
            break;
        }
        case OperandType::LITERAL_WORD:
        {
            WriteWord(operand.GetLiteralNum()[0]);
            break;
        }
        case OperandType::LITERAL_DWORD:
        {
            WriteWord(operand.GetLiteralNum()[0]);
            WriteWord(operand.GetLiteralNum()[1]);
            break;
        }
        default:
        {
            std::cerr << "WriteOperand(): Unsupported OperandType." << std::endl;
            break;
        }
    }
}

void Writer::WriteOperandAtPos(const Operand& operand, const uint32_t& position)
{
    // Check if it is possible to write into the binary vector.
    if (position > m_Binary.size())
    {
        std::cerr << "WriteOperandAtPos(): Current binary size exceeds position to write to." << std::endl;
        return;
    }

    OperandType opType = operand.GetType();
    switch (opType)
    {
        case OperandType::NUMBER_ID:
        {
            WriteWordAtPos(operand.GetNumId(), position);
            break;
        }
        case OperandType::LITERAL_STRING:
        {
            std::string str = operand.GetLiteralStr();
            const char* data = str.c_str();
            size_t numWords = str.size() / 4;

            for (unsigned Idx = 0; Idx < numWords; Idx++)
            {
                WriteWordAtPos(*reinterpret_cast<const uint32_t*>(&data[4 * Idx]), position);
            }

            uint32_t remainder = str.size() % 4;
            uint32_t lastWord = 0;
            if (remainder)
            {
                for (unsigned Idx = 0; Idx < remainder; Idx++)
                {
                    // This takes the remaining bytes that don't take up a full word.
                    // Then combines it using << 8 to place them into an uint32_t.
                    lastWord |= data[4 * numWords + Idx] << 8 * Idx;
                }
            }

            WriteWordAtPos(lastWord, position);
            break;
        }
        case OperandType::LITERAL_WORD:
        {
            WriteWordAtPos(operand.GetLiteralNum()[0], position);
            break;
        }
        case OperandType::LITERAL_DWORD:
        {
            WriteWordAtPos(operand.GetLiteralNum()[0], position);
            WriteWordAtPos(operand.GetLiteralNum()[1], position);
            break;
        }
        default:
        {
            std::cerr << "WriteOperand(): Unsupported OperandType." << std::endl;
            break;
        }
    }
}

void Writer::WriteWordCountAndOpcode(const Instruction& instruction)
{
    uint32_t word = instruction.GetOpcode();
    const uint32_t count = instruction.GetWordCount();

    // Check max limit of count doesn't exceed uint16 limit to shift below.
    if (count > std::numeric_limits<uint16_t>::max())
    {
        std::cerr << "WriteWordCountAndOpcode(): Count exceeds uint16 type limit." << std::endl;
        return;
    }

    // High 16 bit : Word Count
    // Low 16 bit  : Opcode
    word |= instruction.GetWordCount() << 16;
    WriteWord(word);
}

void Writer::WriteWordCountAndOpcodeAtPos(const Instruction& instruction, const uint32_t& position)
{
    // Check if it is possible to write into the binary vector.
    if (position > m_Binary.size())
    {
        std::cerr << "WriteWordCountAndOpcodeAtPos(): Current binary size exceeds position to write to." << std::endl;
        return;
    }

    uint32_t word = instruction.GetOpcode();
    const uint32_t count = instruction.GetWordCount();

    // Check max limit of count doesn't exceed uint16 limit to shift below.
    if (count > std::numeric_limits<uint16_t>::max())
    {
        std::cerr << "WriteWordCountAndOpcodeAtPos(): Count exceeds uint16 type limit." << std::endl;
        return;
    }

    // High 16 bit : Word Count
    // Low 16 bit  : Opcode
    word |= instruction.GetWordCount() << 16;
    WriteWordAtPos(word, position);
}

void Writer::WriteHeader()
{
    WriteWord(spv::MagicNumber);

    uint32_t minor = 0;
    switch (spv::Version)
    {
        case 0x00010000:
            minor = 0;
            break;
        case 0x00010100:
            minor = 1;
            break;
        case 0x00010200:
            minor = 2;
            break;
        case 0x00010300:
            minor = 3;
            break;
        case 0x00010400:
            minor = 4;
            break;
        case 0x00010500:
            minor = 5;
            break;
        case 0x00010600:
            minor = 6;
            break;
        default:
            std::cerr << "WriteHeader(): Unsupported SPIR-V version." << std::endl;
            break;
    }

    uint32_t version = (1 << 16) | (minor << 8);
    WriteWord(version);

    // Use the ARM vendor ID, which is 5.
    // This is defined in SPIRV-Headers/include/spirv/spir-v.xml
    uint32_t vendor = 5 << 16;
    WriteWord(vendor);

    // Set the bounds to 0 for now and save the position.
    // This will be updated later with the last ID set + 1.
    WriteWord(0);
    m_BoundsPosition = m_Binary.size() - 1;

    // Output the schema (reserved for use and must be 0)
    WriteWord(0);
}

void Writer::WriteBounds(const uint32_t& bound)
{
    m_Binary.at(m_BoundsPosition) = bound;
}

void Writer::WriteBinary(InstructionList& instructionList)
{
    for (const auto& instruction : instructionList)
    {
        const auto& operands = instruction.GetOperands();
        auto opCode = static_cast<spv::Op>(instruction.GetOpcode());

        switch (opCode)
        {
            case spv::OpLine:
            case spv::OpNoLine:
            case spv::OpUnreachable:
            case spv::OpCapability:
            case spv::OpExtension:
            case spv::OpMemoryModel:
            case spv::OpEntryPoint:
            case spv::OpExecutionMode:
            case spv::OpSource:
            case spv::OpMemberDecorate:
            case spv::OpBranch:
            case spv::OpBranchConditional:
            case spv::OpSelectionMerge:
            case spv::OpLoopMerge:
            case spv::OpStore:
            case spv::OpImageWrite:
            case spv::OpReturnValue:
            case spv::OpControlBarrier:
            case spv::OpMemoryBarrier:
            case spv::OpReturn:
            case spv::OpFunctionEnd:
            case spv::OpCopyMemory:
            case spv::OpAtomicStore:
            {
                WriteWordCountAndOpcode(instruction);
                for (uint32_t i = 0; i < operands.size(); i++)
                {
                    WriteOperand(operands[i]);
                }
                break;
            }
            // Seperated to make debug easier (same case as above)
            case spv::OpGraphSetOutputARM:
            case spv::OpGraphEndARM:
            case spv::OpGraphEntryPointARM:
            {
                WriteWordCountAndOpcode(instruction);
                for (uint32_t i = 0; i < operands.size(); i++)
                {
                    WriteOperand(operands[i]);
                }
                break;
            }
            case spv::OpTypeBool:
            case spv::OpTypeVoid:
            case spv::OpTypeSampler:
            case spv::OpLabel:
            case spv::OpExtInstImport:
            case spv::OpTypePointer:
            case spv::OpTypeRuntimeArray:
            case spv::OpTypeStruct:
            case spv::OpTypeImage:
            case spv::OpTypeSampledImage:
            case spv::OpTypeInt:
            case spv::OpTypeFloat:
            case spv::OpTypeArray:
            case spv::OpTypeVector:
            case spv::OpTypeFunction:
            case spv::OpTypeTensorARM:
            case spv::OpGraphARM:
            case spv::OpTypeGraphARM:
            case spv::OpString:
            {
                WriteWordCountAndOpcode(instruction);
                WriteWord(instruction.GetResultId().Get());
                for (uint32_t i = 0; i < operands.size(); i++)
                {
                    WriteOperand(operands[i]);
                }
                break;
            }
            case spv::OpFunction:
            case spv::OpFunctionParameter:
            case spv::OpAccessChain:
            case spv::OpPtrAccessChain:
            case spv::OpInBoundsAccessChain:
            case spv::OpUConvert:
            case spv::OpSConvert:
            case spv::OpConvertFToU:
            case spv::OpConvertFToS:
            case spv::OpConvertUToF:
            case spv::OpConvertSToF:
            case spv::OpFConvert:
            case spv::OpConvertPtrToU:
            case spv::OpConvertUToPtr:
            case spv::OpBitcast:
            case spv::OpFNegate:
            case spv::OpIAdd:
            case spv::OpIAddCarry:
            case spv::OpFAdd:
            case spv::OpISub:
            case spv::OpISubBorrow:
            case spv::OpFSub:
            case spv::OpIMul:
            case spv::OpFMul:
            case spv::OpUDiv:
            case spv::OpSDiv:
            case spv::OpFDiv:
            case spv::OpUMod:
            case spv::OpSRem:
            case spv::OpFRem:
            case spv::OpUMulExtended:
            case spv::OpSMulExtended:
            case spv::OpBitwiseOr:
            case spv::OpBitwiseXor:
            case spv::OpBitwiseAnd:
            case spv::OpNot:
            case spv::OpShiftLeftLogical:
            case spv::OpShiftRightLogical:
            case spv::OpShiftRightArithmetic:
            case spv::OpBitCount:
            case spv::OpCompositeConstruct:
            case spv::OpCompositeExtract:
            case spv::OpVectorExtractDynamic:
            case spv::OpCompositeInsert:
            case spv::OpCopyLogical:
            case spv::OpCopyObject:
            case spv::OpVectorInsertDynamic:
            case spv::OpVectorShuffle:
            case spv::OpIEqual:
            case spv::OpINotEqual:
            case spv::OpPtrEqual:
            case spv::OpPtrNotEqual:
            case spv::OpPtrDiff:
            case spv::OpUGreaterThan:
            case spv::OpUGreaterThanEqual:
            case spv::OpULessThan:
            case spv::OpULessThanEqual:
            case spv::OpSGreaterThan:
            case spv::OpSGreaterThanEqual:
            case spv::OpSLessThan:
            case spv::OpSLessThanEqual:
            case spv::OpFOrdEqual:
            case spv::OpFOrdGreaterThan:
            case spv::OpFOrdGreaterThanEqual:
            case spv::OpFOrdLessThan:
            case spv::OpFOrdLessThanEqual:
            case spv::OpFOrdNotEqual:
            case spv::OpFUnordEqual:
            case spv::OpFUnordGreaterThan:
            case spv::OpFUnordGreaterThanEqual:
            case spv::OpFUnordLessThan:
            case spv::OpFUnordLessThanEqual:
            case spv::OpFUnordNotEqual:
            case spv::OpExtInst:
            case spv::OpIsInf:
            case spv::OpIsNan:
            case spv::OpAny:
            case spv::OpAll:
            case spv::OpUndef:
            case spv::OpConstantNull:
            case spv::OpLogicalOr:
            case spv::OpLogicalAnd:
            case spv::OpLogicalNot:
            case spv::OpLogicalNotEqual:
            case spv::OpSpecConstantComposite:
            case spv::OpConstantTrue:
            case spv::OpConstantFalse:
            case spv::OpConstant:
            case spv::OpSpecConstant:
            case spv::OpVariable:
            case spv::OpFunctionCall:
            case spv::OpSampledImage:
            case spv::OpImageFetch:
            case spv::OpImageRead:
            case spv::OpImageSampleExplicitLod:
            case spv::OpImageQuerySize:
            case spv::OpImageQuerySizeLod:
            case spv::OpSelect:
            case spv::OpPhi:
            case spv::OpLoad:
            case spv::OpAtomicLoad:
            case spv::OpAtomicIAdd:
            case spv::OpAtomicISub:
            case spv::OpAtomicExchange:
            case spv::OpAtomicIIncrement:
            case spv::OpAtomicIDecrement:
            case spv::OpAtomicCompareExchange:
            case spv::OpAtomicUMin:
            case spv::OpAtomicSMin:
            case spv::OpAtomicUMax:
            case spv::OpAtomicSMax:
            case spv::OpAtomicAnd:
            case spv::OpAtomicOr:
            case spv::OpAtomicXor:
            case spv::OpDot:
            case spv::OpGroupNonUniformAll:
            case spv::OpGroupNonUniformAny:
            case spv::OpGroupNonUniformBroadcast:
            case spv::OpGroupNonUniformIAdd:
            case spv::OpGroupNonUniformFAdd:
            case spv::OpGroupNonUniformSMin:
            case spv::OpGroupNonUniformUMin:
            case spv::OpGroupNonUniformFMin:
            case spv::OpGroupNonUniformSMax:
            case spv::OpGroupNonUniformUMax:
            case spv::OpGroupNonUniformFMax:
            case spv::OpGraphConstantARM:
            case spv::OpConstantComposite:
            case spv::OpConstantCompositeReplicateEXT:
            case spv::OpGraphInputARM:
            {
                WriteWordCountAndOpcode(instruction);
                WriteOperand(operands[0]);
                WriteWord(instruction.GetResultId().Get());
                for (uint32_t i = 1; i < operands.size(); i++)
                {
                    WriteOperand(operands[i]);
                }
                break;
            }
            default:
            {
                std::cerr << "WriteBinary(): Unsupported SPIR-V opcode: " << int(opCode) << std::endl;
                break;
            }
        }
    }
}

void Writer::WriteBinary(Instruction& instruction)
{
    InstructionList list{instruction};
    WriteBinary(list);
}

void Writer::WriteCapabilities(const std::vector<spv::Capability>& capabilities)
{
    switch(m_Context->GetTOSAVersion())
    {
        case graphbuilder::TOSAVersion::v0_80:
        {
            break;
        }
        default:
            break;
    }

    InstructionList capabilityList;
    for (const auto& capability : capabilities)
    {
        Operand operand(capability);
        std::vector<Operand> capabilityOps({operand});
        Instruction capabilityInst(spv::Op::OpCapability, capabilityOps);
        capabilityList.push_back(capabilityInst);
    }
    WriteBinary(capabilityList);
}

void Writer::WriteExtension(const std::string& extension)
{
    Operand operand(LITERAL_STRING, extension);
    std::vector<Operand> extensionOps{operand};
    Instruction inst(spv::Op::OpExtension, extensionOps);
    WriteBinary(inst);
}

graphbuilder::ResId Writer::WriteExtInstSetImport(const std::string& instructionSet)
{
    Operand operand(LITERAL_STRING, instructionSet);
    std::vector<Operand> extendedInstSetOps{operand};

    auto tosaId = m_IdGenerator.GetNextId();

    Instruction inst(spv::Op::OpExtInstImport, tosaId, extendedInstSetOps);
    WriteBinary(inst);

    return tosaId;
}

void Writer::WriteMemoryModel(spv::AddressingModel addressingModel, spv::MemoryModel memoryModel)
{
    Operand operand1(addressingModel);
    Operand operand2(memoryModel);
    std::vector<Operand> memoryModelsOps{operand1, operand2};
    Instruction inst(spv::Op::OpMemoryModel, memoryModelsOps);
    WriteBinary(inst);

    // Decorations come after Memory Model so save position here to ensure it happens
    SaveDecorationPosition();
}

void Writer::SaveDecorationPosition()
{
    // Set the position for the decorations to start at.
    m_DecorationPosition = m_Binary.size();
}

void Writer::WriteDecoration(spv::Decoration decoration, uint32_t literalNum, graphbuilder::ResId id)
{
    Operand operand1(decoration);
    Operand operand2(literalNum);
    std::vector<Operand> decorateOps{operand1, operand2};

    Instruction instruction(spv::OpDecorate, id, decorateOps);
    WriteWordCountAndOpcodeAtPos(instruction, m_DecorationPosition++);
    WriteWordAtPos(id.Get(), m_DecorationPosition++);
    WriteOperandAtPos(decorateOps[0], m_DecorationPosition++);
    WriteOperandAtPos(decorateOps[1], m_DecorationPosition++);
}

void Writer::InitTypeMap()
{
    using namespace graphbuilder;

    using typesize = unsigned int;
    using signedness = unsigned int;

    // Construct a map of DataType to its equivalent Spir-V Op, size and signedness.
    std::map<DataType, std::tuple<spv::Op, typesize, signedness>> typeMap;
    std::tuple<spv::Op, typesize, signedness> int4Value(spv::Op::OpTypeInt, 4U, 0U);
    std::tuple<spv::Op, typesize, signedness> int8Value(spv::Op::OpTypeInt, 8U, 0U);
    std::tuple<spv::Op, typesize, signedness> int16Value(spv::Op::OpTypeInt, 16U, 0U);
    std::tuple<spv::Op, typesize, signedness> int32Value(spv::Op::OpTypeInt, 32U, 0U);
    std::tuple<spv::Op, typesize, signedness> uint8Value(spv::Op::OpTypeInt, 8U, 0U);
    std::tuple<spv::Op, typesize, signedness> uint16Value(spv::Op::OpTypeInt, 16U, 0U);
    std::tuple<spv::Op, typesize, signedness> uint32Value(spv::Op::OpTypeInt, 32U, 0U);
    std::tuple<spv::Op, typesize, signedness> float16Value(spv::Op::OpTypeFloat, 16U, 0U);
    std::tuple<spv::Op, typesize, signedness> float32Value(spv::Op::OpTypeFloat, 32U, 0U);
    std::tuple<spv::Op, typesize, signedness> bfloat16Value(spv::Op::OpTypeFloat, 16U, 2U);
    std::tuple<spv::Op, typesize, signedness> boolValue(spv::Op::OpTypeBool, 0U, 0U);

    m_TypeMap.emplace(DataType::int4_t, int4Value);
    m_TypeMap.emplace(DataType::int8_t, int8Value);
    m_TypeMap.emplace(DataType::int16_t, int16Value);
    m_TypeMap.emplace(DataType::int32_t, int32Value);
    m_TypeMap.emplace(DataType::uint8_t, uint8Value);
    m_TypeMap.emplace(DataType::uint16_t, uint16Value);
    m_TypeMap.emplace(DataType::uint32_t, uint32Value);
    m_TypeMap.emplace(DataType::float16_t, float16Value);
    m_TypeMap.emplace(DataType::float32_t, float32Value);
    m_TypeMap.emplace(DataType::bfloat16_t, bfloat16Value);
    m_TypeMap.emplace(DataType::bool_t, boolValue);
};

graphbuilder::ResId Writer::CreatePrimitiveTypeId(const graphbuilder::Tensor& tensor)
{
    auto opCode = std::get<0>(m_TypeMap[tensor.GetDataType()]);

    switch (opCode)
    {
        case spv::Op::OpTypeInt:
        {
            auto size = std::get<1>(m_TypeMap[tensor.GetDataType()]);
            Operand tensorTypeSize({size});
            auto sign = std::get<2>(m_TypeMap[tensor.GetDataType()]);
            Operand tensorTypeSignedness({sign});
            std::vector<Operand> tensorTypeSizeAndSignedness{tensorTypeSize, tensorTypeSignedness};
            return m_TypeRegistry.CreateType(spv::Op::OpTypeInt, tensorTypeSizeAndSignedness);
        }
        case spv::Op::OpTypeFloat:
        {
            auto size = std::get<1>(m_TypeMap[tensor.GetDataType()]);
            Operand tensorTypeSize({size});
            std::vector<Operand> tensorTypeSizeAndSignedness{tensorTypeSize};
            return m_TypeRegistry.CreateType(spv::Op::OpTypeFloat, tensorTypeSizeAndSignedness);
        }
        case spv::Op::OpTypeBool:
        {
            std::vector<Operand> tensorTypeSizeAndSignedness{};
            return m_TypeRegistry.CreateType(spv::Op::OpTypeBool, tensorTypeSizeAndSignedness);
        }
        default:
            throw std::invalid_argument("Writer::CreatePrimitiveTypeId(): Unsupported DataType.");
    }
}

spirvwriter::Operand Writer::WriteGraphInputOutputIndexConstant(const int32_t index)
{
    auto inputIndexTensor = graphbuilder::Tensor::CreateAttribute(
        graphbuilder::DataType::int32_t,
        std::vector<unsigned int>{1},
        graphbuilder::Tensor::ConvertInt32tToUint32t({index}));
    return WriteConstantInstruction(inputIndexTensor);
}

Operand Writer::WriteConstantInstruction(const graphbuilder::Tensor& tensor)
{
    using namespace graphbuilder;

    // Check for DataType Support:
    auto it = m_TypeMap.find(tensor.GetDataType());
    if (it == m_TypeMap.end())
    {
        throw std::invalid_argument("Writer::WriteTensorInstruction(): Tensor DataType not supported.");
    }

    // Create an instruction for the type of the tensor.
    auto constTypeId = CreatePrimitiveTypeId(tensor);
    Operand constTypeIdOp(OperandType::NUMBER_ID, constTypeId.Get());

    // Check if the tensor has values
    if (!tensor.HasValue())
    {
        throw std::invalid_argument("Writer::AddConstantInstruction(): Tensor does not have constant values.");
    }

    // Check if the tensor has more than one value, because we only support creating single constant value
    // For more than one values, it could be a Vector instead.
    if (tensor.GetNumElements() != 1)
    {
        throw std::invalid_argument("Writer::AddConstantInstruction(): Tensor has more than one value.");
    }

    // Get tensor values from the tensor pointer
    auto tensorValues = tensor.GetTensorValuesUint32t();

    // Create Constant value instruction for scalar value inside the vector e.g the 2 in {2,2}.
    Operand constValueOp({tensorValues[0]});
    ResId constValue;

    if (tensor.GetDataType() == DataType::bool_t)
    {
        std::vector<Operand> constTypeAndVectorValue{constTypeIdOp};
        if (tensorValues.at(0) == 1)
        {
            constValue = m_TypeRegistry.CreateType(spv::Op::OpConstantTrue, constTypeAndVectorValue);
        }
        else
        {
            constValue = m_TypeRegistry.CreateType(spv::Op::OpConstantFalse, constTypeAndVectorValue);
        }
    }
    else
    {
        std::vector<Operand> constTypeAndVectorValue{constTypeIdOp, constValueOp};
        constValue = m_TypeRegistry.CreateType(spv::Op::OpConstant, constTypeAndVectorValue);
    }

    return {OperandType::NUMBER_ID, constValue.Get()};
}

Operand Writer::WriteTensorInstruction(const graphbuilder::Tensor& tensor,
                                       AdditionalType typeName,
                                       bool tensorShapeAsArray)
{
    using namespace graphbuilder;

    // Check for DataType Support:
    auto it = m_TypeMap.find(tensor.GetDataType());
    if (it == m_TypeMap.end())
    {
        throw std::invalid_argument("Writer::WriteTensorInstruction(): Tensor DataType not supported.");
    }

    // Create an instruction for the type of the tensor.
    auto tensorTypeId = CreatePrimitiveTypeId(tensor);

    graphbuilder::ResId tensorId;

    if (typeName == AdditionalType::array)
    {
        tensorId = Writer::WriteArrayTensorInstruction(tensor, tensorTypeId, tensorShapeAsArray);
    }
    else
    {
        // Type of the rank of the shape
        Operand rankTypeSize({32U});
        Operand rankTypeSignedness({0U});
        std::vector<Operand> rankTypeSizeAndSignedness{rankTypeSize, rankTypeSignedness};
        auto rankTypeId = m_TypeRegistry.CreateType(spv::Op::OpTypeInt, rankTypeSizeAndSignedness);

        // Rank of tensor shape as a constant
        Operand rankTypeIdOp(OperandType::NUMBER_ID, rankTypeId.Get());
        Operand rankValue({static_cast<unsigned int>(tensor.GetTensorShape().size())});
        std::vector<Operand> rankTypeIdAndValue{rankTypeIdOp, rankValue};
        auto tensorRank = m_TypeRegistry.CreateType(spv::Op::OpConstant, rankTypeIdAndValue);

        Operand tensorTypeIdOp(OperandType::NUMBER_ID, tensorTypeId.Get());
        Operand tensorRankIdOp(OperandType::NUMBER_ID, tensorRank.Get());

        // Also create array for tensor shape
        // Shape is the optional shape of the tensor.
        // Shape must be a constant instruction of type OpTypeArray whose Element Type is an integer
        // and whose Length is equal to Rank.
        // Elements of that array must all be greater than 0.
        // %int = OpTypeInt 32 1
        Operand shapeTypeSize({32U});
        Operand shapeTypeSignedness({0U});
        std::vector<Operand> shapeTypeSizeAndSignedness{shapeTypeSize, shapeTypeSignedness};
        auto tensorShapeTypeId = m_TypeRegistry.CreateType(spv::Op::OpTypeInt, shapeTypeSizeAndSignedness);
        // Create the array of type Int32 (tensorShapeTypeId)
        auto TensorShapeId = WriteArrayCompoundInstruction(tensor, tensorShapeTypeId, true)[2];

        std::vector<Operand> tensorTypeIdOpRankIdShapeIdVect{tensorTypeIdOp, tensorRankIdOp, TensorShapeId};
        tensorId = m_TypeRegistry.CreateType(spv::Op::OpTypeTensorARM, tensorTypeIdOpRankIdShapeIdVect);
    }

    return {OperandType::NUMBER_ID, tensorId.Get()};
}

std::vector<Operand> Writer::WriteArrayCompoundInstruction(const graphbuilder::Tensor& tensor,
                                                           graphbuilder::ResId tensorTypeId,
                                                           bool tensorShapeAsArray)
{
    using namespace graphbuilder;

    // Check for DataType Support:
    auto it = m_TypeMap.find(tensor.GetDataType());
    if (it == m_TypeMap.end())
    {
        throw std::invalid_argument("Writer::WriteArrayCompoundInstruction(): Tensor DataType not supported.");
    }

    if (tensorTypeId == 0)
    {
        tensorTypeId = CreatePrimitiveTypeId(tensor);
    }

    // Tensor Shape
    Operand shapeTypeSize({32U});
    Operand shapeTypeSignedness({0U});
    std::vector<Operand> shapeTypeSizeAndSignedness{shapeTypeSize, shapeTypeSignedness};
    auto tensorShapeTypeId = m_TypeRegistry.CreateType(spv::Op::OpTypeInt, shapeTypeSizeAndSignedness);

    // Array Length
    Operand tensorTypeIdOp(OperandType::NUMBER_ID, tensorTypeId.Get());

    // Rank is Signed Int
    // Type of the rank of the shape
    Operand rankTypeSize({32U});
    Operand rankTypeSignedness({0U});
    std::vector<Operand> rankTypeSizeAndSignedness{rankTypeSize, rankTypeSignedness};
    auto rankTypeId = m_TypeRegistry.CreateType(spv::Op::OpTypeInt, rankTypeSizeAndSignedness);

    // Rank as a constant
    Operand rankTypeIdOp(OperandType::NUMBER_ID, rankTypeId.Get());
    if (tensor.GetTensorShape().empty())
    {
        throw std::invalid_argument(
                "Writer::WriteArrayCompoundInstruction(): Arrays passed in via Tensor must not have empty Shape.");
    }
    if (tensor.GetTensorShape().size() != 1 && !tensorShapeAsArray)
    {
        throw std::invalid_argument(
                "Writer::WriteArrayCompoundInstruction(): Arrays must have a Shape of size = 1.");
    }

    unsigned int arrayLengthValue = tensorShapeAsArray ? tensor.GetTensorShape().size()
                                                       : static_cast<unsigned int>(tensor.GetTensorShape()[0]);
    Operand arrayLength({arrayLengthValue});
    std::vector<Operand> tensorArrayLength{rankTypeIdOp, arrayLength};
    auto arrayLengthId = m_TypeRegistry.CreateType(spv::Op::OpConstant, tensorArrayLength);

    // Array Type & Length
    Operand arrayLengthIdOp(OperandType::NUMBER_ID, arrayLengthId.Get());

    std::vector<Operand> typeIdLengthIdCompositeIdVect{rankTypeIdOp, arrayLengthIdOp};
    auto arrayTypeId = m_TypeRegistry.CreateType(spv::Op::OpTypeArray, typeIdLengthIdCompositeIdVect);

    // Create the Array
    Operand arrayTypeIdOp(OperandType::NUMBER_ID, arrayTypeId.Get());
    std::vector<Operand> componentValues{arrayTypeIdOp};
    auto arraySize = tensor.GetTensorShape()[0];

    // If creating an array to use as tensor's shape then use shape directly
    if (tensorShapeAsArray)
    {
        for (unsigned int scalar : tensor.GetTensorShape())
        {
            // Create Constant value instruction for scalar value inside the vector e.g the 2 in {2,2}.
            Operand vectorValue({static_cast<unsigned int>(scalar)});

            std::vector<Operand> tensorTypeAndVectorValue{rankTypeIdOp, vectorValue};

            auto vectorScalarValueId = m_TypeRegistry.CreateType(spv::Op::OpConstant, tensorTypeAndVectorValue);
            Operand vectorScalarValueIdOp(OperandType::NUMBER_ID, vectorScalarValueId.Get());

            // While we have the vectorScalarValue add it to the components of the Vector.
            componentValues.push_back(vectorScalarValueIdOp);
        }
    }
    // If using passing values in via a tensor, use the values passed in via TensorValuesPtr, not the shape
    else
    {
        for (auto scalarIndex = 0; scalarIndex < arraySize; scalarIndex++)
        {
            // Create Constant value instruction for scalar value inside the vector e.g the 2 in {2,2}.
            Operand arrayValue({static_cast<unsigned int>(tensor.GetTensorValuesUint32t()[scalarIndex])});

            std::vector<Operand> tensorTypeAndVectorValue{rankTypeIdOp, arrayValue};

            auto arrayScalarValueId = m_TypeRegistry.CreateType(spv::Op::OpConstant, tensorTypeAndVectorValue);
            Operand arrayScalarValueIdOp(OperandType::NUMBER_ID, arrayScalarValueId.Get());

            // While we have the Array Scalar Value add it to the components of the Array.
            componentValues.push_back(arrayScalarValueIdOp);
        }
    }

    auto compositeId = m_TypeRegistry.CreateType(spv::Op::OpConstantComposite, componentValues);

    Operand compositeIdOp(OperandType::NUMBER_ID, compositeId.Get());

    typeIdLengthIdCompositeIdVect.push_back(compositeIdOp);

    // hack to make tensor type correct but use int32 for array constituents i.e. same as rank
    typeIdLengthIdCompositeIdVect[0] = tensorTypeIdOp;

    return typeIdLengthIdCompositeIdVect;
}

Operand Writer::WriteTensorCompoundInstruction(const graphbuilder::Tensor& tensor, graphbuilder::ResId tensorTypeId)
{
    using namespace graphbuilder;

    // Check for DataType Support:
    auto it = m_TypeMap.find(tensor.GetDataType());
    if (it == m_TypeMap.end())
    {
        throw std::invalid_argument("Writer::WriteTensorCompoundInstruction(): Tensor DataType not supported.");
    }

    if (tensor.GetCategory() == Category::Array &&
        tensor.GetCategory() == Category::Ranked)
    {
        tensorTypeId = CreatePrimitiveTypeId(tensor);
    }

    if (tensor.GetTensorShape().empty())
    {
        throw std::invalid_argument(
            "Writer::WriteTensorCompoundInstruction(): Arrays passed in via Tensor must not have empty Shape.");
    }
    if (tensor.GetTensorShape().size() != 1)
    {
        throw std::invalid_argument(
            "Writer::WriteTensorCompoundInstruction(): Arrays must have a Shape of size = 1.");
    }

    if (tensor.GetTensorValuesUint32t().empty())
    {
        throw std::invalid_argument(
            "Writer::WriteTensorCompoundInstruction(): Tensor's values cannot be empty.");
    }

    // Call the WriteTensorInstruction to create an Operand like the following:
    // %XX = OpTypeTensorARM %uint %uint_X %YY
    // %YY = OpConstantComposite % _arr_uint_uint_X % uint_X
    auto opTypeTensorArm = WriteTensorInstruction(tensor);

    // Add the above Operand as the first element in a vector of Operands called componentValues:
    std::vector<Operand> componentValues{opTypeTensorArm};

    auto arraySize = tensor.GetTensorShape()[0];

    Operand rankTypeSize({32U});
    Operand rankTypeSignedness({0U});
    std::vector<Operand> rankTypeSizeAndSignedness{rankTypeSize, rankTypeSignedness};
    auto rankTypeId = m_TypeRegistry.CreateType(spv::Op::OpTypeInt, rankTypeSizeAndSignedness);
    Operand rankTypeIdOp(OperandType::NUMBER_ID, rankTypeId.Get());

    // Check if the values of the array are zero
    // If they are zero we will return the component values as type OpConstantNull with no values added
    // If the values are not zero we will add them to the vector of Operands called componentValues
    // and return as type OpConstantComposite
    auto opType = spv::Op::OpConstantComposite;
    bool nullValues = true;
    for (auto scalarIndex = 0; scalarIndex < arraySize; ++scalarIndex)
    {
        if (tensor.GetTensorValuesUint32t().at(scalarIndex) != 0)
        {
            nullValues = false;
            break;
        }
    }

    if (!nullValues)
    {
        // If all values in the tensor compound are the same we can use OpConstantCompositeReplicateEXT optimization
        // to save repeating the values and instead mention the value once.
        bool replicatedComposite = true;
        auto firstValue = tensor.GetTensorValuesUint32t()[0];
        for (auto scalarIndex = 1; scalarIndex < arraySize; ++scalarIndex)
        {
            if (tensor.GetTensorValuesUint32t()[scalarIndex] != firstValue)
            {
                replicatedComposite = false;
                break;
            }
        }

        if (replicatedComposite)
        {
            // Only use first value
            arraySize = 1;
            opType = spv::Op::OpConstantCompositeReplicateEXT;
        }

        for (auto scalarIndex = 0; scalarIndex < arraySize; scalarIndex++)
        {
            // Create Constant value instruction for scalar value inside the array e.g the 2 in {2,2}.
            Operand arrayValue({tensor.GetTensorValuesUint32t().at(scalarIndex)});

            std::vector<Operand> tensorTypeAndVectorValue{rankTypeIdOp, arrayValue};

            auto arrayScalarValueId = m_TypeRegistry.CreateType(spv::Op::OpConstant, tensorTypeAndVectorValue);
            Operand arrayScalarValueIdOp(OperandType::NUMBER_ID, arrayScalarValueId.Get());

            // While we have the Array Scalar Value add it to the components of the Array.
            componentValues.push_back(arrayScalarValueIdOp);
        }
    }
    else
    {
        opType = spv::Op::OpConstantNull;
    }

    auto composite = m_TypeRegistry.CreateType(opType, componentValues);
    return {OperandType::NUMBER_ID, composite.Get()};
}

graphbuilder::ResId Writer::WriteArrayTensorInstruction(const graphbuilder::Tensor& tensor,
                                                        graphbuilder::ResId tensorTypeId,
                                                        bool tensorShapeAsArray)
{
    std::vector<Operand> typeIdLengthIdCompositeIdVect = WriteArrayCompoundInstruction(tensor,
                                                                                       tensorTypeId,
                                                                                       tensorShapeAsArray);

    return m_TypeRegistry.CreateType(spv::Op::OpTypeTensorARM, typeIdLengthIdCompositeIdVect);
}

Operand Writer::WriteTypeStructInstruction(std::vector<Operand>& TensorTypeIdOpVect)
{
    auto typeStructId =  m_TypeRegistry.CreateType(spv::Op::OpTypeStruct, TensorTypeIdOpVect);
    return {OperandType::NUMBER_ID, typeStructId.Get()};
}

Instruction Writer::WriteCompositeExtractInstruction(const graphbuilder::ResId operatorId,
                                                     const graphbuilder::ResId compositeExtractId,
                                                     Operand& TensorTypeIdOp,
                                                     unsigned int index)
{
    // Create the index Operand and Operand vector.
    Operand operatorOp(OperandType::NUMBER_ID, operatorId.Get());
    Operand indexOp(index);
    std::vector<Operand> compositeExtractVec{TensorTypeIdOp, operatorOp, indexOp};
    // Get the ResId for the OpCompositeExtract Operand
    // Return instruction for OpCompositeExtract to be added to the graph.m_LayerList
    return {spv::Op::OpCompositeExtract, compositeExtractId, compositeExtractVec};
}

Operand Writer::AddGraphInputInstruction(InstructionList& list, const Operand& inputTensorIdOp)
{
    using namespace graphbuilder;

    // Create our result id for Graph Input we will return.
    ResId graphInputId(GetNextId());
    Operand graphInputIdOp(OperandType::NUMBER_ID, graphInputId.Get());

    // Add both of the above to vector of Operands to be passed to instruction.
    std::vector<Operand> inputTensorIdOpAndGraphInputIdOp{inputTensorIdOp, graphInputIdOp};

    // Add instruction for OpGraphInputARM with its arguments Input Tensors ID and Graph Input Id.
    list.push_back(std::move(Instruction(spv::Op::OpGraphInputARM, inputTensorIdOpAndGraphInputIdOp)));
    return graphInputIdOp;
}

void Writer::WriteGraphSetOutputInstruction(const std::vector<graphbuilder::GraphIOInfo>& outputs)
{
    int32_t index = 0;
    for (const auto& output : outputs)
    {
        auto outputIndexOp = WriteGraphInputOutputIndexConstant(index);
        std::vector<Operand> graphOutputIdOps{output.m_Layer, outputIndexOp};

        // OpGraphSetOutputARM is an atomic instuction so can be written in place.
        // It's called by simply using its opcode and the list of IDs belonging to the layers which will be graph outputs.
        Instruction inst(spv::Op::OpGraphSetOutputARM, graphOutputIdOps);
        WriteBinary(inst);

        index++;
    }
}

Operand Writer::AddPointerInstruction(InstructionList& list, const graphbuilder::Tensor& tensor)
{
    Operand uniformConstantOp({spv::StorageClassUniformConstant});

    auto tensorIdOp = WriteTensorInstruction(tensor);

    std::vector<Operand> uniformConstantAndPointerType{uniformConstantOp, tensorIdOp};

    // Create the pointer to tensor type.
    auto pointerTypeId = m_TypeRegistry.CreateType(spv::Op::OpTypePointer, uniformConstantAndPointerType);

    // Create the OpVariable
    // note: we don't create OpVariable using TypeRegistry as duplicates can be valid:
    //  %input_tensor_ptr = OpVariable %ptr_UniformConstant_int8tensor UniformConstant
    // %output_tensor_ptr = OpVariable %ptr_UniformConstant_int8tensor UniformConstant
    graphbuilder::ResId variableId(GetNextId());
    Operand pointerIdOp(OperandType::NUMBER_ID, pointerTypeId.Get());
    std::vector<Operand> opTypePointerResIdAndUniformConstant{pointerIdOp, uniformConstantOp};
    list.emplace_back(spv::Op::OpVariable, variableId, opTypePointerResIdAndUniformConstant);

    Operand variableIdOp(OperandType::NUMBER_ID, variableId.Get());
    return variableIdOp;
}

Operand Writer::WriteGraphTypeInstruction(unsigned int numInputs,
                                          const std::vector<graphbuilder::GraphIOInfo>& inputs,
                                          const std::vector<graphbuilder::GraphIOInfo>& outputs)
{
    // Create a Result Id for our resulting graph type.
    graphbuilder::ResId graphTypeId(GetNextId());
    Operand graphTypeIdOp(OperandType::NUMBER_ID, graphTypeId.Get());

    Operand numInputsOp({numInputs});
    std::vector<Operand> graphTypeOps{numInputsOp};

    for (const auto& input : inputs)
    {
        graphTypeOps.push_back(input.m_Tensor);
    }

    for (const auto& output : outputs)
    {
        graphTypeOps.push_back(output.m_Tensor);
    }

    Instruction graphTypeInst(spv::Op::OpTypeGraphARM, graphTypeId, graphTypeOps);
    WriteBinary({graphTypeInst});
    return graphTypeIdOp;
}

void Writer::WriteGraphEntryPointInstruction(const graphbuilder::ResId& graphId,
                                             const std::string& name,
                                             const std::vector<graphbuilder::GraphIOInfo>& inputs,
                                             const std::vector<graphbuilder::GraphIOInfo>& outputs)
{
    Operand opGraphId(NUMBER_ID, graphId.Get());
    Operand graphName(LITERAL_STRING, name);
    std::vector<Operand> graphEntryPointOps{opGraphId, graphName};

    for (const auto& input : inputs)
    {
        graphEntryPointOps.push_back(input.m_Pointer);
    }

    for (const auto& output : outputs)
    {
        graphEntryPointOps.push_back(output.m_Pointer);
    }

    Instruction graphEntryPointInst(spv::Op::OpGraphEntryPointARM, graphEntryPointOps);
    WriteBinary({graphEntryPointInst});
}

void Writer::WriteGraphInstruction(const graphbuilder::ResId& graphId, const Operand& opGraphTypeId)
{
    Operand opGraphId(NUMBER_ID, graphId.Get());
    std::vector<Operand> graphOps{opGraphId};
    Instruction graphInst(spv::Op::OpGraphARM, opGraphTypeId.GetNumId(), graphOps);
    WriteBinary({graphInst});
}

void Writer::WriteGraphEndInstruction()
{
    Instruction graphEndInst(spv::Op::OpGraphEndARM);
    WriteBinary({graphEndInst});
}

Operand Writer::WriteGraphConstantInstruction(graphbuilder::GraphConstant& graphConstant, AdditionalType typeName)
{
    // OpGraphConstantARM
    // Declare a graph constant.
    // Result Type must be an OpTypeTensorARM.
    // GraphConstantID must be a 32-bit integer.
    const graphbuilder::Tensor& tensor = graphConstant.GetTensor();
        // Result Id for the graph constant.
    // Create Tensor Type and get the ID.
    auto id = WriteTensorInstruction(tensor, typeName, true);

    graphbuilder::ResId graphConstantId = tensor.GetResId();

    // Set new Result Id on existing graph constant object
    auto tensorGraphInstruction = graphConstant;

    // Need to get the Id provided by the user.
    uint32_t graphConstantIdentifier = graphConstant.GetGraphConstantId();

    // If User has not defined the GraphConstant dataType and Shape then infer.
    if (!graphConstant.GetDataType().has_value() ||
        !graphConstant.GetTensorShape().has_value())
    {
        graphConstant.SetDataType(tensor.GetDataType());
        graphConstant.SetTensorShape(tensor.GetTensorShape());
    }
    // Check for dataType or Shape mismatch
    else if (graphConstant.GetDataType() != tensor.GetDataType() &&
             graphConstant.GetTensorShape() != tensor.GetTensorShape())
    {
        throw std::invalid_argument(
                "WriteGraphConstantInstruction: Tensors has an assigned GraphConstant with mismatching Shape/DataType");
    }

    // Id for the Index of the graph constant.
    Operand opGraphId(NUMBER_ID, graphConstantIdentifier);
    std::vector<Operand> graphConstantOps{id, opGraphId};

    // Initialize the instruction with OpCode, Graph Constant ResId, Tensor Type ID and Graph Constant Index Id.
    Instruction graphConstantInst(spv::Op::OpGraphConstantARM, graphConstantId, graphConstantOps);

    // Write Graph Constant Instruction to binary.
    WriteBinary({graphConstantInst});

    return {OperandType::NUMBER_ID, graphConstant.GetGraphConstantResId()};
}


} // spirvwriter namespace

} // tosa2spirv namespace
