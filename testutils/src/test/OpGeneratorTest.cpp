//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <AssemblyUtils.hpp>
#include <Graph.hpp>
#include <Instruction.hpp>
#include <Module.hpp>
#include <OpGenerator.hpp>
#include <Operand.hpp>
#include <OperatorDefinitions.hpp>
#include <OperatorEnum.hpp>
#include <Tensor.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>
#include <spirv/unified1/spirv.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>

namespace testutils
{

bool GetTosaOperatorTest(TosaOperator op)
{
    tosa::Graph graph{tosa2spirv::CreateModule(TOSAVersion::v1_0)};

    std::vector<tosa::ResId> inputIds;
    for (size_t idx = 0; idx < op.inputs.size(); ++idx)
    {
        const auto inputId = graph.AddInput(op.inputs[idx], idx);
        inputIds.push_back(inputId);
    }
    for (const auto& tensorConst : op.tensorConstants)
    {
        const auto tensorId = graph.AddTensorConstant(tensorConst);
        inputIds.push_back(tensorId);
    }
    for (const auto& graphConst : op.graphConstants)
    {
        const auto graphConstId = graph.AddGraphConstant(graphConst);
        inputIds.push_back(graphConstId);
    }

    const auto operatorId = graph.AddOperator(op.op, inputIds, op.outputs, op.attributes);

    const auto output = operatorId[0];
    const TosaOperator generatedOp = GetTosaOperator(
        output->GetOpCode() == spv::OpCompositeExtract ? *output->m_Operands[2].m_InstructionPtr : *output);

    return op == generatedOp;
}

bool Spirv2operatorsTest(const std::vector<TosaOperator> ops,
                         const std::vector<TosaOperator> expectedOps,
                         OperatorComparator comparator)
{
    std::shared_ptr<spirv::Module> module = tosa2spirv::CreateModule(TOSAVersion::v1_0);
    tosa::Graph graph{module};

    // Generate graph with multiple operators
    for (const auto op : ops)
    {
        std::vector<tosa::ResId> inputIds;
        for (size_t inputIdx = 0; inputIdx < op.inputs.size(); ++inputIdx)
        {
            const auto inputId = graph.AddInput(op.inputs[inputIdx], inputIdx);
            inputIds.push_back(inputId);
        }
        for (const auto& tensorConst : op.tensorConstants)
        {
            const auto tensorId = graph.AddTensorConstant(tensorConst);
            inputIds.push_back(tensorId);
        }
        for (const auto& graphConst : op.graphConstants)
        {
            const auto graphConstId = graph.AddGraphConstant(graphConst);
            inputIds.push_back(graphConstId);
        }
        graph.AddOperator(op.op, inputIds, op.outputs, op.attributes);
    }

    auto generatedOps = Spirv2operators(module, comparator);

    if (expectedOps.size() != generatedOps.size())
        return false;
    // Check that the generated vector contains the same elements as the input vector, allowing for different order
    for (const auto op : expectedOps)
    {
        for (size_t opIdx = 0; opIdx < generatedOps.size(); ++opIdx)
        {
            if (op == generatedOps[opIdx])
            {
                generatedOps.erase(generatedOps.begin() + opIdx);
                opIdx = generatedOps.size();
            }
        }
    }
    return generatedOps.size() == 0;
}

bool Spirv2tosaTest(const std::vector<TosaOperator> ops, const std::vector<std::string> opDeclarations)
{
    std::shared_ptr<spirv::Module> module = tosa2spirv::CreateModule(TOSAVersion::v1_0);
    tosa::Graph graph{module};

    // Generate graph with multiple operators
    for (const auto op : ops)
    {
        std::vector<tosa::ResId> inputIds;
        for (size_t inputIdx = 0; inputIdx < op.inputs.size(); ++inputIdx)
        {
            const auto inputId = graph.AddInput(op.inputs[inputIdx], inputIdx);
            inputIds.push_back(inputId);
        }
        for (const auto& graphConst : op.graphConstants)
        {
            const auto constId = graph.AddGraphConstant(graphConst);
            inputIds.push_back(constId);
        }
        for (const auto& tensorConst : op.tensorConstants)
        {
            const auto constId = graph.AddTensorConstant(tensorConst);
            inputIds.push_back(constId);
        }
        graph.AddOperator(op.op, inputIds, op.outputs, op.attributes);
    }
    graph.FinalizeGraph();

    std::vector<std::string> declarations = opDeclarations;
    std::vector<std::string> generated = Spirv2tosa(module, defaultOpComparator);
    std::sort(declarations.begin(), declarations.end());
    std::sort(generated.begin(), generated.end());
    return declarations == generated;
}

TEST(Spirv2TosaTest, GetTosaOperatorBasicTest)
{
    // NOTE: This fails if supplied with a data type that collapses to a different one when converting back:
    //       data type map not 1-1 and/or TOSA types in the 0.80 spec don't cover all Tensor::DataType definitions
    //       (notably, the Tosa 0.80 spec does not differentiate signed and unsigned ints, thus supplying a signed
    //       int type in the test will lead to failure, as the type is converted back to the same type but signed)
    TosaOperator op;
    op.op = tosa::OperatorEnum::Add;
    op.inputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
    op.inputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
    op.outputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
    EXPECT_TRUE(GetTosaOperatorTest(op));

    op.op = tosa::OperatorEnum::ArithmeticRightShift;
    op.attributes.emplace_back(std::vector<uint32_t>{1}, tosa::DataType::bool_t, tosa::Tensor::TensorShape{1});
    EXPECT_TRUE(GetTosaOperatorTest(op));

    op.op = tosa::OperatorEnum::Fft2d;
    op.attributes.emplace_back(std::vector<uint32_t>{1}, tosa::DataType::bool_t, tosa::Tensor::TensorShape{1});
    op.outputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
    EXPECT_TRUE(GetTosaOperatorTest(op));
}

TEST(Spirv2TosaTest, GetTosaOperatorTest)
{
    TosaOperator op;
    op.op = tosa::OperatorEnum::AvgPool2d;
    op.inputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{4});
    op.outputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{4});
    op.attributes.emplace_back(std::vector<uint32_t>{2, 3}, tosa::DataType::uint32_t, tosa::Tensor::TensorShape{2});
    op.attributes.emplace_back(std::vector<uint32_t>{0, 0}, tosa::DataType::uint32_t, tosa::Tensor::TensorShape{2});
    op.attributes.emplace_back(std::vector<uint32_t>{1, 1, 1, 1},
                               tosa::DataType::uint32_t,
                               tosa::Tensor::TensorShape{4});
    op.attributes.emplace_back(std::vector<uint32_t>{1}, tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
    op.tensorConstants.emplace_back(std::vector<uint32_t>{0}, tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
    op.tensorConstants.emplace_back(std::vector<uint32_t>{0}, tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
    EXPECT_TRUE(GetTosaOperatorTest(op));
}

TEST(Spirv2TosaTest, GetTosaOperatorErrorTest)
{
    spirv::Instruction instruction{spv::Op::OpAny, {}};
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);

    instruction.m_Opcode = spv::Op::OpExtInst;
    instruction.m_Operands.emplace_back(0u);
    instruction.m_Operands.emplace_back(0u);
    instruction.m_Operands.emplace_back(0u);
    EXPECT_THROW(GetTosaOperator(instruction), std::invalid_argument);
}

TEST(Spirv2TosaTest, GetTosaOperatorErrorTest2)
{
    // Set up spirv Instruction with errors to test error checking
    spirv::Instruction typeInstruction{spv::Op::OpTypeInt,
                                       {spirv::Operand{0u}, spirv::Operand{0u}, spirv::Operand{0u}}};
    spirv::Instruction shapeValueInstruction{spv::Op::OpConstant,
                                             {spirv::Operand{0u}, spirv::Operand{0u}, spirv::Operand{1u}}};
    spirv::Instruction shapeArrayInstruction{
        spv::Op::OpTypeTensorARM,
        {spirv::Operand{0u}, spirv::Operand{0u}, spirv::Operand{&shapeValueInstruction}}};
    spirv::Instruction shapeInstruction{
        spv::Op::OpAny,
        {spirv::Operand{&shapeArrayInstruction}, spirv::Operand{0u}, spirv::Operand{&shapeValueInstruction}}};
    spirv::Instruction outputTensor{
        spv::Op::OpAny,
        {spirv::Operand{0u}, spirv::Operand{&typeInstruction}, spirv::Operand{0u}, spirv::Operand{&shapeInstruction}}};
    spirv::Instruction invalidInstruction{spv::Op::OpExtInstImport, {spirv::Operand{0u}}};
    spirv::Instruction instruction{spv::Op::OpExtInst,
                                   {spirv::Operand{&outputTensor},
                                    spirv::Operand{0u},
                                    spirv::Operand{0u},
                                    spirv::Operand{23u},
                                    spirv::Operand{&invalidInstruction}}};
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

    // Replacing invalid instruction with a valid one, expect no throw
    spirv::Instruction validInstruction{spv::Op::OpGraphInputARM, {spirv::Operand{&outputTensor}}};
    instruction.m_Operands[4].m_InstructionPtr = &validInstruction;
    instruction.m_Operands.push_back(spirv::Operand{&validInstruction});
    EXPECT_NO_THROW(GetTosaOperator(instruction));
}

TEST(Spirv2TosaTest, Spirv2OperatorsBasicTest)
{
    std::vector<TosaOperator> ops;
    for (size_t i = 0; i < 9; ++i)
    {
        TosaOperator op;
        op.op = static_cast<tosa::OperatorEnum>(static_cast<size_t>(tosa::OperatorEnum::BitwiseAnd) + i);
        op.inputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
        op.inputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
        op.outputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1});
        ops.push_back(op);
    }
    EXPECT_TRUE(Spirv2operatorsTest(ops, ops, defaultOpComparator));
}

