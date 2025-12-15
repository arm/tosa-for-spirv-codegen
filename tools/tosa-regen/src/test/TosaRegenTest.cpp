//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <AssemblyUtils.hpp>
#include <Graph.hpp>
#include <Instruction.hpp>
#include <OpTestUtils.hpp>
#include <Operand.hpp>
#include <OperatorEnum.hpp>
#include <Tensor.hpp>
#include <TosaOperator.hpp>
#include <TosaSerializationParser.hpp>
#include <TosaRegen.hpp>
#include <TosaForSpirvCodegen.hpp>
#include <spirvmodels/TensorName.hpp>

#include <algorithm>
#include <cstdio>
#include <functional>
#include <stdexcept>
#include <string>
#include <sstream>
#include <regex>

#include <gtest/gtest.h>

#include <attribute.h>
#include <tosa_generated.h>
#include <tosa_serialization_handler.h>

using namespace tosaregen;

bool Spirv2tosaTest(const std::vector<TosaOperator> ops, const std::vector<std::string> opDeclarations)
{
    std::shared_ptr<tfsc::spirv::Module> module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
    tfsc::tosa::Graph graph{module};

    // Generate graph with multiple operators
    for (const auto op : ops)
    {
        std::vector<tfsc::tosa::ResId> inputIds;
        for (size_t inputIdx = 0; inputIdx < op.inputs.size(); ++inputIdx)
        {
            switch (op.inputs[inputIdx].type)
            {
                case TosaInputType::Dynamic:
                    inputIds.push_back(graph.AddInput(op.inputs[inputIdx].GetTensor(), inputIdx));
                    break;
                case TosaInputType::GraphConstant:
                    inputIds.push_back(graph.AddGraphConstant(op.inputs[inputIdx].GetTensor()));
                    break;
                case TosaInputType::TensorConstant:
                    inputIds.push_back(graph.AddTensorConstant(op.inputs[inputIdx].GetAttribute()));
                    break;
            }
        }
        std::vector<tfsc::tosa::Tensor> outputTensors;
        for (const auto& output : op.outputs)
        {
            outputTensors.push_back(output.GetTensor());
        }
        graph.AddOperator(op.op, inputIds, outputTensors, op.attributes);
    }
    graph.FinalizeGraph();

    std::vector<std::string> declarations = opDeclarations;
    std::vector<std::string> generated = Spirv2tosa(module, defaultOpComparator);
    std::sort(declarations.begin(), declarations.end());
    std::sort(generated.begin(), generated.end());
    return declarations == generated;
}

bool Spirv2operatorsTest(const std::vector<TosaOperator>& ops,
                         std::vector<TosaOperator> expectedOps,
                         OperatorComparator comparator,
                         bool ignoreBindingId = false)
{
    std::shared_ptr<tfsc::spirv::Module> module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
    tfsc::tosa::Graph graph{module};

    // Generate graph with multiple operators
    for (const auto op : ops)
    {
        std::vector<tfsc::tosa::ResId> inputIds;
        for (const auto& currentOp : op.inputs)
        {
            switch (currentOp.type)
            {
                case TosaInputType::Dynamic:
                    inputIds.push_back(graph.AddInput(currentOp.GetTensor(), currentOp.bindingId));
                    break;
                case TosaInputType::GraphConstant:
                    inputIds.push_back(graph.AddGraphConstant(currentOp.GetTensor()));
                    break;
                case TosaInputType::TensorConstant:
                    inputIds.push_back(graph.AddTensorConstant(currentOp.GetAttribute()));
                    break;
            }
        }
        std::vector<tfsc::tosa::Tensor> outputTensors;
        for (const auto& output : op.outputs)
        {
            outputTensors.push_back(output.GetTensor());
        }
        graph.AddOperator(op.op, inputIds, outputTensors, op.attributes);
    }

    auto generatedOps = Spirv2operators(module, comparator);

    if (expectedOps.size() != generatedOps.size())
    {
        return false;
    }
    // Check that the generated vector contains the same elements as the input vector, allowing for different order
    std::sort(expectedOps.begin(), expectedOps.end(), comparator);
    std::sort(generatedOps.begin(), generatedOps.end(), comparator);
    auto generatedIt = generatedOps.begin();
    for (auto expectedIt = expectedOps.begin(); expectedIt != expectedOps.end(); ++expectedIt, ++generatedIt)
    {
        if (ignoreBindingId)
        {
            for (auto& input : generatedIt->inputs)
            {
                input.bindingId = 0;
            }
            for (auto& input : expectedIt->inputs)
            {
                input.bindingId = 0;
            }
        }
        if (!CompareTosaOperatorData(*generatedIt, *expectedIt))
        {
            return false;
        }
    }
    return true;
}

template <typename T> bool CompareVectors(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    std::vector<T> comparator;
    for (const T& element : lhs)
    {
        comparator.push_back(element);
    }
    for (const T& element : rhs)
    {
        for (auto iterator = comparator.begin(); iterator != comparator.end(); ++iterator)
        {
            if (*iterator == element)
            {
                comparator.erase(iterator);
                break;
            }
        }
    }

    return comparator.empty();
}

