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
#include <TosaForSpirvCodegen.hpp>

#include <stdexcept>

#include <gtest/gtest.h>

using namespace tosaregen;

bool GetTosaOperatorTest(TosaOperator op)
{
    tfsc::tosa::Graph graph{tfsc::CreateModule(tfsc::TOSAVersion::v1_0)};

    std::vector<tfsc::tosa::ResId> inputIds;
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

    std::vector<tfsc::tosa::Tensor> outputTensors;
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
    tfsc::spirv::Instruction boolInstruction{spv::Op::OpTypeBool, {tfsc::spirv::Operand{}}};
    EXPECT_EQ(GetDataTypeFromInstruction(boolInstruction), tfsc::tosa::DataType::bool_t);

    tfsc::spirv::Instruction intInstruction{
        spv::Op::OpTypeInt,
        {tfsc::spirv::Operand{}, tfsc::spirv::Operand{8u}, tfsc::spirv::Operand{0u}}};
    EXPECT_EQ(GetDataTypeFromInstruction(intInstruction), tfsc::tosa::DataType::uint8_t);
    intInstruction.m_Operands[1].m_LiteralWord = 16;
    EXPECT_EQ(GetDataTypeFromInstruction(intInstruction), tfsc::tosa::DataType::uint16_t);
    intInstruction.m_Operands[1].m_LiteralWord = 32;
    EXPECT_EQ(GetDataTypeFromInstruction(intInstruction), tfsc::tosa::DataType::uint32_t);
    intInstruction.m_Operands[1].m_LiteralWord = 64;
    EXPECT_EQ(GetDataTypeFromInstruction(intInstruction), tfsc::tosa::DataType::int48_t);
    intInstruction.m_Operands[1].m_LiteralWord = 65;
    EXPECT_THROW(GetDataTypeFromInstruction(intInstruction), std::invalid_argument);

    tfsc::spirv::Instruction floatInstruction{spv::Op::OpTypeFloat,
                                                    {tfsc::spirv::Operand{}, tfsc::spirv::Operand{16u}}};
    EXPECT_EQ(GetDataTypeFromInstruction(floatInstruction), tfsc::tosa::DataType::float16_t);
    floatInstruction.m_Operands[1].m_LiteralWord = 32;
    EXPECT_EQ(GetDataTypeFromInstruction(floatInstruction), tfsc::tosa::DataType::float32_t);
    floatInstruction.m_Operands[1].m_LiteralWord = 64;
    EXPECT_THROW(GetDataTypeFromInstruction(floatInstruction), std::invalid_argument);
    floatInstruction.m_Operands[1].m_LiteralWord = 16;
    floatInstruction.m_Operands.emplace_back(0u);
    EXPECT_EQ(GetDataTypeFromInstruction(floatInstruction), tfsc::tosa::DataType::bfloat16_t);
    floatInstruction.m_Operands[2].m_LiteralWord = 1u;
    EXPECT_THROW(GetDataTypeFromInstruction(floatInstruction), std::invalid_argument);

    tfsc::spirv::Instruction incorrectInstruction{
        spv::Op::OpTypeArray,
        {tfsc::spirv::Operand{}, tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{0u}}};
    EXPECT_THROW(GetDataTypeFromInstruction(incorrectInstruction), std::invalid_argument);
}

TEST(Spirv2TosaTest, GetTensorShapeFromInstructionTest)
{
    tfsc::spirv::Instruction const1Instruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{1u}}};
    tfsc::spirv::Instruction const2Instruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{2u}}};
    tfsc::spirv::Instruction const3Instruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{3u}}};
    tfsc::spirv::Instruction const4Instruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{4u}}};
    tfsc::spirv::Instruction arrayInstruction{
        spv::Op::OpTypeArray,
        {tfsc::spirv::Operand{}, tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{&const4Instruction}}};
    tfsc::spirv::Instruction replicateShape{spv::Op::OpConstantCompositeReplicateEXT,
                                                  {tfsc::spirv::Operand{&arrayInstruction},
                                                   tfsc::spirv::Operand{},
                                                   tfsc::spirv::Operand{&const2Instruction}}};
    EXPECT_EQ(GetTensorShapeFromInstruction(replicateShape), std::vector<uint32_t>({2, 2, 2, 2}));

    tfsc::spirv::Instruction constCompositeShape{spv::Op::OpConstantComposite,
                                                       {tfsc::spirv::Operand{&arrayInstruction},
                                                        tfsc::spirv::Operand{},
                                                        tfsc::spirv::Operand{&const1Instruction},
                                                        tfsc::spirv::Operand{&const2Instruction},
                                                        tfsc::spirv::Operand{&const3Instruction},
                                                        tfsc::spirv::Operand{&const4Instruction}}};
    EXPECT_EQ(GetTensorShapeFromInstruction(constCompositeShape), std::vector<uint32_t>({1, 2, 3, 4}));

    tfsc::spirv::Instruction incorrectInstruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{5u}}};
    EXPECT_THROW(GetTensorShapeFromInstruction(incorrectInstruction), std::invalid_argument);
}

