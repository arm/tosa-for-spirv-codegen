//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Graph.hpp>
#include <OpTestUtils.hpp>
#include <Operand.hpp>
#include <SpirvParser.hpp>
#include <Tensor.hpp>
#include <TosaOperator.hpp>
#include <tosa2spirv.hpp>

#include <stdexcept>

#include <gtest/gtest.h>

using namespace spirv2tosa;

bool GetTosaOperatorTest(TosaOperator op)
{
    tosa2spirv::tosa::Graph graph{tosa2spirv::CreateModule(tosa2spirv::TOSAVersion::v1_0)};

    std::vector<tosa2spirv::tosa::ResId> inputIds;
    for (size_t idx = 0; idx < op.inputs.size(); ++idx)
    {
        switch (op.inputs[idx].type)
        {
            case TosaInputType::Dynamic: inputIds.push_back(graph.AddInput(op.inputs[idx].GetTensor(), idx)); break;
            case TosaInputType::GraphConstant:
                inputIds.push_back(graph.AddGraphConstant(op.inputs[idx].GetTensor()));
                break;
            case TosaInputType::TensorConstant:
                inputIds.push_back(graph.AddTensorConstant(op.inputs[idx].GetAttribute()));
                break;
        }
    }

    std::vector<tosa2spirv::tosa::Tensor> outputTensors;
    for (const auto& output : op.outputs)
    {
        outputTensors.push_back(output.GetTensor());
    }
    const auto operatorId = graph.AddOperator(op.op, inputIds, outputTensors, op.attributes);

    const auto output = operatorId[0];
    const TosaOperator generatedOp = GetTosaOperator(
        output->GetOpCode() == spv::OpCompositeExtract ? *output->m_Operands[2].m_InstructionPtr : *output);

    return CompareTosaOperatorData(op, generatedOp);
}

TEST(Spirv2TosaTest, GetDataTypeFromInstructionTest)
{
    tosa2spirv::spirv::Instruction boolInstruction{spv::Op::OpTypeBool, {tosa2spirv::spirv::Operand{}}};
    EXPECT_EQ(GetDataTypeFromInstruction(boolInstruction), tosa2spirv::tosa::DataType::bool_t);

    tosa2spirv::spirv::Instruction intInstruction{
        spv::Op::OpTypeInt,
        {tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{8u}, tosa2spirv::spirv::Operand{0u}}};
    EXPECT_EQ(GetDataTypeFromInstruction(intInstruction), tosa2spirv::tosa::DataType::uint8_t);
    intInstruction.m_Operands[1].m_LiteralWord = 16;
    EXPECT_EQ(GetDataTypeFromInstruction(intInstruction), tosa2spirv::tosa::DataType::uint16_t);
    intInstruction.m_Operands[1].m_LiteralWord = 32;
    EXPECT_EQ(GetDataTypeFromInstruction(intInstruction), tosa2spirv::tosa::DataType::uint32_t);
    intInstruction.m_Operands[1].m_LiteralWord = 64;
    EXPECT_EQ(GetDataTypeFromInstruction(intInstruction), tosa2spirv::tosa::DataType::int48_t);
    intInstruction.m_Operands[1].m_LiteralWord = 65;
    EXPECT_THROW(GetDataTypeFromInstruction(intInstruction), std::invalid_argument);

    tosa2spirv::spirv::Instruction floatInstruction{spv::Op::OpTypeFloat,
                                                    {tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{16u}}};
    EXPECT_EQ(GetDataTypeFromInstruction(floatInstruction), tosa2spirv::tosa::DataType::float16_t);
    floatInstruction.m_Operands[1].m_LiteralWord = 32;
    EXPECT_EQ(GetDataTypeFromInstruction(floatInstruction), tosa2spirv::tosa::DataType::float32_t);
    floatInstruction.m_Operands[1].m_LiteralWord = 64;
    EXPECT_THROW(GetDataTypeFromInstruction(floatInstruction), std::invalid_argument);
    floatInstruction.m_Operands[1].m_LiteralWord = 16;
    floatInstruction.m_Operands.emplace_back(0u);
    EXPECT_EQ(GetDataTypeFromInstruction(floatInstruction), tosa2spirv::tosa::DataType::bfloat16_t);
    floatInstruction.m_Operands[2].m_LiteralWord = 1u;
    EXPECT_THROW(GetDataTypeFromInstruction(floatInstruction), std::invalid_argument);

    tosa2spirv::spirv::Instruction incorrectInstruction{
        spv::Op::OpTypeArray,
        {tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{0u}}};
    EXPECT_THROW(GetDataTypeFromInstruction(incorrectInstruction), std::invalid_argument);
}