TEST(Spirv2TosaTest, Spirv2OperatorsComparatorTest)
{
    std::vector<TosaOperator> ops;
    std::vector<TosaOperator> expectedOps;
    for (size_t i = 0; i < 9; ++i)
    {
        TosaOperator op;
        op.op = static_cast<tosa::OperatorEnum>(static_cast<size_t>(tosa::OperatorEnum::BitwiseAnd) + i);
        op.inputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1, static_cast<uint32_t>(i)});
        op.inputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1, static_cast<uint32_t>(i)});
        op.outputs.emplace_back(tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1, static_cast<uint32_t>(i)});
        ops.push_back(op);
        ops.push_back(op);
        expectedOps.push_back(op);
    }

    EXPECT_TRUE(Spirv2operatorsTest(ops,
                                    expectedOps,
                                    [](const TosaOperator& lhs, const TosaOperator& rhs) { return lhs.op < rhs.op; }));
}

TEST(Spirv2TosaTest, Spirv2TosaTest)
{
    std::vector<std::string> strings = {
        "const OperatorEnum op = OperatorEnum::Add;\n"
        "const std::vector<Tensor> inputs {{DataType::uint32_t, {1}}, {DataType::uint32_t, {1}}};\n"
        "const std::vector<Tensor> graphConstants {};\n"
        "const std::vector<Attribute> tensorConstants {};\n"
        "const std::vector<Tensor> outputs {{DataType::uint32_t, {1}}};\n"
        "const std::vector<Attribute> attributes {};",
        "const OperatorEnum op = OperatorEnum::Sub;\n"
        "const std::vector<Tensor> inputs {{DataType::uint32_t, {1}}};\n"
        "const std::vector<Tensor> graphConstants {{DataType::uint32_t, {1}}};\n"
        "const std::vector<Attribute> tensorConstants {};\n"
        "const std::vector<Tensor> outputs {{DataType::uint32_t, {1}}};\n"
        "const std::vector<Attribute> attributes {};",
        "const OperatorEnum op = OperatorEnum::ArithmeticRightShift;\n"
        "const std::vector<Tensor> inputs {{DataType::uint32_t, {1}}, {DataType::uint32_t, {1}}};\n"
        "const std::vector<Tensor> graphConstants {};\n"
        "const std::vector<Attribute> tensorConstants {};\n"
        "const std::vector<Tensor> outputs {{DataType::uint32_t, {1}}};\n"
        "const std::vector<Attribute> attributes {{std::initializer_list<uint32_t>{1}, DataType::uint32_t, {1}}};"};
    std::vector<TosaOperator> ops = {{tosa::OperatorEnum::Add,
                                      {tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}},
                                       tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}}},
                                      {},
                                      {},
                                      {tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}}},
                                      {}},
                                     {tosa::OperatorEnum::Sub,
                                      {tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}}},
                                      {tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}}},
                                      {},
                                      {tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}}},
                                      {}},
                                     {tosa::OperatorEnum::ArithmeticRightShift,
                                      {tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}},
                                       tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}}},
                                      {},
                                      {},
                                      {tosa::Tensor{tosa::DataType::uint32_t, tosa::Tensor::TensorShape{1}}},
                                      {tosa::Attribute{{1}, tosa::DataType::uint32_t, {1}}}}};

    EXPECT_TRUE(Spirv2tosaTest(ops, strings));
}