TEST(Spirv2TosaTest, Spirv2OperatorsBasicTest)
{
    std::vector<TosaOperator> ops;
    uint32_t bindingId = 0;
    for (size_t i = 0; i < 9; ++i)
    {
        TosaOperator op;
        op.op = static_cast<tfsc::tosa::OperatorEnum>(static_cast<size_t>(tfsc::tosa::OperatorEnum::BitwiseAnd) + i);
        op.inputs.emplace_back(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}},
                               TosaInputType::Dynamic,
                               0,
                               0,
                               bindingId++);
        op.inputs.emplace_back(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}},
                               TosaInputType::Dynamic,
                               0,
                               0,
                               bindingId++);
        op.outputs.emplace_back(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}});
        ops.push_back(op);
    }
    EXPECT_TRUE(Spirv2operatorsTest(ops, ops, defaultOpComparator));
}

TEST(Spirv2TosaTest, Spirv2operatorsGraphTest)
{
    // Creating graph
    const auto& module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
    tfsc::tosa::Graph graph{module};

    const auto& input1 = graph.AddInput(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}}, 0);
    const auto& input2 = graph.AddGraphConstant(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}});
    const auto& shift = graph.AddTensorConstant(tfsc::tosa::Attribute{{1}, tfsc::tosa::DataType::uint32_t, {1}});

    const auto& output =
        graph.AddMulOperator(input1, input2, shift, tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}});
    graph.AddOutput(output, 0);

    graph.FinalizeGraph();

    const auto generatedOp = Spirv2operators(module, defaultOpComparator);

    EXPECT_EQ(generatedOp.size(), 1);

    TosaOperator op;
    op.op = tfsc::tosa::OperatorEnum::Mul;
    op.inputs.emplace_back(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{2}});
    op.inputs.emplace_back(tfsc::tosa::Attribute{std::initializer_list<uint32_t>{},
                                                 tfsc::tosa::DataType::uint32_t,
                                                 tfsc::tosa::Tensor::TensorShape{2}},
                           TosaInputType::GraphConstant);
    op.inputs.emplace_back(tfsc::tosa::Attribute{std::initializer_list<uint32_t>{1},
                                                 tfsc::tosa::DataType::uint32_t,
                                                 tfsc::tosa::Tensor::TensorShape{1}},
                           TosaInputType::TensorConstant);
    op.outputs.emplace_back(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{2}});

    EXPECT_TRUE(CompareTosaOperatorData(op, generatedOp[0]));
}

TEST(Spirv2TosaTest, Spirv2operatorsGraphInputOrderTest)
{
    // Creating graph
    const auto& module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
    tfsc::tosa::Graph graph{module};

    const auto& input1 = graph.AddGraphConstant(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}});
    const auto& input2 = graph.AddTensorConstant(tfsc::tosa::Attribute{{2, 3}, tfsc::tosa::DataType::uint32_t, {2}});
    const auto& shift = graph.AddInput(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {1}}, 0);

    const auto& output =
        graph.AddMulOperator(input1, input2, shift, tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}});
    graph.AddOutput(output, 0);

    graph.FinalizeGraph();

    const auto generatedOp = Spirv2operators(module, defaultOpComparator);

    EXPECT_EQ(generatedOp.size(), 1);

    const auto& module2 = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
    tfsc::tosa::Graph graph2{module2};

    std::vector<tfsc::tosa::ResId> inputIds;
    for (size_t idx = 0; idx < generatedOp[0].inputs.size(); ++idx)
    {
        const auto& in = generatedOp[0].inputs[idx];
        switch (in.type)
        {
            case TosaInputType::Dynamic:
            {
                const auto& inputId = graph2.AddInput(in.GetTensor(), in.bindingId);
                inputIds.push_back(inputId);
            }
            break;
            case TosaInputType::GraphConstant:
            {
                const auto& graphConstId = graph2.AddGraphConstant(in.GetTensor());
                inputIds.push_back(graphConstId);
            }
            break;
            case TosaInputType::TensorConstant:
            {
                const auto& tensorConstId = graph2.AddTensorConstant(generatedOp[0].inputs[idx].GetAttribute());
                inputIds.push_back(tensorConstId);
            }
            break;
        }
    }
    std::vector<tfsc::tosa::Tensor> outputTensors;
    for (const auto& output : generatedOp[0].outputs)
    {
        outputTensors.push_back(output.GetTensor());
    }
    const auto& output2 = graph2.AddOperator(generatedOp[0].op, inputIds, outputTensors, generatedOp[0].attributes);
    graph2.AddOutput(output2[0], 0);
    graph2.FinalizeGraph();

    std::vector<uint32_t> spirv = tfsc::WriteToBinary(module);
    std::vector<uint32_t> spirv2 = tfsc::WriteToBinary(module2);
    const auto actualText = testutils::DisassembleSPIRV(spirv, false);
    const auto actualText2 = testutils::DisassembleSPIRV(spirv2, false);
    const auto actualBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(actualText));
    const auto expectedBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(actualText2));
    EXPECT_EQ(actualBinary, expectedBinary);
}

