//
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <TestUtils.hpp>

#include <tosa2spirv.hpp>
#include <Operand.hpp>
#include <ResId.hpp>
#include <Writer.hpp>

#include <gtest/gtest.h>
#include <spirv/unified1/spirv.hpp>
#include <spirv-tools/libspirv.hpp>

#include <limits>

using namespace tosa2spirv;
using namespace spirvwriter;
using namespace graphbuilder;

// Helper method to check module header in binary is correct.
static void CheckHeader(std::vector<uint32_t>& binary, uint32_t bound)
{
    // Magic Number
    EXPECT_TRUE(binary.at(0) == 0x07230203);
    // Version
    EXPECT_TRUE(binary.at(1) == 0x00010600);
    // Vendor
    EXPECT_TRUE(binary.at(2) == 0x00050000);
    // Bounds
    // This will be 0 until WriteBounds is called.
    EXPECT_TRUE(binary[3] == bound);
    // Schema
    EXPECT_TRUE(binary.at(4) == 0);
}

// Demonstrate some basic assertions.
TEST(TOSA2SPIRV, BasicAssertions)
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(TOSA2SPIRV, ModuleCreate)
{
    auto module = graphbuilder::Module::CreateRaw();
    auto module_unique = graphbuilder::Module::Create();
    EXPECT_TRUE(module);
    EXPECT_TRUE(module_unique);
}

TEST(TOSA2SPIRV, InitialiseSpirvTools)
{
    // Test that we can initialise the SpirvTools correctly.
    spvtools::SpirvTools tools{SPV_ENV_UNIVERSAL_1_6};
    GTEST_EXPECT_TRUE(tools.IsValid());
}

TEST(TOSA2SPIRV, ResId)
{
    // Test that we can create an ResId object and test its methods.
    ResId id1 = ResId(1);
    EXPECT_TRUE(id1.Get() == 1);
    EXPECT_TRUE(id1.IsValid());

    ResId id2 = ResId(2);
    EXPECT_TRUE(id1 < id2);

    ResId id3 = ResId(1);
    EXPECT_TRUE(id1 == id3);
}

TEST(TOSA2SPIRV, Operand)
{
    // Test that we can create an Operand object and test its methods.
    Operand operand1({spv::Capability::CapabilityVulkanMemoryModel});
    EXPECT_EQ(operand1.GetType(), OperandType::LITERAL_WORD);
    EXPECT_EQ(operand1.GetLiteralNum()[0], 5345);
    EXPECT_EQ(operand1.GetNumId(), 5345);

    std::string extGraph("SPV_ARM_graph");
    Operand operand2(LITERAL_STRING, extGraph);
    EXPECT_EQ(operand2.GetType(), OperandType::LITERAL_STRING);
    EXPECT_EQ(operand2.GetLiteralStr(), "SPV_ARM_graph");
    EXPECT_EQ(operand2.GetNumWords(), 4);

    ResId id(1);
    Operand operand3(NUMBER_ID, id.Get());
    EXPECT_EQ(operand3.GetType(), OperandType::NUMBER_ID);
    EXPECT_EQ(operand3.GetLiteralNum()[0], 1);
    EXPECT_EQ(operand3.GetNumId(), 1);
}

TEST(TOSA2SPIRV, Instruction)
{
    // Test that we can create an Instruction object and test its methods.
    Operand operand1({spv::Capability::CapabilityVulkanMemoryModel});
    Operand operand2({spv::Capability::CapabilityShader});
    std::vector<Operand> operands{operand1, operand2};

    Instruction instruction1(spv::Op::OpCapability, 1, operands);
    EXPECT_EQ(instruction1.GetOpcode(), 17);
    EXPECT_EQ(instruction1.GetResultId(), 1);
    EXPECT_EQ(instruction1.GetOperands().size(), 2);
    EXPECT_EQ(instruction1.GetWordCount(), 4);

    Instruction instruction2(spv::Op::OpConstant, 2);
    EXPECT_EQ(instruction2.GetOpcode(), 43);
    EXPECT_EQ(instruction2.GetResultId(), 2);
    EXPECT_EQ(instruction2.GetWordCount(), 2);
}