TEST(Spirv2TosaTest, GetTensorShapeFromInstructionTest)
{
    tosa2spirv::spirv::Instruction const1Instruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{1u}}};
    tosa2spirv::spirv::Instruction const2Instruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{2u}}};
    tosa2spirv::spirv::Instruction const3Instruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{3u}}};
    tosa2spirv::spirv::Instruction const4Instruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{4u}}};
    tosa2spirv::spirv::Instruction arrayInstruction{
        spv::Op::OpTypeArray,
        {tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{&const4Instruction}}};
    tosa2spirv::spirv::Instruction replicateShape{spv::Op::OpConstantCompositeReplicateEXT,
                                                  {tosa2spirv::spirv::Operand{&arrayInstruction},
                                                   tosa2spirv::spirv::Operand{},
                                                   tosa2spirv::spirv::Operand{&const2Instruction}}};
    EXPECT_EQ(GetTensorShapeFromInstruction(replicateShape), std::vector<uint32_t>({2, 2, 2, 2}));

    tosa2spirv::spirv::Instruction constCompositeShape{spv::Op::OpConstantComposite,
                                                       {tosa2spirv::spirv::Operand{&arrayInstruction},
                                                        tosa2spirv::spirv::Operand{},
                                                        tosa2spirv::spirv::Operand{&const1Instruction},
                                                        tosa2spirv::spirv::Operand{&const2Instruction},
                                                        tosa2spirv::spirv::Operand{&const3Instruction},
                                                        tosa2spirv::spirv::Operand{&const4Instruction}}};
    EXPECT_EQ(GetTensorShapeFromInstruction(constCompositeShape), std::vector<uint32_t>({1, 2, 3, 4}));

    tosa2spirv::spirv::Instruction incorrectInstruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{5u}}};
    EXPECT_THROW(GetTensorShapeFromInstruction(incorrectInstruction), std::invalid_argument);
}

TEST(Spirv2TosaTest, GetTensorFromInstructionTest)
{
    tosa2spirv::spirv::Instruction typeInstruction{
        spv::Op::OpTypeInt,
        {tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{32u}, tosa2spirv::spirv::Operand{0u}}};
    tosa2spirv::spirv::Instruction constInstruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{&typeInstruction}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{4u}}};
    tosa2spirv::spirv::Instruction const2Instruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{&typeInstruction}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{2u}}};
    tosa2spirv::spirv::Instruction arrayInstruction{
        spv::Op::OpTypeArray,
        {tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{&constInstruction}}};
    tosa2spirv::spirv::Instruction shapeInstruction{spv::Op::OpConstantCompositeReplicateEXT,
                                                    {tosa2spirv::spirv::Operand{&arrayInstruction},
                                                     tosa2spirv::spirv::Operand{},
                                                     tosa2spirv::spirv::Operand{&const2Instruction}}};
    tosa2spirv::spirv::Instruction tensorInstruction{spv::Op::OpTypeTensorARM,
                                                     {tosa2spirv::spirv::Operand{},
                                                      tosa2spirv::spirv::Operand{&typeInstruction},
                                                      tosa2spirv::spirv::Operand{&constInstruction},
                                                      tosa2spirv::spirv::Operand{&shapeInstruction}}};
    EXPECT_EQ(GetTensorFromInstruction(tensorInstruction),
              tosa2spirv::tosa::Tensor(tosa2spirv::tosa::DataType::uint32_t,
                                       tosa2spirv::tosa::Tensor::TensorShape({2, 2, 2, 2})));

    tosa2spirv::spirv::Instruction incorrectInstruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{5u}}};
    EXPECT_THROW(GetTensorFromInstruction(incorrectInstruction), std::invalid_argument);
}