TEST(Spirv2TosaTest, Spirv2OperatorsGraphMultiOutputTest)
{
    // Creating graph
    const auto& module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
    tfsc::tosa::Graph graph{module};

    // Adding FFT2D
    const auto& input1 = graph.AddInput(tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}}, 0);
    const auto& input2 = graph.AddInput(tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}}, 1);
    const auto& fft2dOutput = graph.AddFft2dOperator(input1,
                                                     input2,
                                                     tfsc::tosa::Attribute{{0}, tfsc::tosa::DataType::bool_t, {1}},
                                                     tfsc::tosa::Attribute{{0}, tfsc::tosa::DataType::bool_t, {1}},
                                                     tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}},
                                                     tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}});
    // Adding ADD connected to the two FFT2D outputs
    const auto& graphOutput = graph.AddAddOperator(fft2dOutput[0],
                                                   fft2dOutput[1],
                                                   tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}});
    graph.AddOutput(graphOutput, 0);
    graph.FinalizeGraph();

    // Getting and validating regenerated TOSA Operators
    const auto& generatedOps = Spirv2operators(module, defaultOpComparator);
    EXPECT_EQ(generatedOps.size(), 2);

    TosaOperator fft2dExpected = TosaOperator{
        tfsc::tosa::OperatorEnum::Fft2d,
        {{tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}}, TosaInputType::Dynamic, 0, 0, 0},
         {tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}}, TosaInputType::Dynamic, 0, 0, 1}},
        {tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}},
         tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}}},
        {tfsc::tosa::Attribute{{0}, tfsc::tosa::DataType::bool_t, {1}},
         tfsc::tosa::Attribute{{0}, tfsc::tosa::DataType::bool_t, {1}}}};
    EXPECT_TRUE(CompareTosaOperatorData(generatedOps[0], fft2dExpected));

    TosaOperator addExpected =
        TosaOperator{tfsc::tosa::OperatorEnum::Add,
                     {{tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}}, TosaInputType::Dynamic},
                      {tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}}, TosaInputType::Dynamic}},
                     {tfsc::tosa::Tensor{tfsc::tosa::DataType::float32_t, {2, 2, 2}}},
                     {}};
    EXPECT_TRUE(CompareTosaOperatorData(generatedOps[1], addExpected));
}

TEST(Spirv2TosaTest, Spirv2OperatorsComparatorTest)
{
    std::vector<TosaOperator> ops;
    std::vector<TosaOperator> expectedOps;
    for (size_t i = 0; i < 9; ++i)
    {
        TosaOperator op;
        op.op = static_cast<tfsc::tosa::OperatorEnum>(static_cast<size_t>(tfsc::tosa::OperatorEnum::BitwiseAnd) + i);
        op.inputs.emplace_back(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t,
                                                  tfsc::tosa::Tensor::TensorShape{1, static_cast<uint32_t>(i)}});
        op.inputs.emplace_back(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t,
                                                  tfsc::tosa::Tensor::TensorShape{1, static_cast<uint32_t>(i)}});
        op.outputs.emplace_back(tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t,
                                                   tfsc::tosa::Tensor::TensorShape{1, static_cast<uint32_t>(i)}});

        ops.push_back(op);
        ops.push_back(op);
        expectedOps.push_back(op);
    }

    EXPECT_TRUE(Spirv2operatorsTest(
        ops,
        expectedOps,
        [](const TosaOperator& lhs, const TosaOperator& rhs) { return lhs.op < rhs.op; },
        true));
}

TEST(Spirv2TosaTest, Spirv2TosaTest)
{
    std::vector<std::string> strings = {
        "const OperatorEnum op = OperatorEnum::Add;\n"
        "const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::uint32_t, {1}}, "
        "{std::initializer_list<uint32_t>{}, DataType::uint32_t, {1}}};\n"
        "const std::vector<Tensor> outputs {{DataType::uint32_t, {1}}};\n"
        "const std::vector<Attribute> attributes {};",
        "const OperatorEnum op = OperatorEnum::Sub;\n"
        "const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::uint32_t, {1}}, "
        "{std::initializer_list<uint32_t>{}, DataType::uint32_t, {1}}};\n"
        "const std::vector<Tensor> outputs {{DataType::uint32_t, {1}}};\n"
        "const std::vector<Attribute> attributes {};",
        "const OperatorEnum op = OperatorEnum::ArithmeticRightShift;\n"
        "const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, DataType::uint32_t, {1}}, "
        "{std::initializer_list<uint32_t>{}, DataType::uint32_t, {1}}};\n"
        "const std::vector<Tensor> outputs {{DataType::uint32_t, {1}}};\n"
        "const std::vector<Attribute> attributes {{std::initializer_list<uint32_t>{1}, DataType::uint32_t, {1}}};",
    };
    std::vector<TosaOperator> ops = {
        {tfsc::tosa::OperatorEnum::Add,
         {tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}},
          tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}}},
         {tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}}},
         {}},
        {tfsc::tosa::OperatorEnum::Sub,
         {tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}},
          {tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}},
           TosaInputType::GraphConstant}},
         {tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}}},
         {}},
        {tfsc::tosa::OperatorEnum::ArithmeticRightShift,
         {tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}},
          tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}}},
         {tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, tfsc::tosa::Tensor::TensorShape{1}}},
         {tfsc::tosa::Attribute{{1}, tfsc::tosa::DataType::uint32_t, {1}}}}};

    EXPECT_TRUE(Spirv2tosaTest(ops, strings));
}

