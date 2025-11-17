//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <AssemblyUtils.hpp>
#include <Graph.hpp>
#include <Module.hpp>
#include <SPIRVDefinitions.hpp>
#include <TosaForSpirvCodegen.hpp>
#include <ValidationUtils.hpp>

#include <gtest/gtest.h>
#include <spirv-tools/libspirv.hpp>
#include <spirvmodels/Header.hpp>

using namespace tfsc;
using namespace spv;
using namespace tosa;

// Helper method to check module header in binary is correct.
static void CheckHeader(const std::vector<uint32_t>& binary, const uint32_t bound)
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
TEST(TOSA_FOR_SPIRV_CODEGEN, BasicAssertions)
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(TOSA_FOR_SPIRV_CODEGEN, ModuleCreate)
{
    auto module = CreateModule(TOSAVersion{});
    EXPECT_TRUE(module);
}

TEST(TOSA_FOR_SPIRV_CODEGEN, InitialiseSpirvTools)
{
    // Test that we can initialise the SpirvTools correctly.
    const spvtools::SpirvTools tools{SPV_ENV_UNIVERSAL_1_6};
    GTEST_EXPECT_TRUE(tools.IsValid());
}

TEST(TOSA_FOR_SPIRV_CODEGEN, TestModuleInitialization)
{
    const auto module = CreateModule(TOSAVersion::v1_0);
    EXPECT_TRUE(module);

    const auto binary = WriteToBinary(module);
    const std::string outputStr(testutils::DisassembleSPIRV(binary, false));

    EXPECT_EQ(outputStr, spirvmodels::SPIRV_HEADER);
}

std::vector<int64_t> ExtractShapeFromTensor(const spirv::Instruction* tensorInstruction)
{
    const auto tensorShape = tensorInstruction->m_Operands[3].m_InstructionPtr;

    if (tensorShape->m_Opcode == spv::OpConstantCompositeReplicateEXT)
    {
        const unsigned int value = tensorShape->m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord;
        const auto array = tensorShape->m_Operands[0].m_InstructionPtr;
        const unsigned int size = array->m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord;
        return std::vector<int64_t>(size, value);
    }
    std::vector<int64_t> shapeVector;
    for (auto operandIt = std::next(tensorShape->m_Operands.begin(), 2); operandIt != tensorShape->m_Operands.end();
         ++operandIt)
    {
        shapeVector.push_back(operandIt->m_InstructionPtr->m_Operands[2].m_LiteralWord);
    }
    return shapeVector;
}