TEST(Spirv2TosaTest, GetTensorFromInstructionTest)
{
    tfsc::spirv::Instruction typeInstruction{
        spv::Op::OpTypeInt,
        {tfsc::spirv::Operand{}, tfsc::spirv::Operand{32u}, tfsc::spirv::Operand{0u}}};
    tfsc::spirv::Instruction constInstruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{&typeInstruction}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{4u}}};
    tfsc::spirv::Instruction const2Instruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{&typeInstruction}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{2u}}};
    tfsc::spirv::Instruction arrayInstruction{
        spv::Op::OpTypeArray,
        {tfsc::spirv::Operand{}, tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{&constInstruction}}};
    tfsc::spirv::Instruction shapeInstruction{spv::Op::OpConstantCompositeReplicateEXT,
                                                    {tfsc::spirv::Operand{&arrayInstruction},
                                                     tfsc::spirv::Operand{},
                                                     tfsc::spirv::Operand{&const2Instruction}}};
    tfsc::spirv::Instruction tensorInstruction{spv::Op::OpTypeTensorARM,
                                                     {tfsc::spirv::Operand{},
                                                      tfsc::spirv::Operand{&typeInstruction},
                                                      tfsc::spirv::Operand{&constInstruction},
                                                      tfsc::spirv::Operand{&shapeInstruction}}};
    EXPECT_EQ(GetTensorFromInstruction(tensorInstruction),
              tfsc::tosa::Tensor(tfsc::tosa::DataType::uint32_t,
                                       tfsc::tosa::Tensor::TensorShape({2, 2, 2, 2})));

    tfsc::spirv::Instruction incorrectInstruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{5u}}};
    EXPECT_THROW(GetTensorFromInstruction(incorrectInstruction), std::invalid_argument);
}

