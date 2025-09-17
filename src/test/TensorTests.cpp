//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <AssemblyUtils.hpp>
#include <Graph.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <AssemblyUtils.hpp>

#include <gtest/gtest.h>

TEST(TOSA_FOR_SPIRV_CODEGEN, EmptyShapeInputs)
{
    using namespace tfsc::tosa;
    std::vector<unsigned int> shape{};
    auto datatype = DataType::int32_t;
    auto tensor = Tensor(datatype, shape);

    // If shape is empty then we expect the constructor to automatically return shape of { 1 }
    std::vector<unsigned int> expectedShape{1};

    EXPECT_EQ(tensor.GetDataType(), DataType::int32_t);
    EXPECT_EQ(tensor.GetTensorShape(), expectedShape);

    // If passed in an empty shape and tensorValuePtr, ensure the tensor has correct values
    std::vector<unsigned int> tensorValuePtr{5};
    auto attribute = Attribute({5u}, datatype, shape);

    // Shape and Datetype
    EXPECT_EQ(attribute.GetTensor().GetDataType(), DataType::int32_t);
    EXPECT_EQ(attribute.GetTensor().GetTensorShape(), expectedShape);

    // Test that we get the correct TensorPtrValue
    std::vector<unsigned int> expectedTensorPtrValue{5};
    EXPECT_EQ(attribute.GetData(), expectedTensorPtrValue);

    auto input = Tensor(datatype, shape);

    EXPECT_EQ(input.GetDataType(), DataType::int32_t);
    EXPECT_EQ(input.GetTensorShape(), expectedShape);
}

TEST(TOSA_FOR_SPIRV_CODEGEN, FloatingPointAttributes)
{
    // Taken from modelzoo_deit_base_fp32_layer_000_conv_2d
    using namespace tfsc::tosa;
    auto floatAttr = Attribute(std::vector<float>{3.14f}, DataType::float32_t);
    auto data = floatAttr.GetData();

    float actual;
    memcpy(&actual, &data[0], sizeof(float));

    // Expect same value up to first 3 decimal places
    EXPECT_NEAR(actual, 3.14f, 0.001f);

    auto module = CreateModule(tfsc::TOSAVersion{});
    auto graph = Graph{module};

    auto input = graph.AddInput(Tensor(DataType::float32_t, std::vector<unsigned int>{1, 224, 224, 3}), 0);
    auto weight = graph.AddInput(Tensor(DataType::float32_t, std::vector<unsigned int>{768, 16, 16, 3}), 0);
    auto bias = graph.AddInput(Tensor(DataType::float32_t, std::vector<unsigned int>{768}), 0);
    auto pad = Attribute({1, 1, 1, 1}, DataType::int32_t);
    auto stride = Attribute({1, 1}, DataType::int32_t);
    auto dilation = Attribute({1, 1}, DataType::int32_t);
    auto input_zp_attr = Attribute({1, 1, 1, 1}, DataType::float32_t);
    auto input_zp = graph.AddTensorConstant(input_zp_attr);
    auto weight_zp_attr = Attribute({1, 1, 1, 1}, DataType::float32_t);
    auto weight_zp = graph.AddTensorConstant(weight_zp_attr);
    auto acc_type = Attribute({1}, DataType::int32_t);
    auto local_bound = Attribute({false}, DataType::bool_t);
    auto output = Tensor(DataType::float32_t, std::vector<unsigned int>{1, 14, 14, 768});

    const auto res = graph.AddConv2dOperator(input,
                                             weight,
                                             bias,
                                             input_zp,
                                             weight_zp,
                                             pad,
                                             stride,
                                             dilation,
                                             acc_type,
                                             local_bound,
                                             output);
    // Add output in place
    graph.AddOutput(res, 1);
    graph.FinalizeGraph();
    auto binary = tfsc::WriteToBinary(module);
    // Check SPIR-V is valid
    std::string outputStr(testutils::DisassembleSPIRV(binary, true));
    EXPECT_FALSE(outputStr.empty());
}
