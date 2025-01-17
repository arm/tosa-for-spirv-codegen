//
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#include <GeneratorUtils.hpp>
#include <ResourceInfo.hpp>
#include <ResourceInfoVisitor.hpp>
#include <TestUtils.hpp>
#include <TosaSerializationParser.hpp>

#include <gtest/gtest.h>
#include <vgf/encoder.hpp>
#include <vgf/decoder.hpp>
#include <vgf/vgf_dump.hpp>

using namespace tosa;
using namespace tosa2spirv;
using namespace parsers;
using namespace generator;

TosaSerializationBasicBlock GeneratorSimpleMaxpool2dModel()
{
    // Create Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> kernel = {2, 2};
    std::vector<int> stride = {2, 2};
    DType dType = DType::DType_INT8;
    TosaPoolAttribute attribute(pad, kernel, stride, 0, 0, dType);

    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, dType, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, dType, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_MAX_POOL2D,
                                                  Attribute::Attribute_PoolAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("max_pool_2d",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    return block;
}

// CONV 2D
TosaSerializationBasicBlock GeneratorSimpleConv2dModel()
{
    // Create Attribute
    std::vector<int32_t> pad      = { 1, 1, 1, 1 };
    std::vector<int32_t> dilation = { 1, 1 };
    std::vector<int32_t> stride   = { 2, 2 };
    std::vector<int8_t> input_zp = {-127};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute attribute(pad, stride, dilation, input_zp[0], weight_zp[0], true);

    // Create Tensors
    std::string inputName   = "input1";
    std::string weightsName = "weights";
    std::string biasName    = "bias";
    std::string outputName  = "output1";
    std::vector<int32_t> inputShape   = { 1, 5, 5, 1 };
    std::vector<int32_t> weightsShape = { 1, 3, 3, 1 };
    std::vector<int32_t> biasShape    = { 1 };
    std::vector<int32_t> outputShape  = { 1, 3, 3, 1 };

    auto* inputTensor   = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* weightsTensor = new TosaSerializationTensor(weightsName,
                                                      weightsShape,
                                                      DType::DType_INT8,
                                                      { 1, 2, 3, 4, 5, 6, 7, 8, 9 });
    auto* biasTensor    = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, { 2 });
    auto* outputTensor  = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto weightsOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                         Attribute::Attribute_NONE,
                                                         nullptr,
                                                         {},
                                                         { weightsName });
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST,
                                                      Attribute::Attribute_NONE,
                                                      nullptr,
                                                      {},
                                                      { biasName });
    auto op = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                  Attribute::Attribute_ConvAttribute,
                                                  &attribute,
                                                  { inputName, weightsName, biasName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("conv_2d",
                                      "main",
                                      { weightsOp, biasOp, op },
                                      { inputTensor, weightsTensor, biasTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    return block;
}

// Rescale
TosaSerializationBasicBlock GeneratorSimpleRescaleModel()
{
    // Create Attribute
    std::vector<int8_t> input_zp    = {20};
    std::vector<int8_t> output_zp   = {-128};
    std::vector<int32_t> multiplier = {60, 60, 60, 60};
    std::vector<int32_t> shift      = {30, 30, 30, 30};
    bool scale32       = true;
    bool double_round  = false;
    bool per_channel   = true;
    TosaRescaleAttribute attribute(
            input_zp[0], output_zp[0], multiplier, shift, scale32, double_round, per_channel, true, true);

    // Create Tensors
    std::string inputName       = "input1";
    std::string outputName      = "output1";
    std::vector<int32_t> inputShape  = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 4, 4};

    auto* inputTensor       = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor      = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                  Attribute::Attribute_RescaleAttribute,
                                                  &attribute,
                                                  { inputName },
                                                  { outputName });

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("rescale",
                                      "main",
                                      { op },
                                      { inputTensor, outputTensor },
                                      { inputName },
                                      { outputName });

    return block;
}

