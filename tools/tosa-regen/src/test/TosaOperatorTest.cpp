//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <OperatorEnum.hpp>
#include <Tensor.hpp>
#include <TosaOperator.hpp>

#include <gtest/gtest.h>

using namespace tosaregen;

TEST(Spirv2TosaTest, TosaOperatorCompareTest)
{
    TosaOperator op1{
        tfsc::tosa::OperatorEnum::Equal,
        {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}, {{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
        {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
        {}};
    TosaOperator op2{
        tfsc::tosa::OperatorEnum::Equal,
        {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}, {{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
        {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
        {}};

    EXPECT_TRUE(op1 == op2);
    EXPECT_FALSE(op1 != op2);

    op2.op = tfsc::tosa::OperatorEnum::GreaterEqual;
    EXPECT_FALSE(op1 == op2);
    EXPECT_TRUE(op1 != op2);

    op2.op = tfsc::tosa::OperatorEnum::Equal;
    op2.inputs[0].type = TosaInputType::GraphConstant;
    EXPECT_FALSE(op1 == op2);
    EXPECT_TRUE(op1 != op2);

    op2.inputs[0].type = TosaInputType::Dynamic;
    op2.inputs[0].id = 1;
    EXPECT_FALSE(op1 == op2);
    EXPECT_TRUE(op1 != op2);

    op2.inputs[0].id = 0;
    op2.inputs[0].idx = 1;
    EXPECT_FALSE(op1 == op2);
    EXPECT_TRUE(op1 != op2);

    op2.inputs[0].idx = 0;
    op2.inputs[0].bindingId = 1;
    EXPECT_FALSE(op1 == op2);
    EXPECT_TRUE(op1 != op2);

    op2.inputs[0].bindingId = 0;
    op2.outputs[0].id = 1;
    EXPECT_FALSE(op1 == op2);
    EXPECT_TRUE(op1 != op2);

    op2.outputs[0].id = 0;
    op2.outputs[0].idx = 1;
    EXPECT_FALSE(op1 == op2);
    EXPECT_TRUE(op1 != op2);

    op2.outputs[0].idx = 0;
    EXPECT_TRUE(op1 == op2);
    EXPECT_FALSE(op1 != op2);

    op2.attributes.push_back(tfsc::tosa::Attribute{{1, 2}, tfsc::tosa::DataType::uint32_t, {2}});
    EXPECT_FALSE(op1 == op2);
    EXPECT_TRUE(op1 != op2);
}

TEST(Spirv2TosaTest, TosaInputCompareTest)
{
    // Attribute-based TensorConstant inputs
    TosaInput in1{tfsc::tosa::Attribute{{1, 1}, tfsc::tosa::DataType::uint32_t, {2}}};
    TosaInput in2{tfsc::tosa::Attribute{{1, 1}, tfsc::tosa::DataType::uint32_t, {2}}};
    TosaInput in3{tfsc::tosa::Attribute{{2, 1}, tfsc::tosa::DataType::uint32_t, {2}}};
    TosaInput in4{tfsc::tosa::Attribute{{1, 1, 1}, tfsc::tosa::DataType::uint32_t, {3}}};
    TosaInput in5{tfsc::tosa::Attribute{{1}, tfsc::tosa::DataType::uint32_t, {1}}};

    EXPECT_TRUE(in1 == in2);
    EXPECT_TRUE(in1 <= in2);
    EXPECT_TRUE(in1 >= in2);
    EXPECT_FALSE(in1 < in2);
    EXPECT_FALSE(in1 > in2);
    EXPECT_FALSE(in1 != in2);
    EXPECT_TRUE(in1 != in3);
    EXPECT_FALSE(in1 == in3);
    EXPECT_TRUE(in2 < in3);
    EXPECT_TRUE(in2 <= in3);
    EXPECT_FALSE(in2 > in3);
    EXPECT_FALSE(in2 >= in3);
    EXPECT_TRUE(in5 < in2);
    EXPECT_TRUE(in5 <= in2);
    EXPECT_FALSE(in5 > in2);
    EXPECT_FALSE(in5 >= in2);
    EXPECT_TRUE(in3 < in4);
    EXPECT_TRUE(in3 <= in4);
    EXPECT_FALSE(in3 > in4);
    EXPECT_FALSE(in3 >= in4);

    // Tensor-based Dynamic/GraphConstant inputs
    TosaInput in6{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}}};
    TosaInput in7{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {3}}};
    TosaInput in8{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2, 2}}};
    TosaInput in9{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}}, TosaInputType::GraphConstant};
    TosaInput in10{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}},
                   TosaInputType::GraphConstant,
                   1,
                   0,
                   0};
    TosaInput in11{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {2}},
                   TosaInputType::Dynamic,
                   0,
                   0,
                   1};

    EXPECT_TRUE(in6 == in6);
    EXPECT_TRUE(in6 < in7);
    EXPECT_TRUE(in6 <= in7);
    EXPECT_FALSE(in6 > in7);
    EXPECT_FALSE(in6 >= in7);
    EXPECT_TRUE(in7 < in8);
    EXPECT_TRUE(in7 <= in8);
    EXPECT_FALSE(in7 > in8);
    EXPECT_FALSE(in7 >= in8);
    EXPECT_TRUE(in6 < in9);
    EXPECT_TRUE(in6 <= in9);
    EXPECT_FALSE(in6 > in9);
    EXPECT_FALSE(in6 >= in9);
    EXPECT_TRUE(in9 < in10);
    EXPECT_TRUE(in9 <= in10);
    EXPECT_FALSE(in9 > in10);
    EXPECT_FALSE(in9 >= in10);
    EXPECT_TRUE(in6 < in11);
    EXPECT_TRUE(in6 <= in11);
    EXPECT_FALSE(in6 > in11);
    EXPECT_FALSE(in6 >= in11);
}

