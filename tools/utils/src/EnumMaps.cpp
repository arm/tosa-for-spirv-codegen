//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "EnumMaps.hpp"

#include <Instruction.hpp>
#include <Operand.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

spv::Op GetOpEnum(const std::string& instruction)
{
    const std::unordered_map<std::string, spv::Op> instructionMap = {
        {"OpCapability", spv::Op::OpCapability},
        {"OpExtension", spv::Op::OpExtension},
        {"OpMemoryModel", spv::Op::OpMemoryModel},
        {"OpTypeInt", spv::Op::OpTypeInt},
        {"OpTypeFloat", spv::Op::OpTypeFloat},
        {"OpTypeBool", spv::Op::OpTypeBool},
        {"OpConstantTrue", spv::Op::OpConstantTrue},
        {"OpConstantFalse", spv::Op::OpConstantFalse},
        {"OpConstant", spv::Op::OpConstant},
        {"OpConstantComposite", spv::Op::OpConstantComposite},
        {"OpConstantNull", spv::Op::OpConstantNull},
        {"OpConstantCompositeReplicateEXT", spv::Op::OpConstantCompositeReplicateEXT},
        {"OpTypeArray", spv::Op::OpTypeArray},
        {"OpTypeTensorARM", spv::Op::OpTypeTensorARM},
        {"OpTypeStruct", spv::Op::OpTypeStruct},
        {"OpExtInst", spv::Op::OpExtInst},
        {"OpCompositeExtract", spv::Op::OpCompositeExtract},
        {"OpTypeGraphARM", spv::Op::OpTypeGraphARM},
        {"OpExtInstImport", spv::Op::OpExtInstImport},
        {"OpGraphInputARM", spv::Op::OpGraphInputARM},
        {"OpGraphSetOutputARM", spv::Op::OpGraphSetOutputARM},
        {"OpGraphARM", spv::Op::OpGraphARM},
        {"OpTypePointer", spv::Op::OpTypePointer},
        {"OpVariable", spv::Op::OpVariable},
        {"OpDecorate", spv::Op::OpDecorate},
        {"OpGraphEntryPointARM", spv::Op::OpGraphEntryPointARM},
        {"OpGraphEndARM", spv::Op::OpGraphEndARM},
        {"OpGraphConstantARM", spv::Op::OpGraphConstantARM},
        {"OpName", spv::Op::OpName}};

    // Look up the instruction in the map.
    if (const auto it = instructionMap.find(instruction); it != instructionMap.end())
    {
        return it->second;
    }
    // If not found, throw an exception.
    throw std::invalid_argument("Unsupported instruction: " + instruction);
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
        tmp.emplace("COS", TOSACOS);
        tmp.emplace("EXP", TOSAEXP);
        tmp.emplace("FLOOR", TOSAFLOOR);
        tmp.emplace("LOG", TOSALOG);
        tmp.emplace("LOGICAL_NOT", TOSALOGICAL_NOT);
        tmp.emplace("NEGATE", TOSANEGATE);
        tmp.emplace("RECIPROCAL", TOSARECIPROCAL);
        tmp.emplace("RSQRT", TOSARSQRT);
        tmp.emplace("SIN", TOSASIN);
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
    const std::unordered_map<std::string, TOSAInstructions> tosaInstructionsMap = {
        {"ARGMAX", TOSAARGMAX},
        {"AVG_POOL2D", TOSAAVG_POOL2D},
        {"CONV2D", TOSACONV2D},
        {"CONV3D", TOSACONV3D},
        {"DEPTHWISE_CONV2D", TOSADEPTHWISE_CONV2D},
        {"FFT2D", TOSAFFT2D},
        {"MATMUL", TOSAMATMUL},
        {"MAX_POOL2D", TOSAMAX_POOL2D},
        {"RFFT2D", TOSARFFT2D},
        {"TRANSPOSE_CONV2D", TOSATRANSPOSE_CONV2D},
        {"CLAMP", TOSACLAMP},
        {"ERF", TOSAERF},
        {"SIGMOID", TOSASIGMOID},
        {"TANH", TOSATANH},
        {"ADD", TOSAADD},
        {"ARITHMETIC_RIGHT_SHIFT", TOSAARITHMETIC_RIGHT_SHIFT},
        {"BITWISE_AND", TOSABITWISE_AND},
        {"BITWISE_OR", TOSABITWISE_OR},
        {"BITWISE_XOR", TOSABITWISE_XOR},
        {"INTDIV", TOSAINTDIV},
        {"LOGICAL_AND", TOSALOGICAL_AND},
        {"LOGICAL_LEFT_SHIFT", TOSALOGICAL_LEFT_SHIFT},
        {"LOGICAL_RIGHT_SHIFT", TOSALOGICAL_RIGHT_SHIFT},
        {"LOGICAL_OR", TOSALOGICAL_OR},
        {"LOGICAL_XOR", TOSALOGICAL_XOR},
        {"MAXIMUM", TOSAMAXIMUM},
        {"MINIMUM", TOSAMINIMUM},
        {"MUL", TOSAMUL},
        {"POW", TOSAPOW},
        {"SUB", TOSASUB},
        {"TABLE", TOSATABLE},
        {"ABS", TOSAABS},
        {"BITWISE_NOT", TOSABITWISE_NOT},
        {"CEIL", TOSACEIL},
        {"CLZ", TOSACLZ},
        {"COS", TOSACOS},
        {"EXP", TOSAEXP},
        {"FLOOR", TOSAFLOOR},
        {"LOG", TOSALOG},
        {"LOGICAL_NOT", TOSALOGICAL_NOT},
        {"NEGATE", TOSANEGATE},
        {"RECIPROCAL", TOSARECIPROCAL},
        {"RSQRT", TOSARSQRT},
        {"SIN", TOSASIN},
        {"SELECT", TOSASELECT},
        {"EQUAL", TOSAEQUAL},
        {"GREATER", TOSAGREATER},
        {"GREATER_EQUAL", TOSAGREATER_EQUAL},
        {"REDUCE_ALL", TOSAREDUCE_ALL},
        {"REDUCE_ANY", TOSAREDUCE_ANY},
        {"REDUCE_MAX", TOSAREDUCE_MAX},
        {"REDUCE_MIN", TOSAREDUCE_MIN},
        {"REDUCE_PRODUCT", TOSAREDUCE_PRODUCT},
        {"REDUCE_SUM", TOSAREDUCE_SUM},
        {"CONCAT", TOSACONCAT},
        {"PAD", TOSAPAD},
        {"RESHAPE", TOSARESHAPE},
        {"REVERSE", TOSAREVERSE},
        {"SLICE", TOSASLICE},
        {"TILE", TOSATILE},
        {"TRANSPOSE", TOSATRANSPOSE},
        {"GATHER", TOSAGATHER},
        {"SCATTER", TOSASCATTER},
        {"RESIZE", TOSARESIZE},
        {"CAST", TOSACAST},
        {"RESCALE", TOSARESCALE}};
    if (const auto it = tosaInstructionsMap.find(instruction); it != tosaInstructionsMap.end())
    {
        return it->second;
    }
    throw std::invalid_argument("Unsupported instruction: " + instruction);
}

