//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <tosa2spirv.hpp>
#include <gtest/gtest.h>

TEST(TOSA2SPIRV, EmptyShapeInputs)
{
    using namespace tosa2spirv::graphbuilder;
    std::vector<unsigned int> shape { };
    auto datatype =  DataType::int32_t;
    auto attribute = Tensor::CreateAttribute(datatype, shape);

    // If shape is empty then we expect the constructor to automatically return shape of { 1 }
    std::vector<unsigned int> expectedShape{ 1 };

    EXPECT_EQ(attribute.GetDataType(), DataType::int32_t);
    EXPECT_EQ(attribute.GetTensorShape(), expectedShape);

    // If passed in an empty shape and tensorValuePtr, ensure the tensor has correct values
    std::vector<unsigned int> tensorValuePtr { 5 };
    attribute = Tensor::CreateAttribute(datatype, shape, tensorValuePtr);

    // Shape and Datetype
    EXPECT_EQ(attribute.GetDataType(), DataType::int32_t);
    EXPECT_EQ(attribute.GetTensorShape(), expectedShape);

    // Test that we get the correct TensorPtrValue
    std::vector<unsigned int> expectedTensorPtrValue { 5 };
    EXPECT_EQ(attribute.GetTensorValuesUint32t(), expectedTensorPtrValue);

    auto module = Module::Create();
    auto input = Tensor::CreateInput(datatype, shape, module);

    EXPECT_EQ(input.GetDataType(), DataType::int32_t);
    EXPECT_EQ(input.GetTensorShape(), expectedShape);
}

TEST(TOSA2SPIRV, TensorNextAvailableBindingTest_Single)
{
    using namespace tosa2spirv::graphbuilder;

    auto module = Module::Create();
    auto graph = module->AddGraph();

    auto A = Tensor::CreateInput(DataType::int8_t,
                                 std::vector<unsigned int>{ 1,1,1,1 },
                                 module,
                                 "A");

    auto B = Tensor::CreateInput(DataType::int8_t,
                                 std::vector<unsigned int>{ 1,1,1,1 },
                                 module,
                                 "B");

    auto A_zp_val = Tensor::ConvertInt8tToUint32t({1});
    auto A_zp = Tensor::CreateAttribute(DataType::int8_t,
                                        std::vector<unsigned int>{ 1 },
                                        A_zp_val);

    auto B_zp_val = Tensor::ConvertInt8tToUint32t({1});
    auto B_zp = Tensor::CreateAttribute(DataType::int8_t,
                                        std::vector<unsigned int>{ 1 },
                                        B_zp_val);

    auto output = Tensor::CreateOutput(DataType::int32_t,
                                       std::vector<unsigned int>{ 1,1,1,1 },
                                       module,
                                       "output");

    graph->AddOperator(OperatorEnum::Matmul,
                       {A, B},
                       {output},
                       {A_zp, B_zp});

    EXPECT_TRUE(A.GetDescSetBinding()->GetBinding() == 0);
    EXPECT_TRUE(B.GetDescSetBinding()->GetBinding() == 1);
    EXPECT_TRUE(output.GetDescSetBinding()->GetBinding() == 2);
}

TEST(TOSA2SPIRV, TensorNextAvailableBindingTest_Multi)
{
    using namespace tosa2spirv::graphbuilder;

    auto module = Module::Create();

    auto graph = module->AddGraph();

    auto A = Tensor::CreateInput(DataType::int8_t,
                                 std::vector<unsigned int>{ 1,1,1,1 },
                                 module,
                                 "A");

    auto B = Tensor::CreateInput(DataType::int8_t,
                                 std::vector<unsigned int>{ 1,1,1,1 },
                                 module,
                                 "B");

    auto A_zp_val = Tensor::ConvertInt8tToUint32t({1});
    auto A_zp = Tensor::CreateAttribute(DataType::int8_t,
                                        std::vector<unsigned int>{ 1 },
                                        A_zp_val);

    auto B_zp_val = Tensor::ConvertInt8tToUint32t({1});
    auto B_zp = Tensor::CreateAttribute(DataType::int8_t,
                                        std::vector<unsigned int>{ 1 },
                                        B_zp_val);

    auto output = Tensor::CreateOutputToLayer(DataType::int32_t,
                                              std::vector<unsigned int>{ 1,1,1,1 });

    auto resId = graph->AddOperator(OperatorEnum::Matmul,
                                    {A, B},
                                    {output},
                                    {A_zp, B_zp});

    EXPECT_TRUE(A.GetDescSetBinding()->GetBinding() == 0);
    EXPECT_TRUE(B.GetDescSetBinding()->GetBinding() == 1);
    EXPECT_TRUE(output.GetDescSetBinding() == nullptr);

    auto C = Tensor::CreateInput(DataType::int8_t,
                                 std::vector<unsigned int>{ 1,1,1,1 },
                                 module,
                                 "C");

    auto D = Tensor::CreateInputFromLayer(DataType::int8_t,
                                          std::vector<unsigned int>{ 1,1,1,1 },
                                          resId[0]);

    auto graphOutput = Tensor::CreateOutput(DataType::int8_t,
                                            std::vector<unsigned int>{ 1,1,1,1 },
                                            module,
                                            "graphOutput");

    EXPECT_TRUE(D.GetDescSetBinding() == nullptr);
    EXPECT_EQ(C.GetDescSetBinding()->GetBinding() , 2 /* 2 */);
    EXPECT_EQ(graphOutput.GetDescSetBinding()->GetBinding() , 3);
}