struct SimpleMaxpool2dExpected
{
    std::string goldenSPIRV = GetPreSPIRVStr() +
R"(          %3 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %23 DescriptorSet 0
               OpDecorate %23 Binding 0
               OpDecorate %27 DescriptorSet 0
               OpDecorate %27 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_3 = OpConstant %uint 3
         %10 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_3 %uint_3
         %11 = OpTypeTensorARM %uchar %uint_4 %10
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
     %uint_2 = OpConstant %uint 2
         %14 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %15 = OpTypeTensorARM %uint %uint_1 %14
         %16 = OpConstantCompositeReplicateEXT %15 %uint_2
         %17 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %18 = OpTypeTensorARM %uint %uint_1 %17
         %19 = OpConstantCompositeReplicateEXT %18 %uint_1
         %20 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_4 %uint_4
         %21 = OpTypeTensorARM %uchar %uint_4 %20
%_ptr_UniformConstant_21 = OpTypePointer UniformConstant %21
     %uint_0 = OpConstant %uint 0
%_ptr_UniformConstant_11 = OpTypePointer UniformConstant %11
         %23 = OpVariable %_ptr_UniformConstant_21 UniformConstant
         %27 = OpVariable %_ptr_UniformConstant_11 UniformConstant
         %28 = OpTypeGraphARM 1 %21 %11
               OpGraphEntryPointARM %1 "main" %23 %27
          %1 = OpGraphARM %28
         %25 = OpGraphInputARM %21 %uint_0
          %2 = OpExtInst %11 %3 MAX_POOL2D %16 %16 %19 %25
               OpGraphSetOutputARM %2 %uint_0
               OpGraphEndARM
)";

};

struct SimpleConv2dExpected
{
    std::string goldenSPIRV = GetPreSPIRVStr() +
R"(          %5 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %34 DescriptorSet 0
               OpDecorate %34 Binding 0
               OpDecorate %38 DescriptorSet 0
               OpDecorate %38 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_3 = OpConstant %uint 3
         %12 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_3 %uint_3 %uint_1
         %13 = OpTypeTensorARM %uchar %uint_4 %12
          %2 = OpGraphConstantARM %13 0
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
         %15 = OpConstantComposite %_arr_uint_uint_1 %uint_1
         %16 = OpTypeTensorARM %uint %uint_1 %15
          %3 = OpGraphConstantARM %16 1
         %17 = OpTypeTensorARM %uint %uint_4 %12
         %18 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %19 = OpTypeTensorARM %uint %uint_1 %18
         %20 = OpConstantCompositeReplicateEXT %19 %uint_1
     %uint_2 = OpConstant %uint 2
         %22 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %23 = OpTypeTensorARM %uint %uint_1 %22
         %24 = OpConstantCompositeReplicateEXT %23 %uint_2
         %25 = OpConstantCompositeReplicateEXT %23 %uint_1
  %uchar_129 = OpConstant %uchar 129
    %uchar_1 = OpConstant %uchar 1
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
     %uint_5 = OpConstant %uint 5
         %31 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_5 %uint_5 %uint_1
         %32 = OpTypeTensorARM %uchar %uint_4 %31
%_ptr_UniformConstant_32 = OpTypePointer UniformConstant %32
     %uint_0 = OpConstant %uint 0
%_ptr_UniformConstant_17 = OpTypePointer UniformConstant %17
         %34 = OpVariable %_ptr_UniformConstant_32 UniformConstant
         %38 = OpVariable %_ptr_UniformConstant_17 UniformConstant
         %39 = OpTypeGraphARM 1 %32 %17
               OpGraphEntryPointARM %1 "main" %34 %38
          %1 = OpGraphARM %39
         %36 = OpGraphInputARM %32 %uint_0
          %4 = OpExtInst %17 %5 CONV2D %20 %24 %25 %uchar_129 %uchar_1 %true %36 %2 %3
               OpGraphSetOutputARM %4 %uint_0
               OpGraphEndARM
)";
};

