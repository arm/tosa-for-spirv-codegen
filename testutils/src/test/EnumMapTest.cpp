//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <EnumMaps.hpp>
#include <Graph.hpp>
#include <Instruction.hpp>
#include <OperatorEnum.hpp>
#include <StringUtils.hpp>

#include <tosa2spirv.hpp>

#include <gtest/gtest.h>
TEST(SpvUtilsTest, OpEnumAndStringRoundTrip)
{
    std::vector<std::pair<std::string, spv::Op>> opMap = {
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
        {"OpName", spv::Op::OpName},
    };

    for (const auto &[name, op] : opMap)
    {
        EXPECT_EQ(GetOpEnum(name), op);
        EXPECT_EQ(OpToString(op), name);
    }

    EXPECT_THROW(GetOpEnum("InvalidOp"), std::invalid_argument);
    EXPECT_THROW(OpToString(static_cast<spv::Op>(9999)), std::invalid_argument);
}

TEST(SpvUtilsTest, CapabilityEnumValidation)
{
    std::map<std::string, spv::Capability> capabilities = {
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

    for (const auto &[name, cap] : capabilities)
    {
        EXPECT_EQ(GetCapabilityEnum(name), cap);
    }

    EXPECT_THROW(GetCapabilityEnum("UnknownCapability"), std::invalid_argument);
}

TEST(SpvUtilsTest, TosaOpEnumMapping)
{
    EXPECT_EQ(GetTosaOpEnum("CONV2D"), TOSACONV2D);
    EXPECT_EQ(GetTosaOpEnum("ADD"), TOSAADD);
    EXPECT_THROW(GetTosaOpEnum("INVALID_OP"), std::invalid_argument);
}

TEST(SpvUtilsTest, OperatorEnumMapping)
{
    EXPECT_EQ(GetOperatorEnum(TOSAARGMAX), tosa2spirv::tosa::OperatorEnum::ArgMax);
    EXPECT_EQ(GetOperatorEnum(TOSAAVG_POOL2D), tosa2spirv::tosa::OperatorEnum::AvgPool2d);
    EXPECT_EQ(GetOperatorEnum(TOSACONV2D), tosa2spirv::tosa::OperatorEnum::Conv2d);
    EXPECT_EQ(GetOperatorEnum(TOSACONV3D), tosa2spirv::tosa::OperatorEnum::Conv3d);
    EXPECT_EQ(GetOperatorEnum(TOSADEPTHWISE_CONV2D), tosa2spirv::tosa::OperatorEnum::DepthwiseConv2d);
    EXPECT_EQ(GetOperatorEnum(TOSAFFT2D), tosa2spirv::tosa::OperatorEnum::Fft2d);
    EXPECT_EQ(GetOperatorEnum(TOSAMATMUL), tosa2spirv::tosa::OperatorEnum::Matmul);
    EXPECT_EQ(GetOperatorEnum(TOSAMAX_POOL2D), tosa2spirv::tosa::OperatorEnum::MaxPool2d);
    EXPECT_EQ(GetOperatorEnum(TOSARFFT2D), tosa2spirv::tosa::OperatorEnum::Rfft2d);
    EXPECT_EQ(GetOperatorEnum(TOSATRANSPOSE_CONV2D), tosa2spirv::tosa::OperatorEnum::TransposeConv2d);
    EXPECT_EQ(GetOperatorEnum(TOSACLAMP), tosa2spirv::tosa::OperatorEnum::Clamp);
    EXPECT_EQ(GetOperatorEnum(TOSAERF), tosa2spirv::tosa::OperatorEnum::Erf);
    EXPECT_EQ(GetOperatorEnum(TOSASIGMOID), tosa2spirv::tosa::OperatorEnum::Sigmoid);
    EXPECT_EQ(GetOperatorEnum(TOSATANH), tosa2spirv::tosa::OperatorEnum::Tanh);
    EXPECT_EQ(GetOperatorEnum(TOSAADD), tosa2spirv::tosa::OperatorEnum::Add);
    EXPECT_EQ(GetOperatorEnum(TOSAARITHMETIC_RIGHT_SHIFT), tosa2spirv::tosa::OperatorEnum::ArithmeticRightShift);
    EXPECT_EQ(GetOperatorEnum(TOSABITWISE_AND), tosa2spirv::tosa::OperatorEnum::BitwiseAnd);
    EXPECT_EQ(GetOperatorEnum(TOSABITWISE_OR), tosa2spirv::tosa::OperatorEnum::BitwiseOr);
    EXPECT_EQ(GetOperatorEnum(TOSABITWISE_XOR), tosa2spirv::tosa::OperatorEnum::BitwiseXor);
    EXPECT_EQ(GetOperatorEnum(TOSAINTDIV), tosa2spirv::tosa::OperatorEnum::IntDiv);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_AND), tosa2spirv::tosa::OperatorEnum::LogicalAnd);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_LEFT_SHIFT), tosa2spirv::tosa::OperatorEnum::LogicalLeftShift);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_RIGHT_SHIFT), tosa2spirv::tosa::OperatorEnum::LogicalRightShift);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_OR), tosa2spirv::tosa::OperatorEnum::LogicalOr);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_XOR), tosa2spirv::tosa::OperatorEnum::LogicalXor);
    EXPECT_EQ(GetOperatorEnum(TOSAMAXIMUM), tosa2spirv::tosa::OperatorEnum::Maximum);
    EXPECT_EQ(GetOperatorEnum(TOSAMINIMUM), tosa2spirv::tosa::OperatorEnum::Minimum);
    EXPECT_EQ(GetOperatorEnum(TOSAMUL), tosa2spirv::tosa::OperatorEnum::Mul);
    EXPECT_EQ(GetOperatorEnum(TOSAPOW), tosa2spirv::tosa::OperatorEnum::Pow);
    EXPECT_EQ(GetOperatorEnum(TOSASUB), tosa2spirv::tosa::OperatorEnum::Sub);
    EXPECT_EQ(GetOperatorEnum(TOSATABLE), tosa2spirv::tosa::OperatorEnum::Table);
    EXPECT_EQ(GetOperatorEnum(TOSAABS), tosa2spirv::tosa::OperatorEnum::Abs);
    EXPECT_EQ(GetOperatorEnum(TOSABITWISE_NOT), tosa2spirv::tosa::OperatorEnum::BitwiseNot);
    EXPECT_EQ(GetOperatorEnum(TOSACEIL), tosa2spirv::tosa::OperatorEnum::Ceil);
    EXPECT_EQ(GetOperatorEnum(TOSACLZ), tosa2spirv::tosa::OperatorEnum::Clz);
    EXPECT_EQ(GetOperatorEnum(TOSACOS), tosa2spirv::tosa::OperatorEnum::Cos);
    EXPECT_EQ(GetOperatorEnum(TOSAEXP), tosa2spirv::tosa::OperatorEnum::Exp);
    EXPECT_EQ(GetOperatorEnum(TOSAFLOOR), tosa2spirv::tosa::OperatorEnum::Floor);
    EXPECT_EQ(GetOperatorEnum(TOSALOG), tosa2spirv::tosa::OperatorEnum::Log);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_NOT), tosa2spirv::tosa::OperatorEnum::LogicalNot);
    EXPECT_EQ(GetOperatorEnum(TOSANEGATE), tosa2spirv::tosa::OperatorEnum::Negate);
    EXPECT_EQ(GetOperatorEnum(TOSARECIPROCAL), tosa2spirv::tosa::OperatorEnum::Reciprocal);
    EXPECT_EQ(GetOperatorEnum(TOSARSQRT), tosa2spirv::tosa::OperatorEnum::Rsqrt);
    EXPECT_EQ(GetOperatorEnum(TOSASIN), tosa2spirv::tosa::OperatorEnum::Sin);
    EXPECT_EQ(GetOperatorEnum(TOSASELECT), tosa2spirv::tosa::OperatorEnum::Select);
    EXPECT_EQ(GetOperatorEnum(TOSAEQUAL), tosa2spirv::tosa::OperatorEnum::Equal);
    EXPECT_EQ(GetOperatorEnum(TOSAGREATER), tosa2spirv::tosa::OperatorEnum::Greater);
    EXPECT_EQ(GetOperatorEnum(TOSAGREATER_EQUAL), tosa2spirv::tosa::OperatorEnum::GreaterEqual);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_ALL), tosa2spirv::tosa::OperatorEnum::ReduceAll);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_ANY), tosa2spirv::tosa::OperatorEnum::ReduceAny);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_MAX), tosa2spirv::tosa::OperatorEnum::ReduceMax);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_MIN), tosa2spirv::tosa::OperatorEnum::ReduceMin);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_PRODUCT), tosa2spirv::tosa::OperatorEnum::ReduceProduct);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_SUM), tosa2spirv::tosa::OperatorEnum::ReduceSum);
    EXPECT_EQ(GetOperatorEnum(TOSACONCAT), tosa2spirv::tosa::OperatorEnum::Concat);
    EXPECT_EQ(GetOperatorEnum(TOSAPAD), tosa2spirv::tosa::OperatorEnum::Pad);
    EXPECT_EQ(GetOperatorEnum(TOSARESHAPE), tosa2spirv::tosa::OperatorEnum::Reshape);
    EXPECT_EQ(GetOperatorEnum(TOSAREVERSE), tosa2spirv::tosa::OperatorEnum::Reverse);
    EXPECT_EQ(GetOperatorEnum(TOSASLICE), tosa2spirv::tosa::OperatorEnum::Slice);
    EXPECT_EQ(GetOperatorEnum(TOSATILE), tosa2spirv::tosa::OperatorEnum::Tile);
    EXPECT_EQ(GetOperatorEnum(TOSATRANSPOSE), tosa2spirv::tosa::OperatorEnum::Transpose);
    EXPECT_EQ(GetOperatorEnum(TOSAGATHER), tosa2spirv::tosa::OperatorEnum::Gather);
    EXPECT_EQ(GetOperatorEnum(TOSASCATTER), tosa2spirv::tosa::OperatorEnum::Scatter);
    EXPECT_EQ(GetOperatorEnum(TOSARESIZE), tosa2spirv::tosa::OperatorEnum::Resize);
    EXPECT_EQ(GetOperatorEnum(TOSACAST), tosa2spirv::tosa::OperatorEnum::Cast);
    EXPECT_EQ(GetOperatorEnum(TOSARESCALE), tosa2spirv::tosa::OperatorEnum::Rescale);
    EXPECT_THROW(GetOperatorEnum(TOSAInstructionsMax), std::invalid_argument);
}