TEST(Spirv2TosaTest, GetOperatorNameTest)
{
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::ArgMax), "ArgMax");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::AvgPool2d), "AvgPool2d");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Conv2d), "Conv2d");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Conv3d), "Conv3d");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::DepthwiseConv2d), "DepthwiseConv2d");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Fft2d), "Fft2d");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Matmul), "Matmul");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::MaxPool2d), "MaxPool2d");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Rfft2d), "Rfft2d");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::TransposeConv2d), "TransposeConv2d");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Clamp), "Clamp");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Erf), "Erf");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Sigmoid), "Sigmoid");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Tanh), "Tanh");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Add), "Add");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::ArithmeticRightShift), "ArithmeticRightShift");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::BitwiseAnd), "BitwiseAnd");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::BitwiseOr), "BitwiseOr");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::BitwiseXor), "BitwiseXor");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::IntDiv), "IntDiv");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::LogicalAnd), "LogicalAnd");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::LogicalLeftShift), "LogicalLeftShift");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::LogicalRightShift), "LogicalRightShift");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::LogicalOr), "LogicalOr");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::LogicalXor), "LogicalXor");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Maximum), "Maximum");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Minimum), "Minimum");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Mul), "Mul");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Pow), "Pow");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Sub), "Sub");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Table), "Table");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Abs), "Abs");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::BitwiseNot), "BitwiseNot");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Ceil), "Ceil");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Clz), "Clz");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Cos), "Cos");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Exp), "Exp");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Floor), "Floor");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Log), "Log");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::LogicalNot), "LogicalNot");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Negate), "Negate");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Reciprocal), "Reciprocal");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Rsqrt), "Rsqrt");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Sin), "Sin");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Select), "Select");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Equal), "Equal");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Greater), "Greater");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::GreaterEqual), "GreaterEqual");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::ReduceAll), "ReduceAll");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::ReduceAny), "ReduceAny");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::ReduceMax), "ReduceMax");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::ReduceMin), "ReduceMin");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::ReduceProduct), "ReduceProduct");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::ReduceSum), "ReduceSum");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Concat), "Concat");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Pad), "Pad");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Reshape), "Reshape");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Reverse), "Reverse");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Slice), "Slice");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Tile), "Tile");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Transpose), "Transpose");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Gather), "Gather");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Scatter), "Scatter");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Resize), "Resize");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Cast), "Cast");
    EXPECT_EQ(tosa::GetOperatorName(tosa::OperatorEnum::Rescale), "Rescale");
}

} // namespace testutils