TEST(TOSA_FOR_SPIRV_CODEGEN, TestInputOutputOrder)
{
    const auto module = CreateModule(TOSAVersion{});
    EXPECT_TRUE(module);

    tosa::Graph graph(module);

    const auto input1 = graph.AddInput(tosa::Tensor{tosa::DataType::int32_t, {1}}, 0);
    graph.AddOutput(input1, 5);

    const auto input2 = graph.AddInput(tosa::Tensor{tosa::DataType::int32_t, {2}}, 1);
    const auto input3 = graph.AddInput(tosa::Tensor{tosa::DataType::int32_t, {3}}, 2);
    graph.AddOutput(input2, 6);
    graph.AddOutput(input3, 7);

    const auto input4 = graph.AddInput(tosa::Tensor{tosa::DataType::int32_t, {4}}, 3);
    const auto input5 = graph.AddInput(tosa::Tensor{tosa::DataType::int32_t, {5}}, 4);
    graph.AddOutput(input5, 8);
    graph.AddOutput(input4, 9);
    graph.AddOutput(input4, 10);

    graph.FinalizeGraph();

    const auto [graphTypeBegin, graphTypeEnd] = module->GetInstructionsOfType(spv::OpTypeGraphARM);
    EXPECT_EQ(std::distance(graphTypeBegin, graphTypeEnd), 1);

    const auto [entryPointBegin, entryPointEnd] = module->GetInstructionsOfType(spv::OpGraphEntryPointARM);
    EXPECT_EQ(std::distance(entryPointBegin, entryPointEnd), 1);
    const auto entryPointInstruction = *entryPointBegin;

    const auto graphTypeInstruction = *graphTypeBegin;
    EXPECT_EQ(graphTypeInstruction.m_Operands.size(), entryPointInstruction.m_Operands.size());
    constexpr unsigned int inputSize = 5;
    constexpr unsigned int outputSize = 6;
    EXPECT_EQ(graphTypeInstruction.m_Operands.size(), inputSize + outputSize + 2);
    EXPECT_EQ(graphTypeInstruction.m_Operands[1].m_LiteralWord, inputSize);

    // map the bindings in the module to the opVariables that will be the operands of GraphEntryPoint
    std::unordered_map<const spirv::Instruction*, unsigned int> opVariableToBindingId;
    auto [decorateBegin, decorateEnd] = module->GetInstructionsOfType(spv::OpDecorate);
    for (auto it = decorateBegin; it != decorateEnd; ++it)
    {
        if (it->m_Operands[1].m_LiteralWord != spv::DecorationBinding)
        {
            continue;
        }

        const auto opVariable = it->m_Operands[0].m_InstructionPtr;
        opVariableToBindingId.emplace(opVariable, it->m_Operands[2].m_LiteralWord);
    }

    // binding ids of entry points should now be in order
    unsigned int expectedBindingId = 0;
    for (auto opVariable = entryPointInstruction.m_Operands.begin() + 2;
         opVariable != entryPointInstruction.m_Operands.end();
         ++opVariable)
    {
        const unsigned int bindingId = opVariableToBindingId.at(opVariable->m_InstructionPtr);
        EXPECT_EQ(bindingId, expectedBindingId++);
    }

    // verify tensor type instructions are in order using their shapes
    unsigned int expectedShape = 1;
    auto graphTypeInputEnd = graphTypeInstruction.m_Operands.begin() + inputSize + 2;
    for (auto tensor = graphTypeInstruction.m_Operands.begin() + 2;
         tensor != graphTypeInstruction.m_Operands.begin() + inputSize + 2;
         ++tensor)
    {
        const auto tensorShape = ExtractShapeFromTensor(tensor->m_InstructionPtr);
        EXPECT_EQ(tensorShape[0], expectedShape++);
    }

    unsigned int index = 0;
    std::array<unsigned int, 6> expectedOutputShapes = {1, 2, 3, 5, 4, 4};
    for (auto tensor = graphTypeInputEnd; tensor != graphTypeInstruction.m_Operands.end(); ++tensor)
    {
        const auto tensorShape = ExtractShapeFromTensor(tensor->m_InstructionPtr);
        EXPECT_EQ(tensorShape[0], expectedOutputShapes[index++]);
    }
}

// Test CreateDataType Function
TEST(GraphTests, CreateDataType)
{

    const auto sharedModule = CreateModule(TOSAVersion{});

    auto int4 = CreateDataType(DataType::int4_t, *sharedModule);
    auto int16 = CreateDataType(DataType::int16_t, *sharedModule);
    auto int32 = CreateDataType(DataType::int32_t, *sharedModule);
    auto float16 = CreateDataType(DataType::float16_t, *sharedModule);
    auto float32 = CreateDataType(DataType::float32_t, *sharedModule);
    auto bfloat16 = CreateDataType(DataType::bfloat16_t, *sharedModule);
    auto boolType = CreateDataType(DataType::bool_t, *sharedModule);

    CheckDataType(int4.m_InstructionPtr, DataType::int4_t);
    CheckDataType(int16.m_InstructionPtr, DataType::int16_t);
    CheckDataType(int32.m_InstructionPtr, DataType::int32_t);
    CheckDataType(float16.m_InstructionPtr, DataType::float16_t);
    CheckDataType(float32.m_InstructionPtr, DataType::float32_t);
    CheckDataType(bfloat16.m_InstructionPtr, DataType::bfloat16_t);
    CheckDataType(boolType.m_InstructionPtr, DataType::bool_t);
}