struct SimpleRescaleExpected
{
    std::string goldenSPIRV = GetPreSPIRVStr() +
        R"(          %5 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %23 DescriptorSet 0
               OpDecorate %23 Binding 0
               OpDecorate %26 DescriptorSet 0
               OpDecorate %26 Binding 1
       %uint = OpTypeInt 32 0
     %uint_1 = OpConstant %uint 1
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
     %uint_4 = OpConstant %uint 4
         %10 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %11 = OpTypeTensorARM %uint %uint_1 %10
          %2 = OpGraphConstantARM %11 0
      %uchar = OpTypeInt 8 0
         %13 = OpTypeTensorARM %uchar %uint_1 %10
          %3 = OpGraphConstantARM %13 1
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
         %15 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_1 %uint_4 %uint_4
         %16 = OpTypeTensorARM %uchar %uint_4 %15
   %uchar_20 = OpConstant %uchar 20
  %uchar_128 = OpConstant %uchar 128
       %bool = OpTypeBool
       %true = OpConstantTrue %bool
      %false = OpConstantFalse %bool
%_ptr_UniformConstant_16 = OpTypePointer UniformConstant %16
     %uint_0 = OpConstant %uint 0
         %23 = OpVariable %_ptr_UniformConstant_16 UniformConstant
         %26 = OpVariable %_ptr_UniformConstant_16 UniformConstant
         %27 = OpTypeGraphARM 1 %16 %16
               OpGraphEntryPointARM %1 "main" %23 %26
          %1 = OpGraphARM %27
         %25 = OpGraphInputARM %16 %uint_0
          %4 = OpExtInst %16 %5 RESCALE %uchar_20 %uchar_128 %2 %3 %true %false %true %true %true %25
               OpGraphSetOutputARM %4 %uint_0
               OpGraphEndARM
)";
};

#ifdef BUILD_EXAMPLE
struct SimpleMaxpool2dExampleExpected
{
    std::string goldenSPIRV = GetPreSPIRVStr() +
        R"(          %3 = OpExtInstImport "TOSA.000080.2"
               OpMemoryModel Logical Vulkan
               OpDecorate %24 DescriptorSet 0
               OpDecorate %24 Binding 0
               OpDecorate %28 DescriptorSet 0
               OpDecorate %28 Binding 1
      %uchar = OpTypeInt 8 0
       %uint = OpTypeInt 32 0
     %uint_4 = OpConstant %uint 4
%_arr_uint_uint_4 = OpTypeArray %uint %uint_4
     %uint_1 = OpConstant %uint 1
     %uint_8 = OpConstant %uint 8
    %uint_16 = OpConstant %uint 16
         %11 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_8 %uint_8 %uint_16
         %12 = OpTypeTensorARM %uchar %uint_4 %11
%_arr_uint_uint_1 = OpTypeArray %uint %uint_1
     %uint_2 = OpConstant %uint 2
         %15 = OpConstantComposite %_arr_uint_uint_1 %uint_2
         %16 = OpTypeTensorARM %uint %uint_1 %15
         %17 = OpConstantCompositeReplicateEXT %16 %uint_2
         %18 = OpConstantComposite %_arr_uint_uint_1 %uint_4
         %19 = OpTypeTensorARM %uint %uint_1 %18
         %20 = OpConstantNull %19
         %21 = OpConstantComposite %_arr_uint_uint_4 %uint_1 %uint_16 %uint_16 %uint_16
         %22 = OpTypeTensorARM %uchar %uint_4 %21
%_ptr_UniformConstant_22 = OpTypePointer UniformConstant %22
     %uint_0 = OpConstant %uint 0
%_ptr_UniformConstant_12 = OpTypePointer UniformConstant %12
         %24 = OpVariable %_ptr_UniformConstant_22 UniformConstant
         %28 = OpVariable %_ptr_UniformConstant_12 UniformConstant
         %29 = OpTypeGraphARM 1 %22 %12
               OpGraphEntryPointARM %1 "main" %24 %28
          %1 = OpGraphARM %29
         %26 = OpGraphInputARM %22 %uint_0
          %2 = OpExtInst %12 %3 MAX_POOL2D %17 %17 %20 %26
               OpGraphSetOutputARM %2 %uint_0
               OpGraphEndARM
)";
};

TEST(TOSA2SPIRV_GENERATOR, GeneratorTestExampleTosaFile)
{

    // Check that the example file is accessible.
    // It may not be if the test executable has been moved to a different device or the source files removed.
    // Skip the test if the example file cannot be found
    std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("GeneratorTests.cpp"));
    std::string tosaFile = dir_path + "../../examples/simple_maxpool2d.tosa";

    // Create a handler from the example file
    TosaSerializationHandler handler;
    tosa_err_t error = handler.LoadFileTosaFlatbuffer(tosaFile.c_str());
    if (error != tosa_err_t::TOSA_OK)

    {
        throw std::runtime_error("Error loading TOSA Flatbuffers file. Error code: " + std::to_string(error));
    }

    // Get the main block from the handler so that we can parse it
    auto mainBlock = handler.GetMainRegion()->GetBlockByName("main");
    if(mainBlock == nullptr)
    {
        throw std::runtime_error("Please ensure there is a block called \"main\" within the TOSA graph.");
    }

    // Initialize TosaSerializationParser with main block.
    auto parser = tosa2spirv::parsers::TosaSerializationParser(mainBlock);

    // Create SPIRV
    std::vector<uint32_t> spirv = parser.GenerateSPIRV("main");

    const std::string outputStr(spirvwriter::DisassembleSPIRV((std::vector<uint32_t> &) spirv, true));

    // Check expected spirv and actual spirv
    SimpleMaxpool2dExampleExpected expected;
    EXPECT_EQ(expected.goldenSPIRV, outputStr);

}