TEST(Spirv2TosaTest, GetResIdNumberTest)
{
    uint32_t idNumber = 12345;

    tfsc::spirv::Instruction instr0{spv::Op::OpTypeInt, {tfsc::spirv::Operand{idNumber, tfsc::spirv::RES_ID}}};
    tfsc::tosa::ResId resId0{&instr0};
    EXPECT_EQ(GetResIdNumber(resId0), idNumber);

    tfsc::spirv::Instruction instr1{spv::Op::OpExtInst,
                                    {tfsc::spirv::Operand{0u, tfsc::spirv::INSTRUCTION_POINTER},
                                     tfsc::spirv::Operand{idNumber, tfsc::spirv::RES_ID}}};
    tfsc::tosa::ResId resId1{&instr1};
    EXPECT_EQ(GetResIdNumber(resId1), idNumber);

    tfsc::spirv::Instruction instr2{spv::Op::OpAny, {}};
    tfsc::tosa::ResId resId2{&instr2};

    EXPECT_THROW(GetResIdNumber(resId2), std::runtime_error);
}

TEST(Spirv2TosaTest, GetTosaSerializationOperatorBasicTest)
{
    TosaOperator op{tfsc::tosa::OperatorEnum::Add,
                    {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 1, 0},
                     {{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 2, 0}},
                    {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 3, 0}}};

    std::vector<std::unique_ptr<::tosa::TosaSerializationOperator>> operators;
    std::map<std::string, std::unique_ptr<::tosa::TosaSerializationTensor>> tensors;
    std::set<std::string> dynamicInputNames, dynamicOutputNames;

    GetTosaSerializationOperator(op, operators, tensors, dynamicInputNames, dynamicOutputNames);

    EXPECT_EQ(operators.size(), 1);
    EXPECT_EQ(tensors.size(), 3);

    // Checking generated operator
    EXPECT_EQ(operators[0]->GetOp(), ::tosa::Op_ADD);
    EXPECT_EQ(operators[0]->GetAttributeType(), ::tosa::Attribute_AddAttribute);
    EXPECT_NE(dynamic_cast<tosa::TosaAddAttribute*>(operators[0]->GetAttribute()), nullptr);
    EXPECT_TRUE(
        CompareVectors(operators[0]->GetInputTensorNames(), {dynamicInputNames.begin(), dynamicInputNames.end()}));
    EXPECT_TRUE(
        CompareVectors(operators[0]->GetOutputTensorNames(), {dynamicOutputNames.begin(), dynamicOutputNames.end()}));

    // Checking generated tensors
    for (const auto& tensor : tensors)
    {
        EXPECT_EQ(tensor.second->GetName(), tensor.first);
        EXPECT_EQ(tensor.second->GetShape(), std::vector<int32_t>({1, 2, 3, 4}));
        EXPECT_EQ(tensor.second->GetDtype(), ::tosa::DType::DType_INT32);
        EXPECT_EQ(tensor.second->GetVariable(), false);
        const auto& data = tensor.second->GetData();
        for (uint8_t byte : data)
        {
            EXPECT_EQ(byte, 0);
        }
        EXPECT_EQ(tensor.second->GetIsUnranked(), false);
        EXPECT_EQ(tensor.second->GetVariableName(), "");
    }
}