TEST(Spirv2TosaTest, GetAttributeFromInstructionTest)
{
    tfsc::spirv::Instruction intInstruction{
        spv::Op::OpTypeInt,
        {tfsc::spirv::Operand{}, tfsc::spirv::Operand{32u}, tfsc::spirv::Operand{0u}}};
    tfsc::spirv::Instruction boolInstruction{spv::Op::OpTypeBool, {tfsc::spirv::Operand{}}};

    tfsc::spirv::Instruction trueInstruction{
        spv::Op::OpConstantTrue,
        {tfsc::spirv::Operand{&boolInstruction}, tfsc::spirv::Operand{}}};
    EXPECT_EQ(GetAttributeFromInstruction(trueInstruction),
              tfsc::tosa::Attribute({1}, tfsc::tosa::DataType::bool_t, {1}));
    tfsc::spirv::Instruction falseInstruction{
        spv::Op::OpConstantFalse,
        {tfsc::spirv::Operand{&boolInstruction}, tfsc::spirv::Operand{}}};
    EXPECT_EQ(GetAttributeFromInstruction(falseInstruction),
              tfsc::tosa::Attribute({0}, tfsc::tosa::DataType::bool_t, {1}));

    tfsc::spirv::Instruction constInstruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{&intInstruction}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{2u}}};
    tfsc::spirv::Instruction constInstruction0{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{&intInstruction}, tfsc::spirv::Operand{}, tfsc::spirv::Operand{0u}}};
    EXPECT_EQ(GetAttributeFromInstruction(constInstruction),
              tfsc::tosa::Attribute({2}, tfsc::tosa::DataType::uint32_t, {1}));

    tfsc::spirv::Instruction arrayInstruction{
        spv::Op::OpTypeArray,
        {tfsc::spirv::Operand{}, tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{&constInstruction}}};
    tfsc::spirv::Instruction shapeInstruction{spv::Op::OpConstantCompositeReplicateEXT,
                                                    {tfsc::spirv::Operand{&arrayInstruction},
                                                     tfsc::spirv::Operand{},
                                                     tfsc::spirv::Operand{&constInstruction}}};
    tfsc::spirv::Instruction tensorInstruction{spv::Op::OpTypeTensorARM,
                                                     {tfsc::spirv::Operand{},
                                                      tfsc::spirv::Operand{&intInstruction},
                                                      tfsc::spirv::Operand{&constInstruction},
                                                      tfsc::spirv::Operand{&shapeInstruction}}};

    tfsc::spirv::Instruction zeroInstruction{
        spv::Op::OpConstantNull,
        {tfsc::spirv::Operand{&tensorInstruction}, tfsc::spirv::Operand{}}};
    EXPECT_EQ(GetAttributeFromInstruction(zeroInstruction),
              tfsc::tosa::Attribute({0, 0, 0, 0}, tfsc::tosa::DataType::uint32_t, {2, 2}));
    tfsc::spirv::Instruction replicateInstruction{spv::Op::OpConstantCompositeReplicateEXT,
                                                        {tfsc::spirv::Operand{&tensorInstruction},
                                                         tfsc::spirv::Operand{},
                                                         tfsc::spirv::Operand{&constInstruction}}};
    EXPECT_EQ(GetAttributeFromInstruction(replicateInstruction),
              tfsc::tosa::Attribute({2, 2, 2, 2}, tfsc::tosa::DataType::uint32_t, {2, 2}));
    tfsc::spirv::Instruction compositeInstruction{spv::Op::OpConstantComposite,
                                                        {tfsc::spirv::Operand{&tensorInstruction},
                                                         tfsc::spirv::Operand{},
                                                         tfsc::spirv::Operand{&constInstruction0},
                                                         tfsc::spirv::Operand{&constInstruction},
                                                         tfsc::spirv::Operand{&constInstruction0},
                                                         tfsc::spirv::Operand{&constInstruction}}};
    EXPECT_EQ(GetAttributeFromInstruction(compositeInstruction),
              tfsc::tosa::Attribute({0, 2, 0, 2}, tfsc::tosa::DataType::uint32_t, {2, 2}));
}

TEST(Spirv2TosaTest, GetTosaOperatorBasicTest)
{
    // NOTE: This fails if supplied with a data type that collapses to a different one when converting back:
    //       data type map not 1-1 and/or TOSA types in the 0.80 spec don't cover all Tensor::DataType definitions
    //       (notably, the Tosa 0.80 spec does not differentiate signed and unsigned ints, thus supplying a signed
    //       int type in the test will lead to failure, as the type is converted back to the same type but signed)
    TosaOperator op;
    op.op = tfsc::tosa::OperatorEnum::Add;
    op.inputs.emplace_back(
        tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}});
    op.inputs.emplace_back(
        tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}},
        TosaInputType::Dynamic,
        0,
        0,
        1);
    op.outputs.emplace_back(
        tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}});
    EXPECT_TRUE(GetTosaOperatorTest(op));

    op.op = tfsc::tosa::OperatorEnum::ArithmeticRightShift;
    op.attributes.emplace_back(std::vector<uint32_t>{1},
                               tfsc::tosa::DataType::bool_t,
                               tfsc::tosa::Tensor::TensorShape{1});
    EXPECT_TRUE(GetTosaOperatorTest(op));

    op.op = tfsc::tosa::OperatorEnum::Fft2d;
    op.attributes.emplace_back(std::vector<uint32_t>{1},
                               tfsc::tosa::DataType::bool_t,
                               tfsc::tosa::Tensor::TensorShape{1});
    op.outputs.emplace_back(
        tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}});
    EXPECT_TRUE(GetTosaOperatorTest(op));
}