TEST(TOSA2SPIRV_GENERATOR, GeneratorTestExampleJsonFile)
{
    // Check that the example file is accessible.
    // It may not be if the test executable has been moved to a different device or the source files removed.
    // Skip the test if the example file cannot be found
    std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("GeneratorTests.cpp"));
    std::string jsonFile = dir_path + "../../examples/simple_maxpool2d.json";
    std::ifstream fileStream;
    fileStream.open(jsonFile);

    auto companion = nlohmann::json::parse(fileStream);

    std::string expectedJson("{\"regions\":["
                             "{\"blocks\":["
                             "{\"inputs\":["
                             "\"input_1\"],"
                             "\"name\":"
                             "\"main\","
                             "\"operators\":["
                             "{\"attribute\":"
                             "{\"accum_dtype\":\"INT8\","
                             "\"kernel\":[2,2],"
                             "\"pad\":[0,0,0,0],"
                             "\"stride\":[2,2]},"
                             "\"attribute_type\":"
                             "\"PoolAttribute\","
                             "\"inputs\":["
                             "\"input_1\"],"
                             "\"op\":\"MAX_POOL2D\","
                             "\"outputs\":["
                             "\"output0_2\"]}],"
                             "\"outputs\":["
                             "\"output0_2\"],"
                             "\"tensors\":["
                             "{\"data\":[],"
                             "\"name\":\"input_1\","
                             "\"shape\":[1,16,16,16],"
                             "\"type\":\"INT8\","
                             "\"variable_name\":\"\"},"
                             "{\"data\":[],"
                             "\"name\":\"output0_2\","
                             "\"shape\":[1,8,8,16],"
                             "\"type\":\"INT8\","
                             "\"variable_name\":\"\"}]}],"
                             "\"name\":\"main\"}],"
                             "\"version\":{"
                             "\"_draft\":false,"
                             "\"_major\":0,"
                             "\"_minor\":80,"
                             "\"_patch\":1}}");

    EXPECT_EQ(companion.dump(), expectedJson);

}
#endif // BUILD_EXAMPLE

static bool fileExists(const char* filename)
{
    std::ifstream file(filename);
    return file.good();
}