tfsc::tosa::OperatorEnum GetOperatorEnum(TOSAInstructions instructionType)
{
    // OPERATOR ENUM MAP SECTION BEGIN
    switch (instructionType)
    {
        case TOSAARGMAX: return tfsc::tosa::OperatorEnum::ArgMax;
        case TOSAAVG_POOL2D: return tfsc::tosa::OperatorEnum::AvgPool2d;
        case TOSACONV2D: return tfsc::tosa::OperatorEnum::Conv2d;
        case TOSACONV3D: return tfsc::tosa::OperatorEnum::Conv3d;
        case TOSADEPTHWISE_CONV2D: return tfsc::tosa::OperatorEnum::DepthwiseConv2d;
        case TOSAFFT2D: return tfsc::tosa::OperatorEnum::Fft2d;
        case TOSAMATMUL: return tfsc::tosa::OperatorEnum::Matmul;
        case TOSAMAX_POOL2D: return tfsc::tosa::OperatorEnum::MaxPool2d;
        case TOSARFFT2D: return tfsc::tosa::OperatorEnum::Rfft2d;
        case TOSATRANSPOSE_CONV2D: return tfsc::tosa::OperatorEnum::TransposeConv2d;
        case TOSACLAMP: return tfsc::tosa::OperatorEnum::Clamp;
        case TOSAERF: return tfsc::tosa::OperatorEnum::Erf;
        case TOSASIGMOID: return tfsc::tosa::OperatorEnum::Sigmoid;
        case TOSATANH: return tfsc::tosa::OperatorEnum::Tanh;
        case TOSAADD: return tfsc::tosa::OperatorEnum::Add;
        case TOSAARITHMETIC_RIGHT_SHIFT: return tfsc::tosa::OperatorEnum::ArithmeticRightShift;
        case TOSABITWISE_AND: return tfsc::tosa::OperatorEnum::BitwiseAnd;
        case TOSABITWISE_OR: return tfsc::tosa::OperatorEnum::BitwiseOr;
        case TOSABITWISE_XOR: return tfsc::tosa::OperatorEnum::BitwiseXor;
        case TOSAINTDIV: return tfsc::tosa::OperatorEnum::IntDiv;
        case TOSALOGICAL_AND: return tfsc::tosa::OperatorEnum::LogicalAnd;
        case TOSALOGICAL_LEFT_SHIFT: return tfsc::tosa::OperatorEnum::LogicalLeftShift;
        case TOSALOGICAL_RIGHT_SHIFT: return tfsc::tosa::OperatorEnum::LogicalRightShift;
        case TOSALOGICAL_OR: return tfsc::tosa::OperatorEnum::LogicalOr;
        case TOSALOGICAL_XOR: return tfsc::tosa::OperatorEnum::LogicalXor;
        case TOSAMAXIMUM: return tfsc::tosa::OperatorEnum::Maximum;
        case TOSAMINIMUM: return tfsc::tosa::OperatorEnum::Minimum;
        case TOSAMUL: return tfsc::tosa::OperatorEnum::Mul;
        case TOSAPOW: return tfsc::tosa::OperatorEnum::Pow;
        case TOSASUB: return tfsc::tosa::OperatorEnum::Sub;
        case TOSATABLE: return tfsc::tosa::OperatorEnum::Table;
        case TOSAABS: return tfsc::tosa::OperatorEnum::Abs;
        case TOSABITWISE_NOT: return tfsc::tosa::OperatorEnum::BitwiseNot;
        case TOSACEIL: return tfsc::tosa::OperatorEnum::Ceil;
        case TOSACLZ: return tfsc::tosa::OperatorEnum::Clz;
        case TOSACOS: return tfsc::tosa::OperatorEnum::Cos;
        case TOSAEXP: return tfsc::tosa::OperatorEnum::Exp;
        case TOSAFLOOR: return tfsc::tosa::OperatorEnum::Floor;
        case TOSALOG: return tfsc::tosa::OperatorEnum::Log;
        case TOSALOGICAL_NOT: return tfsc::tosa::OperatorEnum::LogicalNot;
        case TOSANEGATE: return tfsc::tosa::OperatorEnum::Negate;
        case TOSARECIPROCAL: return tfsc::tosa::OperatorEnum::Reciprocal;
        case TOSARSQRT: return tfsc::tosa::OperatorEnum::Rsqrt;
        case TOSASIN: return tfsc::tosa::OperatorEnum::Sin;
        case TOSASELECT: return tfsc::tosa::OperatorEnum::Select;
        case TOSAEQUAL: return tfsc::tosa::OperatorEnum::Equal;
        case TOSAGREATER: return tfsc::tosa::OperatorEnum::Greater;
        case TOSAGREATER_EQUAL: return tfsc::tosa::OperatorEnum::GreaterEqual;
        case TOSAREDUCE_ALL: return tfsc::tosa::OperatorEnum::ReduceAll;
        case TOSAREDUCE_ANY: return tfsc::tosa::OperatorEnum::ReduceAny;
        case TOSAREDUCE_MAX: return tfsc::tosa::OperatorEnum::ReduceMax;
        case TOSAREDUCE_MIN: return tfsc::tosa::OperatorEnum::ReduceMin;
        case TOSAREDUCE_PRODUCT: return tfsc::tosa::OperatorEnum::ReduceProduct;
        case TOSAREDUCE_SUM: return tfsc::tosa::OperatorEnum::ReduceSum;
        case TOSACONCAT: return tfsc::tosa::OperatorEnum::Concat;
        case TOSAPAD: return tfsc::tosa::OperatorEnum::Pad;
        case TOSARESHAPE: return tfsc::tosa::OperatorEnum::Reshape;
        case TOSAREVERSE: return tfsc::tosa::OperatorEnum::Reverse;
        case TOSASLICE: return tfsc::tosa::OperatorEnum::Slice;
        case TOSATILE: return tfsc::tosa::OperatorEnum::Tile;
        case TOSATRANSPOSE: return tfsc::tosa::OperatorEnum::Transpose;
        case TOSAGATHER: return tfsc::tosa::OperatorEnum::Gather;
        case TOSASCATTER: return tfsc::tosa::OperatorEnum::Scatter;
        case TOSARESIZE: return tfsc::tosa::OperatorEnum::Resize;
        case TOSACAST: return tfsc::tosa::OperatorEnum::Cast;
        case TOSARESCALE: return tfsc::tosa::OperatorEnum::Rescale;
        // OPERATOR ENUM MAP SECTION END
        default: throw std::invalid_argument("Invalid TOSAInstruction");
    }
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
    const std::unordered_map<std::string, spv::Capability> capabilityMap = {
        {"VulkanMemoryModel", spv::Capability::CapabilityVulkanMemoryModel},
        {"Shader", spv::Capability::CapabilityShader},
        {"Int8", spv::Capability::CapabilityInt8},
        {"Int16", spv::Capability::CapabilityInt16},
        {"Float16", spv::Capability::CapabilityFloat16},
        {"Int64", spv::Capability::CapabilityInt64},
        {"GraphARM", spv::Capability::CapabilityGraphARM},
        {"TensorsARM", spv::Capability::CapabilityTensorsARM},
        {"Matrix", spv::Capability::CapabilityMatrix},
        {"ReplicatedCompositesEXT", spv::Capability::CapabilityReplicatedCompositesEXT}};

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
        case spv::Op::OpTypeStruct:
        case spv::Op::OpTypeTensorARM:
        case spv::Op::OpTypePointer:
        case spv::Op::OpTypeGraphARM:
        case spv::Op::OpExtInstImport: return 0;
        case spv::Op::OpConstant:
        case spv::Op::OpConstantTrue:
        case spv::Op::OpConstantFalse:
        case spv::Op::OpConstantNull:
        case spv::Op::OpConstantComposite:
        case spv::Op::OpCompositeExtract:
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

std::string OpToString(const spv::Op op)
{
    const std::unordered_map<spv::Op, std::string> opToString = {
        {spv::Op::OpCapability, "OpCapability"},
        {spv::Op::OpExtension, "OpExtension"},
        {spv::Op::OpMemoryModel, "OpMemoryModel"},
        {spv::Op::OpTypeInt, "OpTypeInt"},
        {spv::Op::OpTypeFloat, "OpTypeFloat"},
        {spv::Op::OpTypeBool, "OpTypeBool"},
        {spv::Op::OpConstantTrue, "OpConstantTrue"},
        {spv::Op::OpConstantFalse, "OpConstantFalse"},
        {spv::Op::OpConstant, "OpConstant"},
        {spv::Op::OpConstantComposite, "OpConstantComposite"},
        {spv::Op::OpConstantNull, "OpConstantNull"},
        {spv::Op::OpConstantCompositeReplicateEXT, "OpConstantCompositeReplicateEXT"},
        {spv::Op::OpTypeArray, "OpTypeArray"},
        {spv::Op::OpTypeTensorARM, "OpTypeTensorARM"},
        {spv::Op::OpTypeStruct, "OpTypeStruct"},
        {spv::Op::OpExtInst, "OpExtInst"},
        {spv::Op::OpCompositeExtract, "OpCompositeExtract"},
        {spv::Op::OpTypeGraphARM, "OpTypeGraphARM"},
        {spv::Op::OpExtInstImport, "OpExtInstImport"},
        {spv::Op::OpGraphInputARM, "OpGraphInputARM"},
        {spv::Op::OpGraphSetOutputARM, "OpGraphSetOutputARM"},
        {spv::Op::OpGraphARM, "OpGraphARM"},
        {spv::Op::OpTypePointer, "OpTypePointer"},
        {spv::Op::OpVariable, "OpVariable"},
        {spv::Op::OpDecorate, "OpDecorate"},
        {spv::Op::OpGraphEntryPointARM, "OpGraphEntryPointARM"},
        {spv::Op::OpGraphEndARM, "OpGraphEndARM"},
        {spv::Op::OpName, "OpName"},
        {spv::Op::OpGraphConstantARM, "OpGraphConstantARM"}};

    if (const auto it = opToString.find(op); it != opToString.end())
    {
        return it->second;
    }
    std::string er = "GetOpString::Unsupported op enum value: " + std::to_string(op);
    throw std::invalid_argument(er);
}

unsigned int GetResId(const tfsc::spirv::Instruction& instruction)
{
    for (const auto& operand : instruction.m_Operands)
    {
        if (operand.m_Type == tfsc::spirv::RES_ID)
        {
            return operand.m_LiteralWord;
        }
    }
    return 0;
}