TEST(TOSA2SPIRV, SimpleWriter)
{
    // Test that we can create a writer and binary is added.
    IdGenerator gen;
    Writer writer(gen);

    Operand operand({spv::Capability::CapabilityVulkanMemoryModel});
    std::vector<Operand> capabilityOps{operand};

    Instruction capabilityInst(spv::Op::OpCapability, capabilityOps);
    InstructionList capabilityList{capabilityInst};

    EXPECT_TRUE(writer.GetBinary().empty());
    writer.WriteBinary(capabilityList);
    EXPECT_TRUE(writer.GetBinary().size() == 2);
}

TEST(TOSA2SPIRV, SimpleWriterCapabilities)
{
    // Test that we can create a writer and write simple operands instructions.
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    // Capabilities
    Operand operand1({spv::Capability::CapabilityVulkanMemoryModel});
    Operand operand2({spv::Capability::CapabilityShader});
    Operand operand3({spv::Capability::CapabilityInt8});
    Operand operand4({spv::Capability::CapabilityGraphARM});
    Operand operand5({spv::Capability::CapabilityTensorsARM});

    std::vector<Operand> capabilityOps{operand1};
    std::vector<Operand> capabilityOps2{operand2};
    std::vector<Operand> capabilityOps3{operand3};
    std::vector<Operand> capabilityOps4{operand4};
    std::vector<Operand> capabilityOps5{operand5};
    Instruction capabilityInst(spv::Op::OpCapability, capabilityOps);
    Instruction capabilityInst2(spv::Op::OpCapability, capabilityOps2);
    Instruction capabilityInst3(spv::Op::OpCapability, capabilityOps3);
    Instruction capabilityInst4(spv::Op::OpCapability, capabilityOps4);
    Instruction capabilityInst5(spv::Op::OpCapability, capabilityOps5);

    InstructionList capabilityList{capabilityInst,
                                   capabilityInst2,
                                   capabilityInst3,
                                   capabilityInst4,
                                   capabilityInst5};
    writer.WriteBinary(capabilityList);
    writer.WriteBounds(6);

    std::string goldenSPIRV = "               OpCapability VulkanMemoryModel\n"
                              "               OpCapability Shader\n"
                              "               OpCapability Int8\n"
                              "               OpCapability GraphARM\n"
                              "               OpCapability TensorsARM\n";

    // Don't validate as end to end SPIR-V generation is required, so it will fail validation.
    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, SimpleWriterExtensions)
{
    // Test that we can create a writer and write simple operands with strings.
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    // Extensions
    std::string extGraph("SPV_ARM_graph");
    Operand operand1(LITERAL_STRING, extGraph);
    std::vector<Operand> extensionOps1{operand1};
    Instruction extensionInst1(spv::Op::OpExtension, extensionOps1);

    std::string extTensors("SPV_ARM_tensors");
    Operand operand2(LITERAL_STRING, extTensors);
    std::vector<Operand> extensionOps2{operand2};
    Instruction extensionInst2(spv::Op::OpExtension, extensionOps2);

    InstructionList extensionList{extensionInst1, extensionInst2};
    writer.WriteBinary(extensionList);

    std::string goldenSPIRV = "               OpExtension \"SPV_ARM_graph\"\n"
                              "               OpExtension \"SPV_ARM_tensors\"\n";

    // Don't validate as end to end SPIR-V generation is required, so it will fail validation.
    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, SimpleWriterTypes)
{
    // Test that we can create a writer and write simple operands with ID's.
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    // Types and constants
    ResId uint(1);
    Operand typeOp1({32U});
    Operand typeOp2({0U});
    std::vector<Operand> typeOps1{typeOp1, typeOp2};
    // %uint = OpTypeInt 32 0
    Instruction typeInst1(spv::Op::OpTypeInt, uint, typeOps1);

    ResId uint_0(2);
    Operand typeOp3(NUMBER_ID, uint.Get());
    Operand typeOp4({0U});
    std::vector<Operand> typeOps2{typeOp3, typeOp4};
    // %uint_0 = OpConstant %uint 0
    Instruction typeInst2(spv::Op::OpConstant, uint_0, typeOps2);

    ResId uint_2(3);
    Operand typeOp5(NUMBER_ID, uint.Get());
    Operand typeOp6({2U});
    std::vector<Operand> typeOps3{typeOp5, typeOp6};
    // %uint_2 = OpConstant %uint 2
    Instruction typeInst3(spv::Op::OpConstant, uint_2, typeOps3);

    InstructionList typeList{typeInst1,
                             typeInst2,
                             typeInst3};

    writer.WriteBinary(typeList);

    std::string goldenSPIRV = "       %uint = OpTypeInt 32 0\n"
                              "     %uint_0 = OpConstant %uint 0\n"
                              "     %uint_2 = OpConstant %uint 2\n";

    // Don't validate as end to end SPIR-V generation is required, so it will fail validation.
    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, WriteHeader)
{
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    std::vector<uint32_t> binary = writer.GetBinary();
    CheckHeader(binary, 0);
}