TEST(Spirv2TosaTest, GetAttributeFromInstructionTest)
{
    tosa2spirv::spirv::Instruction intInstruction{
        spv::Op::OpTypeInt,
        {tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{32u}, tosa2spirv::spirv::Operand{0u}}};
    tosa2spirv::spirv::Instruction boolInstruction{spv::Op::OpTypeBool, {tosa2spirv::spirv::Operand{}}};

    tosa2spirv::spirv::Instruction trueInstruction{
        spv::Op::OpConstantTrue,
        {tosa2spirv::spirv::Operand{&boolInstruction}, tosa2spirv::spirv::Operand{}}};
    EXPECT_EQ(GetAttributeFromInstruction(trueInstruction),
              tosa2spirv::tosa::Attribute({1}, tosa2spirv::tosa::DataType::bool_t, {1}));
    tosa2spirv::spirv::Instruction falseInstruction{
        spv::Op::OpConstantFalse,
        {tosa2spirv::spirv::Operand{&boolInstruction}, tosa2spirv::spirv::Operand{}}};
    EXPECT_EQ(GetAttributeFromInstruction(falseInstruction),
              tosa2spirv::tosa::Attribute({0}, tosa2spirv::tosa::DataType::bool_t, {1}));

    tosa2spirv::spirv::Instruction constInstruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{&intInstruction}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{2u}}};
    tosa2spirv::spirv::Instruction constInstruction0{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{&intInstruction}, tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{0u}}};
    EXPECT_EQ(GetAttributeFromInstruction(constInstruction),
              tosa2spirv::tosa::Attribute({2}, tosa2spirv::tosa::DataType::uint32_t, {1}));

    tosa2spirv::spirv::Instruction arrayInstruction{
        spv::Op::OpTypeArray,
        {tosa2spirv::spirv::Operand{}, tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{&constInstruction}}};
    tosa2spirv::spirv::Instruction shapeInstruction{spv::Op::OpConstantCompositeReplicateEXT,
                                                    {tosa2spirv::spirv::Operand{&arrayInstruction},
                                                     tosa2spirv::spirv::Operand{},
                                                     tosa2spirv::spirv::Operand{&constInstruction}}};
    tosa2spirv::spirv::Instruction tensorInstruction{spv::Op::OpTypeTensorARM,
                                                     {tosa2spirv::spirv::Operand{},
                                                      tosa2spirv::spirv::Operand{&intInstruction},
                                                      tosa2spirv::spirv::Operand{&constInstruction},
                                                      tosa2spirv::spirv::Operand{&shapeInstruction}}};

    tosa2spirv::spirv::Instruction zeroInstruction{
        spv::Op::OpConstantNull,
        {tosa2spirv::spirv::Operand{&tensorInstruction}, tosa2spirv::spirv::Operand{}}};
    EXPECT_EQ(GetAttributeFromInstruction(zeroInstruction),
              tosa2spirv::tosa::Attribute({0, 0, 0, 0}, tosa2spirv::tosa::DataType::uint32_t, {2, 2}));
    tosa2spirv::spirv::Instruction replicateInstruction{spv::Op::OpConstantCompositeReplicateEXT,
                                                        {tosa2spirv::spirv::Operand{&tensorInstruction},
                                                         tosa2spirv::spirv::Operand{},
                                                         tosa2spirv::spirv::Operand{&constInstruction}}};
    EXPECT_EQ(GetAttributeFromInstruction(replicateInstruction),
              tosa2spirv::tosa::Attribute({2, 2, 2, 2}, tosa2spirv::tosa::DataType::uint32_t, {2, 2}));
    tosa2spirv::spirv::Instruction compositeInstruction{spv::Op::OpConstantComposite,
                                                        {tosa2spirv::spirv::Operand{&tensorInstruction},
                                                         tosa2spirv::spirv::Operand{},
                                                         tosa2spirv::spirv::Operand{&constInstruction0},
                                                         tosa2spirv::spirv::Operand{&constInstruction},
                                                         tosa2spirv::spirv::Operand{&constInstruction0},
                                                         tosa2spirv::spirv::Operand{&constInstruction}}};
    EXPECT_EQ(GetAttributeFromInstruction(compositeInstruction),
              tosa2spirv::tosa::Attribute({0, 2, 0, 2}, tosa2spirv::tosa::DataType::uint32_t, {2, 2}));
}