TEST(Spirv2TosaTest, TosaOutputCompareTest)
{
    TosaOutput out1{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {1, 1}}};
    TosaOutput out2{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {1, 1}}};
    TosaOutput out3{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {1, 1}}, 1, 0};
    TosaOutput out4{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {1, 1}}, 1, 1};
    TosaOutput out5{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {1, 1, 1}}};
    TosaOutput out6{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {1}}};
    TosaOutput out7{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint32_t, {1, 2}}};
    TosaOutput out8{tfsc::tosa::Tensor{tfsc::tosa::DataType::uint16_t, {1, 1}}};

    EXPECT_TRUE(out1 == out2);
    EXPECT_FALSE(out1 != out2);
    EXPECT_TRUE(out1 <= out2);
    EXPECT_TRUE(out1 >= out2);
    EXPECT_FALSE(out1 < out2);
    EXPECT_FALSE(out1 > out2);

    EXPECT_TRUE(out2 < out3);
    EXPECT_TRUE(out2 <= out3);
    EXPECT_FALSE(out2 > out3);
    EXPECT_FALSE(out2 >= out3);

    EXPECT_TRUE(out3 < out4);
    EXPECT_TRUE(out3 <= out4);
    EXPECT_FALSE(out3 > out4);
    EXPECT_FALSE(out3 >= out4);

    EXPECT_TRUE(out4 < out5);
    EXPECT_TRUE(out4 <= out5);
    EXPECT_FALSE(out4 > out5);
    EXPECT_FALSE(out4 >= out5);

    EXPECT_TRUE(out2 < out5);
    EXPECT_TRUE(out2 <= out5);
    EXPECT_FALSE(out2 > out5);
    EXPECT_FALSE(out2 >= out5);

    EXPECT_TRUE(out6 < out2);
    EXPECT_TRUE(out6 <= out2);
    EXPECT_FALSE(out6 > out2);
    EXPECT_FALSE(out6 >= out2);

    EXPECT_TRUE(out2 < out7);
    EXPECT_TRUE(out2 <= out7);
    EXPECT_FALSE(out2 > out7);
    EXPECT_FALSE(out2 >= out7);

    EXPECT_TRUE(out8 < out2);
    EXPECT_TRUE(out8 <= out2);
    EXPECT_FALSE(out8 > out2);
    EXPECT_FALSE(out8 >= out2);
}

