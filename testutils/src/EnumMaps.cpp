//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "EnumMaps.hpp"

#include <Instruction.hpp>
#include <Operand.hpp>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

// Maps seperated into their own functions to enable test coverage
static const std::unordered_map<std::string, spv::Op>& getInstructionMap()
{
    static const auto mapInstance = []() {
        std::unordered_map<std::string, spv::Op> tmp;
        tmp.reserve(29);
        tmp.emplace("OpCapability", spv::Op::OpCapability);
        tmp.emplace("OpExtension", spv::Op::OpExtension);
        tmp.emplace("OpMemoryModel", spv::Op::OpMemoryModel);
        tmp.emplace("OpTypeInt", spv::Op::OpTypeInt);
        tmp.emplace("OpTypeFloat", spv::Op::OpTypeFloat);
        tmp.emplace("OpTypeBool", spv::Op::OpTypeBool);
        tmp.emplace("OpConstantTrue", spv::Op::OpConstantTrue);
        tmp.emplace("OpConstantFalse", spv::Op::OpConstantFalse);
        tmp.emplace("OpConstant", spv::Op::OpConstant);
        tmp.emplace("OpConstantComposite", spv::Op::OpConstantComposite);
        tmp.emplace("OpConstantNull", spv::Op::OpConstantNull);
        tmp.emplace("OpConstantCompositeReplicateEXT", spv::Op::OpConstantCompositeReplicateEXT);
        tmp.emplace("OpTypeArray", spv::Op::OpTypeArray);
        tmp.emplace("OpTypeTensorARM", spv::Op::OpTypeTensorARM);
        tmp.emplace("OpTypeStruct", spv::Op::OpTypeStruct);
        tmp.emplace("OpExtInst", spv::Op::OpExtInst);
        tmp.emplace("OpCompositeExtract", spv::Op::OpCompositeExtract);
        tmp.emplace("OpTypeGraphARM", spv::Op::OpTypeGraphARM);
        tmp.emplace("OpExtInstImport", spv::Op::OpExtInstImport);
        tmp.emplace("OpGraphInputARM", spv::Op::OpGraphInputARM);
        tmp.emplace("OpGraphSetOutputARM", spv::Op::OpGraphSetOutputARM);
        tmp.emplace("OpGraphARM", spv::Op::OpGraphARM);
        tmp.emplace("OpTypePointer", spv::Op::OpTypePointer);
        tmp.emplace("OpVariable", spv::Op::OpVariable);
        tmp.emplace("OpDecorate", spv::Op::OpDecorate);
        tmp.emplace("OpGraphEntryPointARM", spv::Op::OpGraphEntryPointARM);
        tmp.emplace("OpGraphEndARM", spv::Op::OpGraphEndARM);
        tmp.emplace("OpGraphConstantARM", spv::Op::OpGraphConstantARM);
        tmp.emplace("OpName", spv::Op::OpName);
        return tmp;
    }();
    return mapInstance;
}

spv::Op GetOpEnum(const std::string& instruction)
{
    const auto& instructionMap = getInstructionMap();
    if (const auto it = instructionMap.find(instruction); it != instructionMap.end())
    {
        return it->second;
    }
    throw std::invalid_argument("Unsupported instruction: " + instruction);
}