TEST(Spirv2TosaTest, GetTosaOperatorBasicTest)
{
    // NOTE: This fails if supplied with a data type that collapses to a different one when converting back:
    //       data type map not 1-1 and/or TOSA types in the 0.80 spec don't cover all Tensor::DataType definitions
    //       (notably, the Tosa 0.80 spec does not differentiate signed and unsigned ints, thus supplying a signed
    //       int type in the test will lead to failure, as the type is converted back to the same type but signed)
    TosaOperator op;
    op.op = tosa2spirv::tosa::OperatorEnum::Add;
    op.inputs.emplace_back(
        tosa2spirv::tosa::Tensor{tosa2spirv::tosa::DataType::uint32_t, tosa2spirv::tosa::Tensor::TensorShape{1}});
    op.inputs.emplace_back(
        tosa2spirv::tosa::Tensor{tosa2spirv::tosa::DataType::uint32_t, tosa2spirv::tosa::Tensor::TensorShape{1}},
        TosaInputType::Dynamic,
        0,
        0,
        1);
    op.outputs.emplace_back(
        tosa2spirv::tosa::Tensor{tosa2spirv::tosa::DataType::uint32_t, tosa2spirv::tosa::Tensor::TensorShape{1}});
    EXPECT_TRUE(GetTosaOperatorTest(op));

    op.op = tosa2spirv::tosa::OperatorEnum::ArithmeticRightShift;
    op.attributes.emplace_back(std::vector<uint32_t>{1},
                               tosa2spirv::tosa::DataType::bool_t,
                               tosa2spirv::tosa::Tensor::TensorShape{1});
    EXPECT_TRUE(GetTosaOperatorTest(op));

    op.op = tosa2spirv::tosa::OperatorEnum::Fft2d;
    op.attributes.emplace_back(std::vector<uint32_t>{1},
                               tosa2spirv::tosa::DataType::bool_t,
                               tosa2spirv::tosa::Tensor::TensorShape{1});
    op.outputs.emplace_back(
        tosa2spirv::tosa::Tensor{tosa2spirv::tosa::DataType::uint32_t, tosa2spirv::tosa::Tensor::TensorShape{1}});
    EXPECT_TRUE(GetTosaOperatorTest(op));
}

TEST(Spirv2TosaTest, GetTosaOperatorTest)
{
    TosaOperator op;
    op.op = tosa2spirv::tosa::OperatorEnum::AvgPool2d;
    op.inputs.emplace_back(
        tosa2spirv::tosa::Tensor{tosa2spirv::tosa::DataType::uint32_t, tosa2spirv::tosa::Tensor::TensorShape{4}});
    op.outputs.emplace_back(
        tosa2spirv::tosa::Tensor{tosa2spirv::tosa::DataType::uint32_t, tosa2spirv::tosa::Tensor::TensorShape{4}});
    op.attributes.emplace_back(std::vector<uint32_t>{2, 3},
                               tosa2spirv::tosa::DataType::uint32_t,
                               tosa2spirv::tosa::Tensor::TensorShape{2});
    op.attributes.emplace_back(std::vector<uint32_t>{0, 0},
                               tosa2spirv::tosa::DataType::uint32_t,
                               tosa2spirv::tosa::Tensor::TensorShape{2});
    op.attributes.emplace_back(std::vector<uint32_t>{1, 1, 1, 1},
                               tosa2spirv::tosa::DataType::uint32_t,
                               tosa2spirv::tosa::Tensor::TensorShape{4});
    op.attributes.emplace_back(std::vector<uint32_t>{1},
                               tosa2spirv::tosa::DataType::uint32_t,
                               tosa2spirv::tosa::Tensor::TensorShape{1});
    op.inputs.emplace_back(tosa2spirv::tosa::Attribute{std::vector<uint32_t>{0},
                                                       tosa2spirv::tosa::DataType::uint32_t,
                                                       tosa2spirv::tosa::Tensor::TensorShape{1}},
                           TosaInputType::TensorConstant);
    op.inputs.emplace_back(tosa2spirv::tosa::Attribute{std::vector<uint32_t>{0},
                                                       tosa2spirv::tosa::DataType::uint32_t,
                                                       tosa2spirv::tosa::Tensor::TensorShape{1}},
                           TosaInputType::TensorConstant);
    EXPECT_TRUE(GetTosaOperatorTest(op));
}

