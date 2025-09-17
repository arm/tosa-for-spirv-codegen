//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <EnumMaps.hpp>
#include <Graph.hpp>
#include <Instruction.hpp>
#include <OperatorEnum.hpp>
#include <StringUtils.hpp>

#include <TosaForSpirvCodegen.hpp>

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
    EXPECT_EQ(GetOperatorEnum(TOSAARGMAX), tfsc::tosa::OperatorEnum::ArgMax);
    EXPECT_EQ(GetOperatorEnum(TOSAAVG_POOL2D), tfsc::tosa::OperatorEnum::AvgPool2d);
    EXPECT_EQ(GetOperatorEnum(TOSACONV2D), tfsc::tosa::OperatorEnum::Conv2d);
    EXPECT_EQ(GetOperatorEnum(TOSACONV3D), tfsc::tosa::OperatorEnum::Conv3d);
    EXPECT_EQ(GetOperatorEnum(TOSADEPTHWISE_CONV2D), tfsc::tosa::OperatorEnum::DepthwiseConv2d);
    EXPECT_EQ(GetOperatorEnum(TOSAFFT2D), tfsc::tosa::OperatorEnum::Fft2d);
    EXPECT_EQ(GetOperatorEnum(TOSAMATMUL), tfsc::tosa::OperatorEnum::Matmul);
    EXPECT_EQ(GetOperatorEnum(TOSAMAX_POOL2D), tfsc::tosa::OperatorEnum::MaxPool2d);
    EXPECT_EQ(GetOperatorEnum(TOSARFFT2D), tfsc::tosa::OperatorEnum::Rfft2d);
    EXPECT_EQ(GetOperatorEnum(TOSATRANSPOSE_CONV2D), tfsc::tosa::OperatorEnum::TransposeConv2d);
    EXPECT_EQ(GetOperatorEnum(TOSACLAMP), tfsc::tosa::OperatorEnum::Clamp);
    EXPECT_EQ(GetOperatorEnum(TOSAERF), tfsc::tosa::OperatorEnum::Erf);
    EXPECT_EQ(GetOperatorEnum(TOSASIGMOID), tfsc::tosa::OperatorEnum::Sigmoid);
    EXPECT_EQ(GetOperatorEnum(TOSATANH), tfsc::tosa::OperatorEnum::Tanh);
    EXPECT_EQ(GetOperatorEnum(TOSAADD), tfsc::tosa::OperatorEnum::Add);
    EXPECT_EQ(GetOperatorEnum(TOSAARITHMETIC_RIGHT_SHIFT), tfsc::tosa::OperatorEnum::ArithmeticRightShift);
    EXPECT_EQ(GetOperatorEnum(TOSABITWISE_AND), tfsc::tosa::OperatorEnum::BitwiseAnd);
    EXPECT_EQ(GetOperatorEnum(TOSABITWISE_OR), tfsc::tosa::OperatorEnum::BitwiseOr);
    EXPECT_EQ(GetOperatorEnum(TOSABITWISE_XOR), tfsc::tosa::OperatorEnum::BitwiseXor);
    EXPECT_EQ(GetOperatorEnum(TOSAINTDIV), tfsc::tosa::OperatorEnum::IntDiv);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_AND), tfsc::tosa::OperatorEnum::LogicalAnd);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_LEFT_SHIFT), tfsc::tosa::OperatorEnum::LogicalLeftShift);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_RIGHT_SHIFT), tfsc::tosa::OperatorEnum::LogicalRightShift);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_OR), tfsc::tosa::OperatorEnum::LogicalOr);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_XOR), tfsc::tosa::OperatorEnum::LogicalXor);
    EXPECT_EQ(GetOperatorEnum(TOSAMAXIMUM), tfsc::tosa::OperatorEnum::Maximum);
    EXPECT_EQ(GetOperatorEnum(TOSAMINIMUM), tfsc::tosa::OperatorEnum::Minimum);
    EXPECT_EQ(GetOperatorEnum(TOSAMUL), tfsc::tosa::OperatorEnum::Mul);
    EXPECT_EQ(GetOperatorEnum(TOSAPOW), tfsc::tosa::OperatorEnum::Pow);
    EXPECT_EQ(GetOperatorEnum(TOSASUB), tfsc::tosa::OperatorEnum::Sub);
    EXPECT_EQ(GetOperatorEnum(TOSATABLE), tfsc::tosa::OperatorEnum::Table);
    EXPECT_EQ(GetOperatorEnum(TOSAABS), tfsc::tosa::OperatorEnum::Abs);
    EXPECT_EQ(GetOperatorEnum(TOSABITWISE_NOT), tfsc::tosa::OperatorEnum::BitwiseNot);
    EXPECT_EQ(GetOperatorEnum(TOSACEIL), tfsc::tosa::OperatorEnum::Ceil);
    EXPECT_EQ(GetOperatorEnum(TOSACLZ), tfsc::tosa::OperatorEnum::Clz);
    EXPECT_EQ(GetOperatorEnum(TOSACOS), tfsc::tosa::OperatorEnum::Cos);
    EXPECT_EQ(GetOperatorEnum(TOSAEXP), tfsc::tosa::OperatorEnum::Exp);
    EXPECT_EQ(GetOperatorEnum(TOSAFLOOR), tfsc::tosa::OperatorEnum::Floor);
    EXPECT_EQ(GetOperatorEnum(TOSALOG), tfsc::tosa::OperatorEnum::Log);
    EXPECT_EQ(GetOperatorEnum(TOSALOGICAL_NOT), tfsc::tosa::OperatorEnum::LogicalNot);
    EXPECT_EQ(GetOperatorEnum(TOSANEGATE), tfsc::tosa::OperatorEnum::Negate);
    EXPECT_EQ(GetOperatorEnum(TOSARECIPROCAL), tfsc::tosa::OperatorEnum::Reciprocal);
    EXPECT_EQ(GetOperatorEnum(TOSARSQRT), tfsc::tosa::OperatorEnum::Rsqrt);
    EXPECT_EQ(GetOperatorEnum(TOSASIN), tfsc::tosa::OperatorEnum::Sin);
    EXPECT_EQ(GetOperatorEnum(TOSASELECT), tfsc::tosa::OperatorEnum::Select);
    EXPECT_EQ(GetOperatorEnum(TOSAEQUAL), tfsc::tosa::OperatorEnum::Equal);
    EXPECT_EQ(GetOperatorEnum(TOSAGREATER), tfsc::tosa::OperatorEnum::Greater);
    EXPECT_EQ(GetOperatorEnum(TOSAGREATER_EQUAL), tfsc::tosa::OperatorEnum::GreaterEqual);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_ALL), tfsc::tosa::OperatorEnum::ReduceAll);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_ANY), tfsc::tosa::OperatorEnum::ReduceAny);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_MAX), tfsc::tosa::OperatorEnum::ReduceMax);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_MIN), tfsc::tosa::OperatorEnum::ReduceMin);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_PRODUCT), tfsc::tosa::OperatorEnum::ReduceProduct);
    EXPECT_EQ(GetOperatorEnum(TOSAREDUCE_SUM), tfsc::tosa::OperatorEnum::ReduceSum);
    EXPECT_EQ(GetOperatorEnum(TOSACONCAT), tfsc::tosa::OperatorEnum::Concat);
    EXPECT_EQ(GetOperatorEnum(TOSAPAD), tfsc::tosa::OperatorEnum::Pad);
    EXPECT_EQ(GetOperatorEnum(TOSARESHAPE), tfsc::tosa::OperatorEnum::Reshape);
    EXPECT_EQ(GetOperatorEnum(TOSAREVERSE), tfsc::tosa::OperatorEnum::Reverse);
    EXPECT_EQ(GetOperatorEnum(TOSASLICE), tfsc::tosa::OperatorEnum::Slice);
    EXPECT_EQ(GetOperatorEnum(TOSATILE), tfsc::tosa::OperatorEnum::Tile);
    EXPECT_EQ(GetOperatorEnum(TOSATRANSPOSE), tfsc::tosa::OperatorEnum::Transpose);
    EXPECT_EQ(GetOperatorEnum(TOSAGATHER), tfsc::tosa::OperatorEnum::Gather);
    EXPECT_EQ(GetOperatorEnum(TOSASCATTER), tfsc::tosa::OperatorEnum::Scatter);
    EXPECT_EQ(GetOperatorEnum(TOSARESIZE), tfsc::tosa::OperatorEnum::Resize);
    EXPECT_EQ(GetOperatorEnum(TOSACAST), tfsc::tosa::OperatorEnum::Cast);
    EXPECT_EQ(GetOperatorEnum(TOSARESCALE), tfsc::tosa::OperatorEnum::Rescale);
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
    using namespace tfsc::spirv;
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
    using namespace tfsc::spirv;
    Instruction instr;
    instr.m_Operands.emplace_back(123, RES_ID);
    EXPECT_EQ(GetResId(instr), 123);
}