TEST(TOSA2SPIRV, WriteBounds)
{
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    EXPECT_TRUE(writer.GetBinary().at(3) == 0);
    writer.WriteBounds(5);
    EXPECT_TRUE(writer.GetBinary().at(3) == 5);
}

TEST(TOSA2SPIRV, ModuleCreation)
{
    auto module = graphbuilder::Module::Create();

    auto binary = module->Write();

    CheckHeader(binary, 2);
}

TEST(TOSA2SPIRV, ModuleContext)
{
    auto module = Module::Create();
    auto graph = module->AddGraph();

    auto input = Tensor::CreateInput(DataType::int32_t, std::vector<unsigned int>{1,1}, module);

    auto output = Tensor::CreateOutput(DataType::int8_t, std::vector<unsigned int>{1,1}, module);

    graph->AddCastOperator(input, output);

    module->SetTargetTOSAVersion(TOSAVersion::v0_80);

    std::string goldenSPIRV = GetPreSPIRVStr() +
            "          %3 = OpExtInstImport \"TOSA.000080.2\"\n"
            "               OpMemoryModel Logical Vulkan\n"
            "               OpDecorate %13 DescriptorSet 0\n"
            "               OpDecorate %13 Binding 0\n"
            "               OpDecorate %17 DescriptorSet 0\n"
            "               OpDecorate %17 Binding 1\n"
            "      %uchar = OpTypeInt 8 0\n"
            "       %uint = OpTypeInt 32 0\n"
            "     %uint_2 = OpConstant %uint 2\n"
            "%_arr_uint_uint_2 = OpTypeArray %uint %uint_2\n"
            "     %uint_1 = OpConstant %uint 1\n"
            "          %9 = OpConstantComposite %_arr_uint_uint_2 %uint_1 %uint_1\n"
            "         %10 = OpTypeTensorARM %uchar %uint_2 %9\n"
            "         %11 = OpTypeTensorARM %uint %uint_2 %9\n"
            "%_ptr_UniformConstant_11 = OpTypePointer UniformConstant %11\n"
            "     %uint_0 = OpConstant %uint 0\n"
            "%_ptr_UniformConstant_10 = OpTypePointer UniformConstant %10\n"
            "         %13 = OpVariable %_ptr_UniformConstant_11 UniformConstant\n"
            "         %17 = OpVariable %_ptr_UniformConstant_10 UniformConstant\n"
            "         %18 = OpTypeGraphARM 1 %11 %10\n"
            "               OpGraphEntryPointARM %1 \"Graph_1\" %13 %17\n"
            "          %1 = OpGraphARM %18\n"
            "         %15 = OpGraphInputARM %11 %uint_0\n"
            "          %2 = OpExtInst %10 %3 CAST %15\n"
            "               OpGraphSetOutputARM %2 %uint_0\n"
            "               OpGraphEndARM\n";

    auto binary = module->Write();
    const std::string outputStr(tosa2spirv::spirvwriter::DisassembleSPIRV(binary, false));

    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, ModuleCreationRaw)
{
    auto module = graphbuilder::Module::CreateRaw();

    auto binary = module->Write();

    CheckHeader(binary, 2);

    // Call Destroy method to safely delete the Module
    graphbuilder::Module::Destroy(module);
}