TEST(Spirv2TosaTest, GetTosaSerializationOperatorMultipleTest)
{
    TosaOperator op1{tfsc::tosa::OperatorEnum::Add,
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 1, 0},
                      {{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 2, 0}},
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 4, 0}}};
    TosaOperator op2{tfsc::tosa::OperatorEnum::Sub,
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 2, 0},
                      {{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 3, 0}},
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 5, 0}}};
    TosaOperator op3{tfsc::tosa::OperatorEnum::Concat,
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 4, 0},
                      {{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 5, 0}},
                     {{{tfsc::tosa::DataType::uint32_t, {2, 2, 3, 4}}, 6, 0}},
                     {tfsc::tosa::Attribute{{0}, tfsc::tosa::DataType::int32_t, {1}}}};
    TosaOperator op4{tfsc::tosa::OperatorEnum::Abs,
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 5, 0}},
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 7, 0}}};

    std::vector<std::unique_ptr<::tosa::TosaSerializationOperator>> operators;
    std::map<std::string, std::unique_ptr<::tosa::TosaSerializationTensor>> tensors;
    std::set<std::string> dynamicInputNames, dynamicOutputNames;

    GetTosaSerializationOperator(op1, operators, tensors, dynamicInputNames, dynamicOutputNames);
    GetTosaSerializationOperator(op2, operators, tensors, dynamicInputNames, dynamicOutputNames);
    GetTosaSerializationOperator(op3, operators, tensors, dynamicInputNames, dynamicOutputNames);
    GetTosaSerializationOperator(op4, operators, tensors, dynamicInputNames, dynamicOutputNames);

    EXPECT_EQ(operators.size(), 4);
    EXPECT_EQ(tensors.size(), 7);

    // Checking generated operators
    EXPECT_EQ(operators[0]->GetOp(), ::tosa::Op_ADD);
    EXPECT_EQ(operators[1]->GetOp(), ::tosa::Op_SUB);
    EXPECT_EQ(operators[2]->GetOp(), ::tosa::Op_CONCAT);
    EXPECT_EQ(operators[3]->GetOp(), ::tosa::Op_ABS);

    EXPECT_EQ(operators[0]->GetAttributeType(), ::tosa::Attribute_AddAttribute);
    EXPECT_EQ(operators[1]->GetAttributeType(), ::tosa::Attribute_SubAttribute);
    EXPECT_EQ(operators[2]->GetAttributeType(), ::tosa::Attribute_ConcatAttribute);
    EXPECT_EQ(operators[3]->GetAttributeType(), ::tosa::Attribute_AbsAttribute);

    EXPECT_NE(dynamic_cast<tosa::TosaAddAttribute*>(operators[0]->GetAttribute()), nullptr);
    EXPECT_NE(dynamic_cast<tosa::TosaSubAttribute*>(operators[1]->GetAttribute()), nullptr);
    EXPECT_NE(dynamic_cast<tosa::TosaConcatAttribute*>(operators[2]->GetAttribute()), nullptr);
    EXPECT_NE(dynamic_cast<tosa::TosaAbsAttribute*>(operators[3]->GetAttribute()), nullptr);

    EXPECT_EQ(dynamic_cast<tosa::TosaConcatAttribute*>(operators[2]->GetAttribute())->axis(), 0);

    EXPECT_TRUE(CompareVectors(operators[0]->GetInputTensorNames(), {"tensor_1_0", "tensor_2_0"}));
    EXPECT_TRUE(CompareVectors(operators[0]->GetOutputTensorNames(), {"tensor_4_0"}));
    EXPECT_TRUE(CompareVectors(operators[1]->GetInputTensorNames(), {"tensor_2_0", "tensor_3_0"}));
    EXPECT_TRUE(CompareVectors(operators[1]->GetOutputTensorNames(), {"tensor_5_0"}));
    EXPECT_TRUE(CompareVectors(operators[2]->GetInputTensorNames(), {"tensor_4_0", "tensor_5_0"}));
    EXPECT_TRUE(CompareVectors(operators[2]->GetOutputTensorNames(), {"tensor_6_0"}));
    EXPECT_TRUE(CompareVectors(operators[3]->GetInputTensorNames(), {"tensor_5_0"}));
    EXPECT_TRUE(CompareVectors(operators[3]->GetOutputTensorNames(), {"tensor_7_0"}));

    std::vector<std::vector<int32_t>> expectedShapes{{1, 2, 3, 4},
                                                     {1, 2, 3, 4},
                                                     {1, 2, 3, 4},
                                                     {1, 2, 3, 4},
                                                     {1, 2, 3, 4},
                                                     {2, 2, 3, 4},
                                                     {1, 2, 3, 4}};
    EXPECT_EQ(expectedShapes.size(), tensors.size());
    for (size_t i = 0; i < expectedShapes.size(); ++i)
    {
        std::string tensorName = "tensor_" + std::to_string(i + 1) + "_0";
        EXPECT_TRUE(tensors.find(tensorName) != tensors.end());
        EXPECT_EQ(tensors[tensorName]->GetName(), tensorName);
        EXPECT_EQ(tensors[tensorName]->GetShape(), expectedShapes[i]);
        EXPECT_EQ(tensors[tensorName]->GetDtype(), ::tosa::DType::DType_INT32);
        EXPECT_EQ(tensors[tensorName]->GetVariable(), false);
        const auto& data = tensors[tensorName]->GetData();
        for (uint8_t byte : data)
        {
            EXPECT_EQ(byte, 0);
        }
        EXPECT_EQ(tensors[tensorName]->GetIsUnranked(), false);
        EXPECT_EQ(tensors[tensorName]->GetVariableName(), "");
    }
}