static const std::unordered_map<spv::Op, std::string>& getOpToStringMap()
{
    static const auto mapInstance = []() {
        std::unordered_map<spv::Op, std::string> tmp;
        tmp.reserve(29);
        tmp.emplace(spv::Op::OpCapability, "OpCapability");
        tmp.emplace(spv::Op::OpExtension, "OpExtension");
        tmp.emplace(spv::Op::OpMemoryModel, "OpMemoryModel");
        tmp.emplace(spv::Op::OpTypeInt, "OpTypeInt");
        tmp.emplace(spv::Op::OpTypeFloat, "OpTypeFloat");
        tmp.emplace(spv::Op::OpTypeBool, "OpTypeBool");
        tmp.emplace(spv::Op::OpConstantTrue, "OpConstantTrue");
        tmp.emplace(spv::Op::OpConstantFalse, "OpConstantFalse");
        tmp.emplace(spv::Op::OpConstant, "OpConstant");
        tmp.emplace(spv::Op::OpConstantComposite, "OpConstantComposite");
        tmp.emplace(spv::Op::OpConstantNull, "OpConstantNull");
        tmp.emplace(spv::Op::OpConstantCompositeReplicateEXT, "OpConstantCompositeReplicateEXT");
        tmp.emplace(spv::Op::OpTypeArray, "OpTypeArray");
        tmp.emplace(spv::Op::OpTypeTensorARM, "OpTypeTensorARM");
        tmp.emplace(spv::Op::OpTypeStruct, "OpTypeStruct");
        tmp.emplace(spv::Op::OpExtInst, "OpExtInst");
        tmp.emplace(spv::Op::OpCompositeExtract, "OpCompositeExtract");
        tmp.emplace(spv::Op::OpTypeGraphARM, "OpTypeGraphARM");
        tmp.emplace(spv::Op::OpExtInstImport, "OpExtInstImport");
        tmp.emplace(spv::Op::OpGraphInputARM, "OpGraphInputARM");
        tmp.emplace(spv::Op::OpGraphSetOutputARM, "OpGraphSetOutputARM");
        tmp.emplace(spv::Op::OpGraphARM, "OpGraphARM");
        tmp.emplace(spv::Op::OpTypePointer, "OpTypePointer");
        tmp.emplace(spv::Op::OpVariable, "OpVariable");
        tmp.emplace(spv::Op::OpDecorate, "OpDecorate");
        tmp.emplace(spv::Op::OpGraphEntryPointARM, "OpGraphEntryPointARM");
        tmp.emplace(spv::Op::OpGraphEndARM, "OpGraphEndARM");
        tmp.emplace(spv::Op::OpGraphConstantARM, "OpGraphConstantARM");
        tmp.emplace(spv::Op::OpName, "OpName");
        return tmp;
    }();
    return mapInstance;
}

std::string GetOpString(const spv::Op op)
{
    const auto& opToString = getOpToStringMap();
    if (const auto it = opToString.find(op); it != opToString.end())
    {
        return it->second;
    }
    throw std::invalid_argument("Unsupported op enum value");
}