TEST(TOSA2SPIRV, WriteCapabilities)
{
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    // Write a single capability
    writer.WriteCapabilities({spv::Capability::CapabilityVulkanMemoryModel});

    // Write multiple capabilities
    writer.WriteCapabilities({spv::Capability::CapabilityShader,
                              spv::Capability::CapabilityInt8});

    std::string goldenSPIRV = "               OpCapability VulkanMemoryModel\n"
                              "               OpCapability Shader\n"
                              "               OpCapability Int8\n";

    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, WriteExtension)
{
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    writer.WriteExtension("SPV_ARM_graph");
    writer.WriteExtension("SPV_ARM_tensors");

    std::string goldenSPIRV = "               OpExtension \"SPV_ARM_graph\"\n"
                              "               OpExtension \"SPV_ARM_tensors\"\n";

    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, WriteExtendedInstSetImport)
{
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    auto resId = writer.WriteExtInstSetImport("TOSA.000080.2");
    EXPECT_EQ(1, resId.Get());

    std::string goldenSPIRV = "          %1 = OpExtInstImport \"TOSA.000080.2\"\n";

    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, WriteMemoryModel)
{
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    writer.WriteMemoryModel(spv::AddressingModel::AddressingModelLogical,
                            spv::MemoryModel::MemoryModelVulkan);

    std::string goldenSPIRV = "               OpMemoryModel Logical Vulkan\n";

    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, WriteMemoryModelDefault)
{
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    writer.WriteMemoryModel();

    std::string goldenSPIRV = "               OpMemoryModel Logical Vulkan\n";

    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, WriteDecoration)
{
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    // Save the position for WriteDecoration to use.
    writer.SaveDecorationPosition();

    uint32_t min = std::numeric_limits<uint32_t>::min();
    uint32_t max = std::numeric_limits<uint32_t>::max();
    writer.WriteDecoration(spv::DecorationDescriptorSet, min, 1);
    writer.WriteDecoration(spv::DecorationBinding, max, 1);

    std::string goldenSPIRV = "               OpDecorate %1 DescriptorSet " + std::to_string(min) + "\n"
                              "               OpDecorate %1 Binding " + std::to_string(max) + "\n";

    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, SimpleWriteTypesWithTypeRegistry)
{
    // Test that we can create a writer and write simple operands with ID's.
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    // Types and constants
    Operand typeOp1({32U});
    Operand typeOp2({0U});
    std::vector<Operand> typeOps1{typeOp1, typeOp2};
    // %uint = OpTypeInt 32 0
    TypeRegistry registry(writer, gen);
    auto uint = registry.CreateType(spv::Op::OpTypeInt, typeOps1);
    auto uint2 = registry.CreateType(spv::Op::OpTypeInt, typeOps1);

    Operand typeOp3(NUMBER_ID, uint.Get());
    Operand typeOp4({0U});
    std::vector<Operand> typeOps2{typeOp3, typeOp4};
    // %uint_0 = OpConstant %uint 0
    registry.CreateType(spv::Op::OpConstant, typeOps2);

    Operand typeOp5(NUMBER_ID, uint.Get());
    Operand typeOp6({2U});
    std::vector<Operand> typeOps3{typeOp5, typeOp6};
    // %uint_2 = OpConstant %uint 2
    registry.CreateType(spv::Op::OpConstant, typeOps3);

    std::string goldenSPIRV = "       %uint = OpTypeInt 32 0\n"
                              "     %uint_0 = OpConstant %uint 0\n"
                              "     %uint_2 = OpConstant %uint 2\n";

    // Don't validate as end to end SPIR-V generation is required, so it will fail validation.
    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, WriteTensorWithTypeRegistry)
{
    // Create an instruction for the type of the tensor.
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    TypeRegistry registry(writer, gen);

    Operand intTypeSize({32U});
    Operand intTypeSignedness({0U});
    std::vector<Operand> intTypeSizeAndSignedness{intTypeSize, intTypeSignedness};
    auto opInt = spv::Op::OpTypeInt;
    auto intTypeId = registry.CreateType(opInt, intTypeSizeAndSignedness);

    Operand rankTypeSize({32U});
    Operand rankTypeSignedness({0U});
    std::vector<Operand> rankTypeSizeAndSignedness{rankTypeSize, rankTypeSignedness};
    auto rankTypeId = registry.CreateType(spv::Op::OpTypeInt, rankTypeSizeAndSignedness);

    // Rank of tensor shape as a constant
    Operand rankTypeIdOp(OperandType::NUMBER_ID, rankTypeId.Get());
    Operand rankValue({4});
    std::vector<Operand> rankTypeIdAndValue{rankTypeIdOp, rankValue};
    // Result type, result id, value
    auto tensorRank = registry.CreateType(spv::Op::OpConstant, rankTypeIdAndValue);

    Operand intTypeIdOp(OperandType::NUMBER_ID, intTypeId.Get());
    Operand tensorRankIdOp(OperandType::NUMBER_ID, tensorRank.Get());
    std::vector<Operand> intTypeIdOpAndRankIdOp{intTypeIdOp, tensorRankIdOp};

    registry.CreateType(spv::Op::OpTypeTensorARM, intTypeIdOpAndRankIdOp);

    std::string goldenSPIRV = "       %uint = OpTypeInt 32 0\n"
                              "     %uint_4 = OpConstant %uint 4\n"
                              "          %3 = OpTypeTensorARM %uint %uint_4\n";

    // Don't validate as end to end SPIR-V generation is required, so it will fail validation.
    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, WritePointerWithTypeRegistry)
{
    // Test that we can create pointer types via the registry.
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    TypeRegistry registry(writer, gen);

    // First create int type that tensor contains
    Operand intTypeSize({32U});
    Operand intTypeSignedness({0U});
    std::vector<Operand> intTypeSizeAndSignedness{intTypeSize, intTypeSignedness};
    auto opInt = spv::Op::OpTypeInt;
    auto intTypeId = registry.CreateType(opInt, intTypeSizeAndSignedness);

    // Create uint type for rank
    Operand rankTypeSize({32U});
    Operand rankTypeSignedness({0U});
    std::vector<Operand> rankTypeSizeAndSignedness{rankTypeSize, rankTypeSignedness};
    auto rankTypeId = registry.CreateType(spv::Op::OpTypeInt, rankTypeSizeAndSignedness);

    // Rank of tensor shape as a constant
    Operand rankTypeIdOp(OperandType::NUMBER_ID, rankTypeId.Get());
    Operand rankValue({4});
    std::vector<Operand> rankTypeIdAndValue{rankTypeIdOp, rankValue};
    auto tensorRank = registry.CreateType(spv::Op::OpConstant, rankTypeIdAndValue);

    Operand intTypeIdOp(OperandType::NUMBER_ID, intTypeId.Get());
    Operand tensorRankIdOp(OperandType::NUMBER_ID, tensorRank.Get());
    std::vector<Operand> intTypeIdOpOpAndRankIdOp{intTypeIdOp, tensorRankIdOp};

    auto tensorId = registry.CreateType(spv::Op::OpTypeTensorARM, intTypeIdOpOpAndRankIdOp);

    // Proceed to create pointer instruction
    Operand uniformConstantOp({spv::StorageClassUniformConstant});
    Operand tensorIdOp(OperandType::NUMBER_ID, tensorId.Get());
    std::vector<Operand> uniformConstantAndPointerType{uniformConstantOp, tensorIdOp};
    // Create the pointer to tensor type.
    auto pointerTypeId = registry.CreateType(spv::Op::OpTypePointer, uniformConstantAndPointerType);

    // Create duplicate to test registry finds it.
    registry.CreateType(spv::Op::OpTypePointer, uniformConstantAndPointerType);

    std::string goldenSPIRV = "       %uint = OpTypeInt 32 0\n"
                              "     %uint_4 = OpConstant %uint 4\n"
                              "          %3 = OpTypeTensorARM %uint %uint_4\n"
                              "%_ptr_UniformConstant_3 = OpTypePointer UniformConstant %3\n";

    // Don't validate as end to end SPIR-V generation is required, so it will fail validation.
    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));
    EXPECT_EQ(goldenSPIRV, outputStr);

    Operand pointerTypeIdOp(OperandType::NUMBER_ID, pointerTypeId.Get());
    std::vector<Operand> opTypePointerResIdAndUniformConstant{pointerTypeIdOp, uniformConstantOp};

    // It is valid to have duplicates of OpVariable instructions such as:
    // %input_tensor_ptr = OpVariable %ptr_UniformConstant_int8tensor UniformConstant
    // %output_tensor_ptr = OpVariable %ptr_UniformConstant_int8tensor UniformConstant
    // Attempting to register a type using an invalid opcode should return invalid argument exception.
    EXPECT_THROW(registry.CreateType(spv::Op::OpVariable, opTypePointerResIdAndUniformConstant),
                 std::invalid_argument);
}