TEST(Spirv2TosaTest, GetTosaSerializationOperatorTensorNameTest)
{
    TosaOperator op1{tfsc::tosa::OperatorEnum::Add,
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 1, 0},
                      {{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 2, 0}},
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 3, 0}}};
    TosaOperator op2{tfsc::tosa::OperatorEnum::Sub,
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 3, 0},
                      {{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, TosaInputType::Dynamic, 4, 0}},
                     {{{tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 5, 0}}};

    std::vector<std::unique_ptr<::tosa::TosaSerializationOperator>> operators;
    std::map<std::string, std::unique_ptr<::tosa::TosaSerializationTensor>> tensors;
    std::set<std::string> dynamicInputNames, dynamicOutputNames;

    std::map<uint32_t, std::string> tensorNameMap;
    tensorNameMap[1] = "input0";
    tensorNameMap[2] = "input1";
    tensorNameMap[4] = "input2";
    tensorNameMap[5] = "output0";

    GetTosaSerializationOperator(op1,
                                 operators,
                                 tensors,
                                 dynamicInputNames,
                                 dynamicOutputNames,
                                 {},
                                 tensorNameMap,
                                 "TENSOR:");
    GetTosaSerializationOperator(op2,
                                 operators,
                                 tensors,
                                 dynamicInputNames,
                                 dynamicOutputNames,
                                 {},
                                 tensorNameMap,
                                 "TENSOR:");

    EXPECT_EQ(operators.size(), 2);
    EXPECT_EQ(tensors.size(), 5);

    // Checking generated operators
    EXPECT_EQ(operators[0]->GetOp(), ::tosa::Op_ADD);
    EXPECT_EQ(operators[1]->GetOp(), ::tosa::Op_SUB);
    EXPECT_EQ(operators[0]->GetAttributeType(), ::tosa::Attribute_AddAttribute);
    EXPECT_EQ(operators[1]->GetAttributeType(), ::tosa::Attribute_SubAttribute);
    EXPECT_NE(dynamic_cast<tosa::TosaAddAttribute*>(operators[0]->GetAttribute()), nullptr);
    EXPECT_NE(dynamic_cast<tosa::TosaSubAttribute*>(operators[1]->GetAttribute()), nullptr);

    // Checking tensor names
    EXPECT_TRUE(CompareVectors(operators[0]->GetInputTensorNames(), {"input0", "input1"}));
    EXPECT_TRUE(CompareVectors(operators[0]->GetOutputTensorNames(), {"TENSOR:3_0"}));
    EXPECT_TRUE(CompareVectors(operators[1]->GetInputTensorNames(), {"input2", "TENSOR:3_0"}));
    EXPECT_TRUE(CompareVectors(operators[1]->GetOutputTensorNames(), {"output0"}));
}

TEST(Spirv2TosaTest, GetTosaSerializationOperatorConstantTest)
{
    std::vector<uint32_t> tensorConstData{1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6,
                                          7, 8, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4,
                                          5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8};
    TosaOperator op1{tfsc::tosa::OperatorEnum::Add,
                     {{{tfsc::tosa::DataType::uint32_t, {1, 1, 8, 8}}, TosaInputType::Dynamic, 1, 0},
                      {tfsc::tosa::Attribute{tensorConstData, tfsc::tosa::DataType::uint32_t, {1, 1, 8, 8}},
                       TosaInputType::TensorConstant,
                       2,
                       0}},
                     {{{tfsc::tosa::DataType::uint32_t, {1, 1, 8, 8}}, 3, 0}}};
    TosaOperator op2{tfsc::tosa::OperatorEnum::Conv2d,
                     {{{tfsc::tosa::DataType::uint32_t, {1, 1, 8, 8}}, TosaInputType::Dynamic, 3, 0},
                      {{tfsc::tosa::DataType::uint32_t, {1, 1, 4, 4}}, TosaInputType::GraphConstant, 4, 0, 0},
                      {{tfsc::tosa::DataType::uint32_t, {16}}, TosaInputType::GraphConstant, 5, 0, 1},
                      {{{0}, tfsc::tosa::DataType::uint32_t, {1}}, TosaInputType::TensorConstant, 6, 0},
                      {{{0}, tfsc::tosa::DataType::uint32_t, {1}}, TosaInputType::TensorConstant, 7, 0}},
                     {{{tfsc::tosa::DataType::uint32_t, {1, 1, 8, 8}}, 8, 0}},
                     {tfsc::tosa::Attribute{{1, 1, 1, 1}},
                      tfsc::tosa::Attribute{{8, 8}},
                      tfsc::tosa::Attribute{{0, 0}},
                      tfsc::tosa::Attribute{{1}},
                      tfsc::tosa::Attribute{{0}, tfsc::tosa::DataType::bool_t}}};

    std::vector<std::vector<uint32_t>> graphConstants;
    graphConstants.push_back({1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4});
    graphConstants.push_back({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});

    std::vector<std::unique_ptr<::tosa::TosaSerializationOperator>> operators;
    std::map<std::string, std::unique_ptr<::tosa::TosaSerializationTensor>> tensors;
    std::set<std::string> dynamicInputNames, dynamicOutputNames;

    GetTosaSerializationOperator(op1, operators, tensors, dynamicInputNames, dynamicOutputNames, graphConstants);
    GetTosaSerializationOperator(op2, operators, tensors, dynamicInputNames, dynamicOutputNames, graphConstants);

    EXPECT_EQ(operators.size(), 7);
    EXPECT_EQ(tensors.size(), 8);

    EXPECT_EQ(operators[0]->GetOp(), ::tosa::Op_CONST);
    EXPECT_EQ(operators[1]->GetOp(), ::tosa::Op_ADD);
    EXPECT_EQ(operators[2]->GetOp(), ::tosa::Op_CONST);
    EXPECT_EQ(operators[3]->GetOp(), ::tosa::Op_CONST);
    EXPECT_EQ(operators[4]->GetOp(), ::tosa::Op_CONST);
    EXPECT_EQ(operators[5]->GetOp(), ::tosa::Op_CONST);
    EXPECT_EQ(operators[6]->GetOp(), ::tosa::Op_CONV2D);

    // Validating constant data
    for (const auto& data : {std::pair{"tensor_2_0", tensorConstData},
                             std::pair{"tensor_4_0", graphConstants[0]},
                             {"tensor_5_0", graphConstants[1]},
                             {"tensor_6_0", {0}},
                             {"tensor_7_0", {0}}})
    {
        std::vector<int32_t> constDataI32{data.second.begin(), data.second.end()};
        std::vector<uint8_t> constDataU8;
        ::tosa::TosaSerializationHandler::ConvertI32toU8(constDataI32, constDataU8);
        EXPECT_TRUE(CompareVectors(tensors[data.first]->GetData(), constDataU8));
    }
}