TEST(Spirv2TosaTest, GetTosaOperatorTest)
{
    TosaOperator op;
    op.op = tfsc::tosa::OperatorEnum::AvgPool2d;
    op.inputs.emplace_back(
        tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{4}});
    op.outputs.emplace_back(
        tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{4}});
    op.attributes.emplace_back(std::vector<uint32_t>{2, 3},
                               tfsc::tosa::DataType::uint32_t,
                               tfsc::tosa::Tensor::TensorShape{2});
    op.attributes.emplace_back(std::vector<uint32_t>{0, 0},
                               tfsc::tosa::DataType::uint32_t,
                               tfsc::tosa::Tensor::TensorShape{2});
    op.attributes.emplace_back(std::vector<uint32_t>{1, 1, 1, 1},
                               tfsc::tosa::DataType::uint32_t,
                               tfsc::tosa::Tensor::TensorShape{4});
    op.attributes.emplace_back(std::vector<uint32_t>{1},
                               tfsc::tosa::DataType::uint32_t,
                               tfsc::tosa::Tensor::TensorShape{1});
    op.inputs.emplace_back(tfsc::tosa::Attribute{std::vector<uint32_t>{0},
                                                       tfsc::tosa::DataType::uint32_t,
                                                       tfsc::tosa::Tensor::TensorShape{1}},
                           TosaInputType::TensorConstant);
    op.inputs.emplace_back(tfsc::tosa::Attribute{std::vector<uint32_t>{0},
                                                       tfsc::tosa::DataType::uint32_t,
                                                       tfsc::tosa::Tensor::TensorShape{1}},
                           TosaInputType::TensorConstant);
    EXPECT_TRUE(GetTosaOperatorTest(op));
}

TEST(Spirv2TosaTest, GetTosaOperatorErrorTest)
{
    tfsc::spirv::Instruction instruction{spv::Op::OpAny, {}};
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
    tfsc::spirv::Instruction typeInstruction{
        spv::Op::OpTypeInt,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{0u}}};
    tfsc::spirv::Instruction shapeValueInstruction{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{1u}}};
    tfsc::spirv::Instruction shapeArrayInstruction{spv::Op::OpTypeTensorARM,
                                                         {tfsc::spirv::Operand{0u},
                                                          tfsc::spirv::Operand{0u},
                                                          tfsc::spirv::Operand{&shapeValueInstruction}}};
    tfsc::spirv::Instruction shapeInstruction{spv::Op::OpAny,
                                                    {tfsc::spirv::Operand{&shapeArrayInstruction},
                                                     tfsc::spirv::Operand{0u},
                                                     tfsc::spirv::Operand{&shapeValueInstruction}}};
    tfsc::spirv::Instruction outputTensor{spv::Op::OpAny,
                                                {tfsc::spirv::Operand{0u},
                                                 tfsc::spirv::Operand{&typeInstruction},
                                                 tfsc::spirv::Operand{0u},
                                                 tfsc::spirv::Operand{&shapeInstruction}}};
    tfsc::spirv::Instruction invalidInstruction{spv::Op::OpExtInstImport, {tfsc::spirv::Operand{0u}}};
    tfsc::spirv::Instruction instruction{spv::Op::OpExtInst,
                                               {tfsc::spirv::Operand{&outputTensor},
                                                tfsc::spirv::Operand{0u},
                                                tfsc::spirv::Operand{0u},
                                                tfsc::spirv::Operand{23u},
                                                tfsc::spirv::Operand{&invalidInstruction}}};
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

    tfsc::spirv::Instruction inputBindingId{
        spv::Op::OpConstant,
        {tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{0u}, tfsc::spirv::Operand{0u}}};

    // Replacing invalid instruction with a valid one, expect no throw
    tfsc::spirv::Instruction validInstruction{spv::Op::OpGraphInputARM,
                                                    {tfsc::spirv::Operand{&outputTensor},
                                                     tfsc::spirv::Operand{0u},
                                                     tfsc::spirv::Operand{&inputBindingId}}};
    instruction.m_Operands[4].m_InstructionPtr = &validInstruction;
    instruction.m_Operands.push_back(tfsc::spirv::Operand{&validInstruction});
    EXPECT_NO_THROW(GetTosaOperator(instruction));
}