TEST(SpvUtilsTest, GetResultIdPositionCases)
{
    EXPECT_EQ(GetResultIdPosition(spv::Op::OpTypeInt), 0);
    EXPECT_EQ(GetResultIdPosition(spv::Op::OpConstant), 1);

    // Use a known no-result-id case
    EXPECT_THROW(GetResultIdPosition(spv::Op::OpMemoryModel), std::runtime_error);
    EXPECT_THROW(GetResultIdPosition(static_cast<spv::Op>(9999)), std::runtime_error);
}

TEST(SpvUtilsTest, InstructionToStringBasic)
{
    using namespace tosa2spirv::spirv;
    Instruction instr;
    instr.m_Opcode = spv::Op::OpConstant;
    instr.m_Operands.emplace_back(42);

    instr.m_Operands.emplace_back("some_str");
    instr.m_Operands.emplace_back(123);

    std::string str = testutils::InstructionToString(instr);
    EXPECT_NE(str.find("OpConstant"), std::string::npos);
    EXPECT_NE(str.find("42"), std::string::npos);
    EXPECT_NE(str.find("some_str"), std::string::npos);
    EXPECT_NE(str.find("123"), std::string::npos);
}

TEST(SpvUtilsTest, GetResId)
{
    using namespace tosa2spirv::spirv;
    Instruction instr;
    instr.m_Operands.emplace_back(123, RES_ID);
    EXPECT_EQ(GetResId(instr), 123);
}