TEST(Spirv2TosaTest, GetTosaSerializationHandlerBasicTest)
{
    // Setting up spirv::Graph
    auto module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
    tfsc::tosa::Graph graph{module};

    const auto& input1Id = graph.AddInput({tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 0);
    const auto& input2Id = graph.AddInput({tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 1);
    const auto& input3Id = graph.AddInput({tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}}, 2);

    const auto& addOutputId = graph.AddAddOperator(input1Id, input2Id, {tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}});

    const auto& subOutputId =
        graph.AddSubOperator(addOutputId, input3Id, {tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}});

    const auto& absOutputId = graph.AddAbsOperator(subOutputId, {tfsc::tosa::DataType::uint32_t, {1, 2, 3, 4}});

    graph.AddOutput(absOutputId, 0);
    graph.FinalizeGraph();

    // Converting graph to TOSA Serialization structure
    const auto handler = tosaregen::GetTosaSerializationHandler(module);

    // Verifying the correctness of the generated handler using the parser and comparator
    tfsc::parsers::TosaSerializationParser parser{handler->GetMainRegion()->GetBlockByName("main")};
    // Checking that the SPIR-V generated is valid
    const auto module2 = parser.GenerateSPIRVModule("main");
    std::vector<uint32_t> spirv = tfsc::WriteToBinary(module2);
    EXPECT_NE(testutils::DisassembleSPIRV(spirv, true), "");
    std::vector<uint32_t> spirv2 = tfsc::WriteToBinary(module);
    const auto text1 = testutils::DisassembleSPIRV(spirv2, false);
    const auto actualBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text1));
    const auto text2 = testutils::DisassembleSPIRV(actualBinary, false);
    const auto expectedBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text2));
    EXPECT_EQ(actualBinary, expectedBinary);
}

