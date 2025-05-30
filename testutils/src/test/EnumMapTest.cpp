//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include "../EnumMaps.hpp"

#include <Graph.hpp>
#include <Instruction.hpp>
#include <gtest/gtest.h>
#include <tosa2spirv.hpp>

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
        EXPECT_EQ(GetOpString(op), name);
    }

    EXPECT_THROW(GetOpEnum("InvalidOp"), std::invalid_argument);
    EXPECT_THROW(GetOpString(static_cast<spv::Op>(9999)), std::invalid_argument);
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

    std::string str = InstructionToString(instr);
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