static const std::unordered_map<std::string, TOSAInstructions>& getTosaInstructionsMap()
{
    static const auto mapInstance = []() {
        std::unordered_map<std::string, TOSAInstructions> tmp;
        tmp.reserve(66);
        tmp.emplace("ARGMAX", TOSAARGMAX);
        tmp.emplace("AVG_POOL2D", TOSAAVG_POOL2D);
        tmp.emplace("CONV2D", TOSACONV2D);
        tmp.emplace("CONV3D", TOSACONV3D);
        tmp.emplace("DEPTHWISE_CONV2D", TOSADEPTHWISE_CONV2D);
        tmp.emplace("FFT2D", TOSAFFT2D);
        tmp.emplace("FULLY_CONNECTED", TOSAFULLY_CONNECTED);
        tmp.emplace("MATMUL", TOSAMATMUL);
        tmp.emplace("MAX_POOL2D", TOSAMAX_POOL2D);
        tmp.emplace("RFFT2D", TOSARFFT2D);
        tmp.emplace("TRANSPOSE_CONV2D", TOSATRANSPOSE_CONV2D);
        tmp.emplace("CLAMP", TOSACLAMP);
        tmp.emplace("ERF", TOSAERF);
        tmp.emplace("SIGMOID", TOSASIGMOID);
        tmp.emplace("TANH", TOSATANH);
        tmp.emplace("ADD", TOSAADD);
        tmp.emplace("ARITHMETIC_RIGHT_SHIFT", TOSAARITHMETIC_RIGHT_SHIFT);
        tmp.emplace("BITWISE_AND", TOSABITWISE_AND);
        tmp.emplace("BITWISE_OR", TOSABITWISE_OR);
        tmp.emplace("BITWISE_XOR", TOSABITWISE_XOR);
        tmp.emplace("INTDIV", TOSAINTDIV);
        tmp.emplace("LOGICAL_AND", TOSALOGICAL_AND);
        tmp.emplace("LOGICAL_LEFT_SHIFT", TOSALOGICAL_LEFT_SHIFT);
        tmp.emplace("LOGICAL_RIGHT_SHIFT", TOSALOGICAL_RIGHT_SHIFT);
        tmp.emplace("LOGICAL_OR", TOSALOGICAL_OR);
        tmp.emplace("LOGICAL_XOR", TOSALOGICAL_XOR);
        tmp.emplace("MAXIMUM", TOSAMAXIMUM);
        tmp.emplace("MINIMUM", TOSAMINIMUM);
        tmp.emplace("MUL", TOSAMUL);
        tmp.emplace("POW", TOSAPOW);
        tmp.emplace("SUB", TOSASUB);
        tmp.emplace("TABLE", TOSATABLE);
        tmp.emplace("ABS", TOSAABS);
        tmp.emplace("BITWISE_NOT", TOSABITWISE_NOT);
        tmp.emplace("CEIL", TOSACEIL);
        tmp.emplace("CLZ", TOSACLZ);
        tmp.emplace("EXP", TOSAEXP);
        tmp.emplace("FLOOR", TOSAFLOOR);
        tmp.emplace("LOG", TOSALOG);
        tmp.emplace("LOGICAL_NOT", TOSALOGICAL_NOT);
        tmp.emplace("NEGATE", TOSANEGATE);
        tmp.emplace("RECIPROCAL", TOSARECIPROCAL);
        tmp.emplace("RSQRT", TOSARSQRT);
        tmp.emplace("SELECT", TOSASELECT);
        tmp.emplace("EQUAL", TOSAEQUAL);
        tmp.emplace("GREATER", TOSAGREATER);
        tmp.emplace("GREATER_EQUAL", TOSAGREATER_EQUAL);
        tmp.emplace("REDUCE_ALL", TOSAREDUCE_ALL);
        tmp.emplace("REDUCE_ANY", TOSAREDUCE_ANY);
        tmp.emplace("REDUCE_MAX", TOSAREDUCE_MAX);
        tmp.emplace("REDUCE_MIN", TOSAREDUCE_MIN);
        tmp.emplace("REDUCE_PRODUCT", TOSAREDUCE_PRODUCT);
        tmp.emplace("REDUCE_SUM", TOSAREDUCE_SUM);
        tmp.emplace("CONCAT", TOSACONCAT);
        tmp.emplace("PAD", TOSAPAD);
        tmp.emplace("DIM", TOSADIM);
        tmp.emplace("RESHAPE", TOSARESHAPE);
        tmp.emplace("REVERSE", TOSAREVERSE);
        tmp.emplace("SLICE", TOSASLICE);
        tmp.emplace("TILE", TOSATILE);
        tmp.emplace("TRANSPOSE", TOSATRANSPOSE);
        tmp.emplace("GATHER", TOSAGATHER);
        tmp.emplace("SCATTER", TOSASCATTER);
        tmp.emplace("RESIZE", TOSARESIZE);
        tmp.emplace("CAST", TOSACAST);
        tmp.emplace("RESCALE", TOSARESCALE);
        return tmp;
    }();
    return mapInstance;
}

TOSAInstructions GetTosaOpEnum(const std::string& instruction)
{
    const auto& tosaMap = getTosaInstructionsMap();
    if (const auto it = tosaMap.find(instruction); it != tosaMap.end())
    {
        return it->second;
    }
    throw std::invalid_argument("Unsupported instruction: " + instruction);
}