TEST(GraphTests, CreateConstant)
{
    using namespace tosa;
    const auto sharedModule = CreateModule(TOSAVersion{});

    auto int4 = CreateConstant(123, DataType::int4_t, *sharedModule);
    auto int16 = CreateConstant(123, DataType::int16_t, *sharedModule);
    auto int32 = CreateConstant(123, DataType::int32_t, *sharedModule);
    auto int48 = CreateConstantDouble(123, 123, *sharedModule);
    auto float16 = CreateConstant(123, DataType::float16_t, *sharedModule);
    auto float32 = CreateConstant(123, DataType::float32_t, *sharedModule);
    auto bfloat16 = CreateConstant(123, DataType::bfloat16_t, *sharedModule);

    CheckConstant(int4.m_InstructionPtr, DataType::int4_t, 123);
    CheckConstant(int16.m_InstructionPtr, DataType::int16_t, 123);
    CheckConstant(int32.m_InstructionPtr, DataType::int32_t, 123);
    CheckConstant(int48.m_InstructionPtr, DataType::int48_t, 123, 123);
    CheckConstant(float16.m_InstructionPtr, DataType::float16_t, 123);
    CheckConstant(float32.m_InstructionPtr, DataType::float32_t, 123);
    CheckConstant(bfloat16.m_InstructionPtr, DataType::bfloat16_t, 123);

    auto boolTrue = CreateConstant(1, DataType::bool_t, *sharedModule);
    auto boolFalse = CreateConstant(0, DataType::bool_t, *sharedModule);

    EXPECT_EQ(boolTrue.m_InstructionPtr->GetOpCode(), OpConstantTrue);
    EXPECT_EQ(boolFalse.m_InstructionPtr->GetOpCode(), OpConstantFalse);
}

TEST(GraphTests, CreateConstantCompositeBasic)
{
    using namespace tosa;
    const auto sharedModule = CreateModule(TOSAVersion{});

    std::vector<uint32_t> values = {2, 2};
    std::vector<DataType> types = {DataType::int4_t,
                                   DataType::int8_t,
                                   DataType::int16_t,
                                   DataType::int32_t,
                                   DataType::uint8_t,
                                   DataType::uint16_t,
                                   DataType::uint32_t,
                                   DataType::float16_t,
                                   DataType::float32_t,
                                   DataType::bfloat16_t,
                                   DataType::bool_t};

    for (auto dt : types)
    {
        auto typeId = CreateDataType(dt, *sharedModule);
        auto result = CreateConstantComposite(values, typeId, *sharedModule);

        CheckConstantComposite(result.m_InstructionPtr, values, DataType::uint32_t, dt);
    }
}

TEST(GraphTests, CreateCreateConstantCompositeDouble)
{
    using namespace tosa;
    const auto sharedModule = CreateModule(TOSAVersion{});

    const std::vector<uint32_t> values = {2, 2};

    const auto typeId = CreateDataType(DataType::int48_t, *sharedModule);
    const auto result = CreateConstantComposite(values, typeId, *sharedModule, DataType::int48_t);

    CheckConstantComposite(result.m_InstructionPtr, values, DataType::int48_t, DataType::int48_t);
}

TEST(GraphTests, CreateConstantCompositeTypedBool)
{
    const auto sharedModule = CreateModule(TOSAVersion{});
    ASSERT_TRUE(sharedModule);

    // Construct a bool tensor type to use as the typeId
    Tensor boolTensor(tosa::DataType::bool_t, {3});
    const auto typeId = CreateTensor(boolTensor, *sharedModule);

    const std::vector<uint32_t> boolData = {1, 0, 1};

    const auto result = CreateConstantComposite(boolData, typeId, *sharedModule, tosa::DataType::bool_t);

    ASSERT_EQ(result.m_InstructionPtr->GetOpCode(), spv::OpConstantComposite);
    ASSERT_EQ(result.m_InstructionPtr->m_Operands.size(), boolData.size() + 2);

    // Verify each element is a bool constant
    for (size_t i = 0; i < boolData.size(); ++i)
    {
        const auto constant = result.m_InstructionPtr->m_Operands[i + 2].m_InstructionPtr;
        if (boolData[i] == 1)
        {
            EXPECT_EQ(constant->GetOpCode(), spv::OpConstantTrue) << "Expected OpConstantTrue for index " << i;
        }
        else
        {
            EXPECT_EQ(constant->GetOpCode(), spv::OpConstantFalse) << "Expected OpConstantFalse for index " << i;
        }
    }
}