TEST(TOSA2SPIRV, WriteVectorWithTypeRegistry)
{
    // Test that we can create vector types via the registry.
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    TypeRegistry registry(writer, gen);

    // First create tensor for the pointer to point to.
    Operand intTypeSize({32U});
    Operand intTypeSignedness({0U});
    std::vector<Operand> intTypeSizeAndSignedness{intTypeSize, intTypeSignedness};
    auto opInt = spv::Op::OpTypeInt;
    auto intTypeId = registry.CreateType(opInt, intTypeSizeAndSignedness);

    // Create the Vector Type.
    Operand intTypeIdOp(OperandType::NUMBER_ID, intTypeId.Get());
    Operand vectorComponentCount({4U});
    std::vector<Operand> intTypeIdAndVectorComponents{intTypeIdOp, vectorComponentCount};
    registry.CreateType(spv::Op::OpTypeVector, intTypeIdAndVectorComponents);

    // Ensure duplication doesnt show up in spirv
    registry.CreateType(spv::Op::OpTypeVector, intTypeIdAndVectorComponents);

    std::string goldenSPIRV = "       %uint = OpTypeInt 32 0\n"
                              "     %v4uint = OpTypeVector %uint 4\n";

    // Don't validate as end to end SPIR-V generation is required, so it will fail validation.
    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));

    EXPECT_EQ(goldenSPIRV, outputStr);
}