static const std::unordered_map<std::string, spv::Capability>& getCapabilityMap()
{
    static const auto mapInstance = []() {
        std::unordered_map<std::string, spv::Capability> tmp;
        tmp.reserve(10);
        tmp.emplace("VulkanMemoryModel", spv::Capability::CapabilityVulkanMemoryModel);
        tmp.emplace("Shader", spv::Capability::CapabilityShader);
        tmp.emplace("Int8", spv::Capability::CapabilityInt8);
        tmp.emplace("Int16", spv::Capability::CapabilityInt16);
        tmp.emplace("Float16", spv::Capability::CapabilityFloat16);
        tmp.emplace("Int64", spv::Capability::CapabilityInt64);
        tmp.emplace("GraphARM", spv::Capability::CapabilityGraphARM);
        tmp.emplace("TensorsARM", spv::Capability::CapabilityTensorsARM);
        tmp.emplace("Matrix", spv::Capability::CapabilityMatrix);
        tmp.emplace("ReplicatedCompositesEXT", spv::Capability::CapabilityReplicatedCompositesEXT);
        return tmp;
    }();
    return mapInstance;
}

spv::Capability GetCapabilityEnum(const std::string& instruction)
{
    const auto& capabilityMap = getCapabilityMap();
    if (const auto it = capabilityMap.find(instruction); it != capabilityMap.end())
    {
        return it->second;
    }
    throw std::invalid_argument("Unsupported capability: " + instruction);
}

unsigned int GetResultIdPosition(const spv::Op op)
{
    switch (op)
    {
        case spv::Op::OpTypeInt:
        case spv::Op::OpTypeFloat:
        case spv::Op::OpTypeBool:
        case spv::Op::OpTypeArray:
        case spv::Op::OpTypeTensorARM:
        case spv::Op::OpTypePointer:
        case spv::Op::OpTypeGraphARM:
        case spv::Op::OpExtInstImport: return 0;
        case spv::Op::OpConstant:
        case spv::Op::OpConstantTrue:
        case spv::Op::OpConstantFalse:
        case spv::Op::OpConstantNull:
        case spv::Op::OpConstantComposite:
        case spv::Op::OpVariable:
        case spv::Op::OpGraphInputARM:
        case spv::Op::OpExtInst:
        case spv::Op::OpGraphARM:
        case spv::Op::OpConstantCompositeReplicateEXT:
        case spv::Op::OpGraphConstantARM: return 1;

        // Instructions that do not produce a result id.
        case spv::Op::OpMemoryModel:
        case spv::Op::OpDecorate:
        case spv::Op::OpGraphEntryPointARM:
        case spv::Op::OpGraphSetOutputARM:
        case spv::Op::OpGraphEndARM:
        default:
        {
            std::ostringstream oss;
            oss << "Opcode " << static_cast<unsigned int>(op) << " does not produce a result id.";
            throw std::runtime_error(oss.str());
        }
    }
}

unsigned int GetResId(const tosa2spirv::spirv::Instruction& instruction)
{
    for (const auto& operand : instruction.m_Operands)
    {
        if (operand.m_Type == tosa2spirv::spirv::RES_ID)
        {
            return operand.m_LiteralWord;
        }
    }
    return 0;
}

std::string InstructionToString(const tosa2spirv::spirv::Instruction& instruction)
{
    std::string instructionString;

    if (const auto resId = GetResId(instruction); resId != 0)
    {
        instructionString += "%" + std::to_string(resId) + " = ";
    }

    instructionString += GetOpString(instruction.m_Opcode);
    for (const auto& operand : instruction.m_Operands)
    {
        switch (operand.m_Type)
        {
            case tosa2spirv::spirv::INSTRUCTION_POINTER:
                instructionString += " %" + std::to_string(GetResId(*operand.m_InstructionPtr));
                break;
            case tosa2spirv::spirv::RES_ID: break;
            case tosa2spirv::spirv::LITERAL_WORD:
                instructionString += " " + std::to_string(operand.m_LiteralWord);
                break;
            case tosa2spirv::spirv::LITERAL_STRING: instructionString += " " + *operand.m_LiteralStr; break;
            default: throw std::runtime_error("Invalid operand type");
        }
    }
    return instructionString;
}