TEST(TOSA2SPIRV_GENERATOR, GeneratorWriteVgfFile)
{
    // Create a simple MaxPool2d model block.
    TosaSerializationBasicBlock block = GeneratorSimpleMaxpool2dModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    TosaSerializationParser parser(&block);

    std::string modelName = "quantized_max_pool_single_layer";
    auto vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto spirv = parser.GenerateSPIRV("main");
    WriteVgfFile(&parser, spirv, modelName, ".", vgfName);

    // Open vgf archive
    if (!fileExists(vgfName.c_str()))
    {
        FAIL() << "GeneratorWriteVgfFile: Error on opening quantized_max_pool_single_layer.vgf.";
    }

    std::string expectedJson =
R"(            {
                    "constants": [],
            "header": {
                "major": 0,
                        "minor": 3,
                        "patch": 0
            },
            "model_sequence": {
                "inputs": [
                {
                    "binding": 0,
                            "index": 0,
                            "mrt_index": 0,
                            "name": "input_0"
                }
                ],
                "outputs": [
                {
                    "binding": 1,
                            "index": 0,
                            "mrt_index": 1,
                            "name": "output_0"
                }
                ],
                "segments": [
                {
                    "constants": [],
                            "descriptor_set_infos": [
                    [
                    {
                    "binding": 0,
                    "index": 0,
                    "mrt_index": 0
                    },
                    {
                    "binding": 1,
                    "index": 1,
                    "mrt_index": 1
                    }
                    ]
                    ],
                    "dispatch_shape": [0, 0, 0],
                    "index": 0,
                            "inputs": [
                    {
                        "binding": 0,
                                "index": 0,
                                "mrt_index": 0
                    }
                    ],
                    "module_index": 0,
                            "name": "segment_quantized_max_pool_single_layer",
                            "outputs": [
                    {
                        "binding": 1,
                                "index": 0,
                                "mrt_index": 1
                    }
                    ],
                    "push_constant_ranges": [],
                            "type": "GRAPH"
                }
                ]
            },
            "modules": [
            {
                "code_size": 198,
                        "entry_point": "main",
                        "has_spirv": true,
                        "index": 0,
                        "name": "quantized_max_pool_single_layer",
                        "type": "GRAPH"
            }
            ],
            "resources": [
            {
                "category": "INPUT",
                        "index": 0,
                        "shape": [1, 1, 4, 4],
                "stride": [],
                        "vk_descriptor_type": "none",
                        "vk_format": "VK_FORMAT_R8_SINT"
            },
            {
                "category": "OUTPUT",
                        "index": 1,
                        "shape": [1, 1, 3, 3],
                "stride": [],
                        "vk_descriptor_type": "none",
                        "vk_format": "VK_FORMAT_R8_SINT"
            }
            ]
            }
)";

    EXPECT_EQ(vgf_dump::getFile(vgfName), nlohmann::json::parse(expectedJson));

    // Open spv file
    const auto spvData = vgf_dump::getSpirv(vgfName, 0);
    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(spirvwriter::DisassembleSPIRV(spvData, true));

    // Check as expected
    SimpleMaxpool2dExpected expected;
    EXPECT_EQ(expected.goldenSPIRV, outputStr);

    // Delete the .vgf
    if (std::remove(vgfName.c_str()) != 0)
    {
        FAIL() << "Error deleting vgf file.";
    }
}

TEST(TOSA2SPIRV_GENERATOR, GeneratorWriteNpyFiles)
{
    // Create a simple MaxPool2d model block.
    TosaSerializationBasicBlock block = GeneratorSimpleConv2dModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    TosaSerializationParser parser(&block);

    std::string modelName = "quantized_conv2d_single_layer";
    auto vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto spirv = parser.GenerateSPIRV("main");
    WriteVgfFile(&parser, spirv, modelName, ".", vgfName);

    // Open vgf archive
    if (!fileExists(vgfName.c_str()))
    {
        FAIL() << "GeneratorWriteVgfFile: Error on opening quantized_conv2d_single_layer.vgf.";
    }

    std::string expectedJson =
R"(    {
        "constants": [
        {
            "index": 0,
                    "mrt_index": 0,
                    "sparsity_dimension": -1
        },
        {
            "index": 1,
                    "mrt_index": 1,
                    "sparsity_dimension": -1
        }
        ],
        "header": {
            "major": 0,
                    "minor": 3,
                    "patch": 0
        },
        "model_sequence": {
            "inputs": [
            {
                "binding": 0,
                        "index": 0,
                        "mrt_index": 2,
                        "name": "input_0"
            }
            ],
            "outputs": [
            {
                "binding": 1,
                        "index": 0,
                        "mrt_index": 3,
                        "name": "output_0"
            }
            ],
            "segments": [
            {
                "constants": [0, 1],
                "descriptor_set_infos": [
                [
                {
                "binding": 0,
                "index": 0,
                "mrt_index": 2
                },
                {
                "binding": 1,
                "index": 1,
                "mrt_index": 3
                }
                ]
                ],
                "dispatch_shape": [0, 0, 0],
                "index": 0,
                        "inputs": [
                {
                    "binding": 0,
                            "index": 0,
                            "mrt_index": 2
                }
                ],
                "module_index": 0,
                        "name": "segment_quantized_conv2d_single_layer",
                        "outputs": [
                {
                    "binding": 1,
                            "index": 0,
                            "mrt_index": 3
                }
                ],
                "push_constant_ranges": [],
                        "type": "GRAPH"
            }
            ]
        },
        "modules": [
        {
            "code_size": 246,
                    "entry_point": "main",
                    "has_spirv": true,
                    "index": 0,
                    "name": "quantized_conv2d_single_layer",
                    "type": "GRAPH"
        }
        ],
        "resources": [
        {
            "category": "CONSTANT",
                    "index": 0,
                    "shape": [1, 3, 3, 1],
            "stride": [],
                    "vk_descriptor_type": "none",
                    "vk_format": "VK_FORMAT_R8_SINT"
        },
        {
            "category": "CONSTANT",
                    "index": 1,
                    "shape": [1],
            "stride": [],
                    "vk_descriptor_type": "none",
                    "vk_format": "VK_FORMAT_R32_SINT"
        },
        {
            "category": "INPUT",
                    "index": 2,
                    "shape": [1, 5, 5, 1],
            "stride": [],
                    "vk_descriptor_type": "none",
                    "vk_format": "VK_FORMAT_R8_SINT"
        },
        {
            "category": "OUTPUT",
                    "index": 3,
                    "shape": [1, 3, 3, 1],
            "stride": [],
                    "vk_descriptor_type": "none",
                    "vk_format": "VK_FORMAT_R32_SINT"
        }
        ]
    }
)";

    EXPECT_EQ(vgf_dump::getFile(vgfName), nlohmann::json::parse(expectedJson));

    // Open spv file
    const auto spvData = vgf_dump::getSpirv(vgfName, 0);
    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(spirvwriter::DisassembleSPIRV(spvData, true));

    // Check as expected
    SimpleConv2dExpected expected;
    EXPECT_EQ(expected.goldenSPIRV, outputStr);

    const std::vector<uint8_t> weightData = vgf_dump::getConstant(vgfName, 0);
    const std::vector<int8_t> expectedWeightData{1,2,3,4,5,6,7,8,9};
    EXPECT_EQ (std::equal(weightData.begin(), weightData.begin() + weightData.size(), expectedWeightData.begin()), 1);

    const std::vector<uint8_t> biasData = vgf_dump::getConstant(vgfName, 1);
    const std::vector<int8_t> expectedBiasData{2};
    EXPECT_EQ (std::equal(biasData.begin(), biasData.begin() + biasData.size(), expectedBiasData.begin()), 1);

    // Delete the .vgf
    if (std::remove(vgfName.c_str()) != 0)
    {
        FAIL() << "Error deleting vgf file.";
    }
}