TEST(Spirv2TosaTest, GetTosaOperatorErrorTest)
{
    tosa2spirv::spirv::Instruction instruction{spv::Op::OpAny, {}};
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);

    instruction.m_Opcode = spv::Op::OpExtInst;
    instruction.m_Operands.emplace_back(0u);
    instruction.m_Operands.emplace_back(0u);
    instruction.m_Operands.emplace_back(0u);
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);

    instruction.m_Operands.emplace_back(128u);
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);
}

TEST(Spirv2TosaTest, GetTosaOperatorErrorTest2)
{
    // Set up spirv Instruction with errors to test error checking
    tosa2spirv::spirv::Instruction typeInstruction{
        spv::Op::OpTypeInt,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{0u}}};
    tosa2spirv::spirv::Instruction shapeValueInstruction{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{1u}}};
    tosa2spirv::spirv::Instruction shapeArrayInstruction{spv::Op::OpTypeTensorARM,
                                                         {tosa2spirv::spirv::Operand{0u},
                                                          tosa2spirv::spirv::Operand{0u},
                                                          tosa2spirv::spirv::Operand{&shapeValueInstruction}}};
    tosa2spirv::spirv::Instruction shapeInstruction{spv::Op::OpAny,
                                                    {tosa2spirv::spirv::Operand{&shapeArrayInstruction},
                                                     tosa2spirv::spirv::Operand{0u},
                                                     tosa2spirv::spirv::Operand{&shapeValueInstruction}}};
    tosa2spirv::spirv::Instruction outputTensor{spv::Op::OpAny,
                                                {tosa2spirv::spirv::Operand{0u},
                                                 tosa2spirv::spirv::Operand{&typeInstruction},
                                                 tosa2spirv::spirv::Operand{0u},
                                                 tosa2spirv::spirv::Operand{&shapeInstruction}}};
    tosa2spirv::spirv::Instruction invalidInstruction{spv::Op::OpExtInstImport, {tosa2spirv::spirv::Operand{0u}}};
    tosa2spirv::spirv::Instruction instruction{spv::Op::OpExtInst,
                                               {tosa2spirv::spirv::Operand{&outputTensor},
                                                tosa2spirv::spirv::Operand{0u},
                                                tosa2spirv::spirv::Operand{0u},
                                                tosa2spirv::spirv::Operand{23u},
                                                tosa2spirv::spirv::Operand{&invalidInstruction}}};
    // Expect invalid argument throw for broken instruction
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);

    // Fixing data type, still invalid
    typeInstruction.m_Operands[1].m_LiteralWord = 32u;
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);

    // Fixing tensor shape, still invalid
    shapeInstruction.m_Opcode = spv::Op::OpConstantCompositeReplicateEXT;
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);

    // Fixing output tensor instruction, still invalid
    outputTensor.m_Opcode = spv::Op::OpTypeTensorARM;
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);

    tosa2spirv::spirv::Instruction inputBindingId{
        spv::Op::OpConstant,
        {tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{0u}, tosa2spirv::spirv::Operand{0u}}};

    // Replacing invalid instruction with a valid one, expect no throw
    tosa2spirv::spirv::Instruction validInstruction{spv::Op::OpGraphInputARM,
                                                    {tosa2spirv::spirv::Operand{&outputTensor},
                                                     tosa2spirv::spirv::Operand{0u},
                                                     tosa2spirv::spirv::Operand{&inputBindingId}}};
    instruction.m_Operands[4].m_InstructionPtr = &validInstruction;
    instruction.m_Operands.push_back(tosa2spirv::spirv::Operand{&validInstruction});
    EXPECT_NO_THROW(GetTosaOperator(instruction));
}