TEST(TOSA2SPIRV, TestOutOfScopeTypeRegistry)
{
    // Test to ensure registry remains persistent when operands passed to CreateType go out of scope
    IdGenerator gen;
    Writer writer(gen);
    writer.WriteHeader();

    TypeRegistry registry(writer, gen);

    {
        // First create type for the pointer to point to.
        Operand intTypeSize({32U});
        Operand intTypeSignedness({0U});
        std::vector<Operand> intTypeSizeAndSignedness{intTypeSize, intTypeSignedness};
        auto opInt = spv::Op::OpTypeInt;
        auto intTypeId = registry.CreateType(opInt, intTypeSizeAndSignedness);

        // Create the Vector Type.
        Operand intTypeIdOp(OperandType::NUMBER_ID, intTypeId.Get());
        Operand vectorComponentCount({4U});
        std::vector<Operand> intTypeIdAndVectorComponents{intTypeIdOp, vectorComponentCount};
        registry.CreateType(spv::Op::OpTypeVector, intTypeIdAndVectorComponents);

        // Ensure duplication doesnt show up in spirv
        registry.CreateType(spv::Op::OpTypeVector, intTypeIdAndVectorComponents);

        // Allow above to go out of scope
    }
    // Recreate the same int type
    Operand intTypeSize({32U});
    Operand intTypeSignedness({0U});
    std::vector<Operand> intTypeSizeAndSignedness{intTypeSize, intTypeSignedness};
    auto opInt = spv::Op::OpTypeInt;
    auto intTypeId = registry.CreateType(opInt, intTypeSizeAndSignedness);
    Operand intTypeIdOp(OperandType::NUMBER_ID, intTypeId.Get());
    Operand vectorComponentCount({4U});
    std::vector<Operand> intTypeIdAndVectorComponents{intTypeIdOp, vectorComponentCount};

    // Ensure duplication doesnt show up in spirv
    registry.CreateType(spv::Op::OpTypeVector, intTypeIdAndVectorComponents);

    std::string goldenSPIRV = "       %uint = OpTypeInt 32 0\n"
                              "     %v4uint = OpTypeVector %uint 4\n";

    // Don't validate as end to end SPIR-V generation is required, so it will fail validation.
    const std::string outputStr(DisassembleSPIRV(writer.GetBinary(), false));

    EXPECT_EQ(goldenSPIRV, outputStr);
}