TEST(TOSA2SPIRV_GENERATOR, GeneratorWriteNpyFilesRescale)
{
    // Create a simple MaxPool2d model block.
    TosaSerializationBasicBlock block = GeneratorSimpleRescaleModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    TosaSerializationParser parser(&block);

    std::string modelName = "quantized_rescale_single_layer";
    auto vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto spirv = parser.GenerateSPIRV("main");
    WriteVgfFile(&parser, spirv, modelName, ".", vgfName);

    // Open vgf archive
    if (!fileExists(vgfName.c_str()))
    {
        FAIL() << "GeneratorWriteVgfFile: Error on opening quantized_conv2d_single_layer.vgf.";
    }

    std::string expectedJson =
R"({
    "constants": [
        {
            "index": 0,
            "mrt_index": 0,
            "sparsity_dimension": -1
        },
        {
            "index": 1,
            "mrt_index": 1,
            "sparsity_dimension": -1
        }
    ],
    "header": {
        "major": 0,
        "minor": 3,
        "patch": 0
    },
    "model_sequence": {
        "inputs": [
            {
                "binding": 0,
                "index": 0,
                "mrt_index": 2,
                "name": "input_0"
            }
        ],
        "outputs": [
            {
                "binding": 1,
                "index": 0,
                "mrt_index": 3,
                "name": "output_0"
            }
        ],
        "segments": [
            {
                "constants": [0, 1],
                "descriptor_set_infos": [
                    [
                        {
                            "binding": 0,
                            "index": 0,
                            "mrt_index": 2
                        },
                        {
                            "binding": 1,
                            "index": 1,
                            "mrt_index": 3
                        }
                    ]
                ],
                "dispatch_shape": [0, 0, 0],
                "index": 0,
                "inputs": [
                    {
                        "binding": 0,
                        "index": 0,
                        "mrt_index": 2
                    }
                ],
                "module_index": 0,
                "name": "segment_quantized_rescale_single_layer",
                "outputs": [
                    {
                        "binding": 1,
                        "index": 0,
                        "mrt_index": 3
                    }
                ],
                "push_constant_ranges": [],
                "type": "GRAPH"
            }
        ]
    },
    "modules": [
        {
            "code_size": 192,
            "entry_point": "main",
            "has_spirv": true,
            "index": 0,
            "name": "quantized_rescale_single_layer",
            "type": "GRAPH"
        }
    ],
    "resources": [
        {
            "category": "CONSTANT",
            "index": 0,
            "shape": [4],
            "stride": [],
            "vk_descriptor_type": "none",
            "vk_format": "VK_FORMAT_R32_SINT"
        },
        {
            "category": "CONSTANT",
            "index": 1,
            "shape": [4],
            "stride": [],
            "vk_descriptor_type": "none",
            "vk_format": "VK_FORMAT_R8_SINT"
        },
        {
            "category": "INPUT",
            "index": 2,
            "shape": [1, 1, 4, 4],
            "stride": [],
            "vk_descriptor_type": "none",
            "vk_format": "VK_FORMAT_R8_SINT"
        },
        {
            "category": "OUTPUT",
            "index": 3,
            "shape": [1, 1, 4, 4],
            "stride": [],
            "vk_descriptor_type": "none",
            "vk_format": "VK_FORMAT_R8_SINT"
        }
    ]
})";

    EXPECT_EQ(vgf_dump::getFile(vgfName), nlohmann::json::parse(expectedJson));

    // Open spv file
    const auto spvData = vgf_dump::getSpirv(vgfName, 0);
    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(spirvwriter::DisassembleSPIRV(spvData, true));
    // Check as expected
    SimpleRescaleExpected expected;
    EXPECT_EQ(expected.goldenSPIRV, outputStr);

    const std::vector<int32_t> expectedMultData{60, 60, 60, 60};
    const std::vector<int8_t> expectedShiftData{30, 30, 30, 30};

    const std::vector<uint8_t> multData = vgf_dump::getConstant(vgfName, 0);

    // Ensure vect is multiple of 4
    EXPECT_EQ(multData.size() % 4, 0);

    // Build up the int32 vector again from uint8
    std::vector<int32_t> resultMultVect;
    for (size_t i = 0; i < multData.size(); i += 4) {
        int32_t value = 0;
        value |= static_cast<int32_t>(multData[i]) << 0;
        value |= static_cast<int32_t>(multData[i + 1]) << 8;
        value |= static_cast<int32_t>(multData[i + 2]) << 16;
        value |= static_cast<int32_t>(multData[i + 3]) << 24;
        resultMultVect.push_back(value);
    }
    EXPECT_EQ (std::equal(resultMultVect.begin(), resultMultVect.begin() + resultMultVect.size(), expectedMultData.begin()), 1);

    const std::vector<uint8_t> shiftData = vgf_dump::getConstant(vgfName, 1);
    EXPECT_EQ (std::equal(shiftData.begin(), shiftData.begin() + shiftData.size(), expectedShiftData.begin()), 1);

    // Delete the .vgf
    if (std::remove(vgfName.c_str()) != 0)
    {
        FAIL() << "Error deleting vgf file.";
    }
}