TEST(Spirv2TosaTest, DefaultOpComparatorTest)
{
    TosaOperator op1{
        tfsc::tosa::OperatorEnum::Equal,
        {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}, {{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
        {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
        {}};
    TosaOperator op2{
        tfsc::tosa::OperatorEnum::Add,
        {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}, {{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
        {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
        {}};
    TosaOperator op3{tfsc::tosa::OperatorEnum::Add,
                     {{{tfsc::tosa::DataType::uint32_t, {2, 2}}, TosaInputType::GraphConstant},
                      {{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
                     {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
                     {}};

    // Verifying that defaultOpComparator has the desired properties of std::less
    // Irreflexive
    EXPECT_FALSE(defaultOpComparator(op1, op1));
    EXPECT_FALSE(defaultOpComparator(op2, op2));
    EXPECT_FALSE(defaultOpComparator(op3, op3));
    // Asymmetric
    EXPECT_TRUE(defaultOpComparator(op2, op1));
    EXPECT_FALSE(defaultOpComparator(op1, op2));
    EXPECT_TRUE(defaultOpComparator(op2, op3));
    EXPECT_FALSE(defaultOpComparator(op3, op2));
    EXPECT_TRUE(defaultOpComparator(op3, op1));
    EXPECT_FALSE(defaultOpComparator(op1, op3));
    // Transitive
    EXPECT_TRUE(defaultOpComparator(op2, op3));
    EXPECT_TRUE(defaultOpComparator(op3, op1));
    EXPECT_TRUE(defaultOpComparator(op2, op1));
}

TEST(Spirv2TosaTest, OperatorToStringTest)
{
    TosaOperator op{tfsc::tosa::OperatorEnum::Equal,
                    {{{tfsc::tosa::DataType::uint32_t, {2, 2}}},
                     {{tfsc::tosa::DataType::uint32_t, {2, 2}}, TosaInputType::GraphConstant}},
                    {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
                    {}};
    std::string expected =
        "const OperatorEnum op = OperatorEnum::Equal;\n"
        "const std::vector<Attribute> inputs {{std::initializer_list<uint32_t>{}, "
        "DataType::uint32_t, {2, 2}}, {std::initializer_list<uint32_t>{}, DataType::uint32_t, {2, 2}}};\n"
        "const std::vector<Tensor> outputs {{DataType::uint32_t, {2, 2}}};\n"
        "const std::vector<Attribute> attributes {};";
    EXPECT_EQ(OperatorToString(op), expected);
}

TEST(Spirv2TosaTest, OperatorToGraphDefinitionTest)
{
    TosaOperator op{tfsc::tosa::OperatorEnum::Equal,
                    {{{tfsc::tosa::DataType::uint32_t, {2, 2}}},
                     {{tfsc::tosa::DataType::uint32_t, {2, 2}}, TosaInputType::GraphConstant}},
                    {{{tfsc::tosa::DataType::uint32_t, {2, 2}}}},
                    {}};
    std::string expected = "const auto& input1 = graph.AddInput(Tensor{DataType::uint32_t, {2, 2}}, 0);\n"
                           "const auto& input2 = graph.AddGraphConstant(Tensor{DataType::uint32_t, {2, 2}});\n"
                           "\n"
                           "const auto& output1 = Tensor{DataType::uint32_t, {2, 2}};\n"
                           "const auto& graphRes = graph.AddEqualOperator(input1, input2, output1);\n"
                           "graph.AddOutput(graphRes, 0);\n"
                           "graph.FinalizeGraph();";
    EXPECT_EQ(OperatorToGraphDefinition(op), expected);
}