TEST(GraphTests, CreateTensor)
{
    const auto sharedModule = CreateModule(TOSAVersion{});

    Tensor tensor(DataType::float32_t, {1, 3, 3, 1});
    auto result = CreateTensor(tensor, *sharedModule);
    CheckTensorType(result.m_InstructionPtr, DataType::float32_t, 4, {1, 3, 3, 1});

    Tensor emptyTensor(DataType::float32_t, {});
    auto emptyResult = CreateTensor(emptyTensor, *sharedModule);
    CheckTensorType(emptyResult.m_InstructionPtr, DataType::float32_t, 1, {});

    Tensor int32Tensor(DataType::int32_t, {2, 3, 4, 5});
    result = CreateTensor(int32Tensor, *sharedModule);
    CheckTensorType(result.m_InstructionPtr, DataType::int32_t, 4, {2, 3, 4, 5});

    Tensor int16Tensor(DataType::int16_t, {2, 3, 4, 5});
    result = CreateTensor(int16Tensor, *sharedModule);
    CheckTensorType(result.m_InstructionPtr, DataType::int16_t, 4, {2, 3, 4, 5});

    Tensor int8Tensor(DataType::int8_t, {2, 3, 4, 5});
    result = CreateTensor(int8Tensor, *sharedModule);
    CheckTensorType(result.m_InstructionPtr, DataType::int8_t, 4, {2, 3, 4, 5});
}

// Test Graph Constructor
TEST(GraphTests, Constructor)
{
    const auto sharedModule = CreateModule(TOSAVersion{});
    Graph graph(sharedModule, "TestGraph");

    EXPECT_EQ(graph.GetName(), "TestGraph");
}

// Test AddOperator
TEST(GraphTests, AddOperator)
{
    const auto sharedModule = CreateModule(TOSAVersion{});
    Graph graph(sharedModule, "TestGraph");

    std::vector<ResId> inputs = {ResId(1)};

    Tensor tensor(DataType::int32_t, {2, 3, 4});
    std::vector<Tensor> outputs = {tensor};
    std::vector<Attribute> attributes = {};

    auto result = graph.AddOperator(OperatorEnum::Add, inputs, outputs, attributes);

    EXPECT_FALSE(result.empty());
}

TEST(GraphTests, AddInput)
{
    const auto sharedModule = CreateModule(TOSAVersion{});
    Graph graph(sharedModule, "TestGraph");

    Tensor inputTensor(DataType::float32_t, {4, 4});
    ResId res1 = graph.AddInput(inputTensor, 1);
    ResId res2 = graph.AddInput(inputTensor, 2);

    EXPECT_TRUE(res1 != nullptr);
    EXPECT_TRUE(res2 != nullptr);
}

// Test AddExternalGraphConstant
TEST(GraphTests, AddExternalGraphConstant)
{
    const auto sharedModule = CreateModule(TOSAVersion{});
    Graph graph(sharedModule, "TestGraph");

    Tensor tensor(DataType::int32_t, {2, 2});
    ResId res = graph.AddGraphConstant(tensor);

    EXPECT_TRUE(res != nullptr);
}

// Test FinalizeGraph
TEST(GraphTests, FinalizeGraph)
{
    const auto sharedModule = CreateModule(TOSAVersion{});
    Graph graph(sharedModule, "TestGraph");

    graph.FinalizeGraph();

    auto range = sharedModule->GetInstructionsOfType(spv::OpGraphEndARM);
    EXPECT_NE(range.first, range.second);
}

// Test FinalizeGraphLoop
TEST(GraphTests, FinalizeGraphLoop)
{
    const auto sharedModule = CreateModule(TOSAVersion{});
    Graph graph(sharedModule, "TestGraph");

    Tensor inputTensor(DataType::int32_t, {2, 2});
    ResId inputRes = graph.AddInput(inputTensor, 0);

    Tensor outputTensor(DataType::int32_t, {2, 2});
    ResId outputRes = graph.AddGraphConstant(outputTensor);

    graph.AddOutput(outputRes, 1);

    graph.FinalizeGraph();

    auto range = sharedModule->GetInstructionsOfType(spv::OpGraphEndARM);
    EXPECT_NE(range.first, range.second);
}

TEST(GraphTests, CreateAttribute)
{
    const auto sharedModule = CreateModule(TOSAVersion{});

    std::vector<uint32_t> data = {12, 12};
    Attribute attribute(data, DataType::uint32_t);

    auto result = CreateAttribute(attribute, *sharedModule);
    CheckConstantComposite(result.m_InstructionPtr, data, DataType::uint32_t, DataType::uint32_t);
}

TEST(GraphTests, AttributeConversion)
{
    int32_t negativeValue = -1;
    std::vector<int32_t> input = {negativeValue};

    // Construct Attribute using uint32_t as target type
    Attribute attr1(input, DataType::uint32_t);

    const auto& dataUint32 = attr1.GetData();
    ASSERT_EQ(dataUint32.size(), 1);
    EXPECT_EQ(dataUint32[0], static_cast<uint32_t>(negativeValue));
}