TEST(TOSA2SPIRV_GENERATOR, ResourceInfoLayerWriteWithVisitor)
{
    using namespace graphbuilder;

    auto module = Module::Create();

    auto graph = module->AddGraph();

    // Don't give ResId to input Tensor so that it is seen as a Graph Input
    auto input = Tensor::CreateInput(DataType::int8_t,
                                     std::vector<unsigned int>{1,1},
                                     module,
                                     "input_tensor_ptr");

    // Attributes
    auto kernel_val = Tensor::ConvertInt8tToUint32t({1,1});
    auto kernel = Tensor::CreateAttribute(DataType::int8_t,
                                          std::vector<unsigned int>{ 2 },
                                          kernel_val);

    auto stride_val = Tensor::ConvertInt8tToUint32t({1,1});
    auto stride = Tensor::CreateAttribute(DataType::int8_t,
                                          std::vector<unsigned int>{ 2 },
                                          stride_val);

    auto pad_val = Tensor::ConvertInt8tToUint32t({1,1});
    auto pad = Tensor::CreateAttribute(DataType::int8_t,
                                       std::vector<unsigned int>{ 2 },
                                       pad_val);

    auto output = Tensor{DataType::int8_t, std::vector<unsigned int>{1,1}};

    auto maxPool2d = graph->AddMaxPool2dOperator(input, kernel, stride, pad, output);


    // Pass in ResId as it is coming from previous layer
    auto inputFromLayer = Tensor{DataType::int8_t, std::vector<unsigned int>{1,1}, maxPool2d};


    // Create an output tensor with isGraphOutput enabled.
    auto outputTensor = Tensor::CreateOutput(DataType::int8_t,
                                             std::vector<unsigned int>{1,1},
                                             module,
                                             "output_tensor_ptr");

    auto maxPool2d_2 = graph->AddMaxPool2dOperator(inputFromLayer,  kernel, stride, pad, outputTensor);

    // Use Module to write the full binary.
    module->Write();

    // Information about the layers / graph are accessed using a visitor. Calling module->Accept allows us to access
    // and check that the information is correct via the ResourceInfo within the visitor that has now been updated.
    ResourceInfoVisitor visitor;
    module->Accept(visitor);
    auto resourceInfo = visitor.GetResourceInfo();

    ResourceInfo resourceInfoExpected;
    resourceInfoExpected.AddInput(0,                                                            // bindingId
                                  0,                                                            // descriptorSet
                                  "VK_FORMAT_R8_SINT",                                          // format
                                  0,                                                            // inputIndex
                                  "VK_TENSOR_TILING_LINEAR_EXT",                                // layout
                                  "Maxpool2d_" + std::to_string(maxPool2d.Get()),                 // name
                                  "VK_DESCRIPTOR_TYPE_STORAGE_TENSOR_EXT",                      // resourceType
                                  std::vector<unsigned int>{1,1});                              // shape);
    resourceInfoExpected.AddOutput(1,
                                   0,
                                   "VK_FORMAT_R8_SINT",
                                   0,
                                   "VK_TENSOR_TILING_LINEAR_EXT",
                                   "Maxpool2d_" + std::to_string(maxPool2d_2.Get()),
                                   "VK_DESCRIPTOR_TYPE_STORAGE_TENSOR_EXT",
                                   std::vector<unsigned int>{1,1});

    // Ensure Constants and Intermediates are empty as they should not be used here.
    EXPECT_TRUE(resourceInfo.GetConstants().empty());
    EXPECT_TRUE(resourceInfo.GetIntermediates().empty());

    // Check resourceInfo is as expected.
    EXPECT_EQ(resourceInfo.GetInputs()[0].m_BindingId, resourceInfoExpected.GetInputs()[0].m_BindingId);
    EXPECT_EQ(resourceInfo.GetInputs()[0].m_DescriptorSet, resourceInfoExpected.GetInputs()[0].m_DescriptorSet);
    EXPECT_EQ(resourceInfo.GetInputs()[0].m_Format, resourceInfoExpected.GetInputs()[0].m_Format);
    EXPECT_EQ(resourceInfo.GetInputs()[0].m_IOIndex, resourceInfoExpected.GetInputs()[0].m_IOIndex);
    EXPECT_EQ(resourceInfo.GetInputs()[0].m_Layout, resourceInfoExpected.GetInputs()[0].m_Layout);
    EXPECT_EQ(resourceInfo.GetInputs()[0].m_Name, resourceInfoExpected.GetInputs()[0].m_Name);
    EXPECT_EQ(resourceInfo.GetInputs()[0].m_ResourceType, resourceInfoExpected.GetInputs()[0].m_ResourceType);

    EXPECT_EQ(resourceInfo.GetOutputs()[0].m_BindingId, resourceInfoExpected.GetOutputs()[0].m_BindingId);
    EXPECT_EQ(resourceInfo.GetOutputs()[0].m_DescriptorSet, resourceInfoExpected.GetOutputs()[0].m_DescriptorSet);
    EXPECT_EQ(resourceInfo.GetOutputs()[0].m_Format, resourceInfoExpected.GetOutputs()[0].m_Format);
    EXPECT_EQ(resourceInfo.GetOutputs()[0].m_IOIndex, resourceInfoExpected.GetOutputs()[0].m_IOIndex);
    EXPECT_EQ(resourceInfo.GetOutputs()[0].m_Layout, resourceInfoExpected.GetOutputs()[0].m_Layout);
    EXPECT_EQ(resourceInfo.GetOutputs()[0].m_Name, resourceInfoExpected.GetOutputs()[0].m_Name);
    EXPECT_EQ(resourceInfo.GetOutputs()[0].m_ResourceType, resourceInfoExpected.GetOutputs()[0].m_ResourceType);
}