TEST(Spirv2TosaTest, GetTosaSerializationHandlerTensorNameTest)
{
    // Test graph setup
    auto module = tfsc::CreateModule(tfsc::TOSAVersion::v1_0);
    tfsc::tosa::Graph graph{module};

    const auto& literal2 = graph.AddTensorConstant({{2.0f, 2.0f}, tfsc::tosa::DataType::float32_t, {2}});
    const auto& literal4 = graph.AddTensorConstant({{4.0f, 2.0f}, tfsc::tosa::DataType::float32_t, {2}});
    const auto& literalI0 = graph.AddTensorConstant({{0}, tfsc::tosa::DataType::int8_t, {1}});
    const auto& literalF0 = graph.AddTensorConstant({{0.0f}, tfsc::tosa::DataType::float32_t, {1}});

    const auto& in1 = graph.AddInput({tfsc::tosa::DataType::float32_t, {2}}, 1);
    const auto& in2 = graph.AddInput({tfsc::tosa::DataType::float32_t, {2}}, 2);
    const auto& in3 = graph.AddInput({tfsc::tosa::DataType::float32_t, {2}}, 3);

    const auto& mul1Op = graph.AddMulOperator(in1, in3, literalI0, {tfsc::tosa::DataType::float32_t, {2}});

    const auto& powOp = graph.AddPowOperator(in2, literal2, {tfsc::tosa::DataType::float32_t, {2}});
    const auto& mul2Op = graph.AddMulOperator(mul1Op, literal4, literalI0, {tfsc::tosa::DataType::float32_t, {2}});

    const auto& negativeIn2 =
        graph.AddNegateOperator(in2, literalF0, literalF0, {tfsc::tosa::DataType::float32_t, {2}});
    const auto& doubleIn1 = graph.AddMulOperator(in1, literal2, literalI0, {tfsc::tosa::DataType::float32_t, {2}});
    const auto& inverseIn1 = graph.AddReciprocalOperator(doubleIn1, {tfsc::tosa::DataType::float32_t, {2}});

    const auto& delta = graph.AddSubOperator(powOp, mul2Op, {tfsc::tosa::DataType::float32_t, {2}});
    graph.AddOutput(delta, 0);
    const auto& rsqrtOp = graph.AddRsqrtOperator(delta, {tfsc::tosa::DataType::float32_t, {2}});

    const auto& subRsqrt = graph.AddSubOperator(negativeIn2, rsqrtOp, {tfsc::tosa::DataType::float32_t, {2}});
    const auto& addRsqrt = graph.AddAddOperator(negativeIn2, rsqrtOp, {tfsc::tosa::DataType::float32_t, {2}});

    const auto& result2 = graph.AddMulOperator(subRsqrt, inverseIn1, literalI0, {tfsc::tosa::DataType::float32_t, {2}});
    const auto& result1 = graph.AddMulOperator(addRsqrt, inverseIn1, literalI0, {tfsc::tosa::DataType::float32_t, {2}});

    graph.AddOutput(result1, 1);
    graph.AddOutput(result2, 2);

    graph.FinalizeGraph();

    // Setting up tensor name map to validate names
    std::map<uint32_t, std::string> tensorNameMap;
    tensorNameMap[GetResIdNumber(in1)] = "Input_A";
    tensorNameMap[GetResIdNumber(in2)] = "Input_B";
    tensorNameMap[GetResIdNumber(in3)] = "Input_C";
    tensorNameMap[GetResIdNumber(delta)] = "Delta";
    tensorNameMap[GetResIdNumber(result1)] = "Output_1";
    tensorNameMap[GetResIdNumber(result2)] = "Output_2";

    // Converting graph to TOSA Serialization structure
    const auto handler = tosaregen::GetTosaSerializationHandler(module, {}, tensorNameMap);
    handler->SaveFileTosaFlatbuffer("tosa_demo.tosa");

    // Verifying the correctness of the generated handler using the parser and comparator
    tfsc::parsers::TosaSerializationParser parser{handler->GetMainRegion()->GetBlockByName("main")};

    // Checking that the SPIR-V generated is valid
    const auto module2 = parser.GenerateSPIRVModule("main");
    const auto spirv = tfsc::WriteToBinary(module2);

    const auto text1 = testutils::DisassembleSPIRV(spirv, false);

#ifndef _WIN32
    const auto text2        = spirvmodels::GetTosaSerializationHandlerTensorNameTestGolden;
    const auto actualText   = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text1));
    const auto expectedText = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text2));
    EXPECT_EQ(actualText, expectedText);
#else
    EXPECT_FALSE(text1.empty());
#endif

}

TEST(Spirv2TosaTest, GetOperatorNameTest)
{
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::ArgMax), "ArgMax");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::AvgPool2d), "AvgPool2d");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Conv2d), "Conv2d");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Conv3d), "Conv3d");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::DepthwiseConv2d), "DepthwiseConv2d");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Fft2d), "Fft2d");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Matmul), "Matmul");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::MaxPool2d), "MaxPool2d");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Rfft2d), "Rfft2d");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::TransposeConv2d), "TransposeConv2d");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Clamp), "Clamp");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Erf), "Erf");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Sigmoid), "Sigmoid");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Tanh), "Tanh");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Add), "Add");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::ArithmeticRightShift), "ArithmeticRightShift");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::BitwiseAnd), "BitwiseAnd");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::BitwiseOr), "BitwiseOr");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::BitwiseXor), "BitwiseXor");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::IntDiv), "IntDiv");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::LogicalAnd), "LogicalAnd");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::LogicalLeftShift), "LogicalLeftShift");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::LogicalRightShift), "LogicalRightShift");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::LogicalOr), "LogicalOr");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::LogicalXor), "LogicalXor");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Maximum), "Maximum");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Minimum), "Minimum");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Mul), "Mul");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Pow), "Pow");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Sub), "Sub");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Table), "Table");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Abs), "Abs");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::BitwiseNot), "BitwiseNot");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Ceil), "Ceil");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Clz), "Clz");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Cos), "Cos");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Exp), "Exp");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Floor), "Floor");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Log), "Log");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::LogicalNot), "LogicalNot");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Negate), "Negate");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Reciprocal), "Reciprocal");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Rsqrt), "Rsqrt");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Sin), "Sin");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Select), "Select");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Equal), "Equal");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Greater), "Greater");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::GreaterEqual), "GreaterEqual");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::ReduceAll), "ReduceAll");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::ReduceAny), "ReduceAny");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::ReduceMax), "ReduceMax");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::ReduceMin), "ReduceMin");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::ReduceProduct), "ReduceProduct");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::ReduceSum), "ReduceSum");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Concat), "Concat");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Pad), "Pad");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Reshape), "Reshape");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Reverse), "Reverse");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Slice), "Slice");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Tile), "Tile");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Transpose), "Transpose");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Gather), "Gather");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Scatter), "Scatter");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Resize), "Resize");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Cast), "Cast");
    EXPECT_EQ(tfsc::tosa::GetOperatorName(tfsc::tosa::OperatorEnum::Rescale), "Rescale");
}
