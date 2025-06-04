//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#include <fstream>

#include <AssemblyUtils.hpp>
#include <ModuleComparator.hpp>
#include <TosaSerializationParser.hpp>
#include <VgfWriter.hpp>
#include <tosa2spirv.hpp>

#include <gtest/gtest.h>
#include <spirvmodels/Conv2d.hpp>
#include <spirvmodels/MaxPool.hpp>
#include <spirvmodels/Rescale.hpp>
#include <vgf/decoder.hpp>
#include <vgf/vgf_dump.hpp>

using namespace tosa;

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
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("max_pool_2d",
                                      "main",
                                      {op},
                                      {inputTensor, outputTensor},
                                      {inputName},
                                      {outputName});

    return block;
}

// CONV 2D
TosaSerializationBasicBlock GeneratorSimpleConv2dModel()
{
    // Create Attribute
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> dilation = {1, 1};
    std::vector<int32_t> stride = {2, 2};
    std::vector<int8_t> input_zp = {-127};
    std::vector<int8_t> weight_zp = {1};
    TosaConvAttribute attribute(pad, stride, dilation, input_zp[0], weight_zp[0], true);

    // Create Tensors
    std::string inputName = "input1";
    std::string weightsName = "weights";
    std::string biasName = "bias";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 5, 5, 1};
    std::vector<int32_t> weightsShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});

    auto* weightsTensor =
        new TosaSerializationTensor(weightsName, weightsShape, DType::DType_INT8, {1, 2, 3, 4, 5, 6, 7, 8, 9});

    auto* biasTensor = new TosaSerializationTensor(biasName, biasShape, DType::DType_INT32, {2, 0, 0, 0});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT32, {});

    // Create Operator
    auto weightsOp =
        new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {weightsName});
    auto biasOp = new tosa::TosaSerializationOperator(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, {}, {biasName});
    auto op = new tosa::TosaSerializationOperator(Op::Op_CONV2D,
                                                  Attribute::Attribute_ConvAttribute,
                                                  &attribute,
                                                  {inputName, weightsName, biasName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("conv_2d",
                                      "main",
                                      {weightsOp, biasOp, op},
                                      {inputTensor, weightsTensor, biasTensor, outputTensor},
                                      {inputName},
                                      {outputName});

    return block;
}

// Rescale
TosaSerializationBasicBlock GeneratorSimpleRescaleModel()
{
    // Create Attribute
    std::vector<int8_t> input_zp = {20};
    std::vector<int8_t> output_zp = {-128};
    std::vector<int32_t> multiplier = {60, 60, 60, 60};
    std::vector<int32_t> shift = {30, 30, 30, 30};
    bool scale32 = true;
    bool double_round = false;
    bool per_channel = true;
    TosaRescaleAttribute
        attribute(input_zp[0], output_zp[0], multiplier, shift, scale32, double_round, per_channel, true, true);

    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 4, 4};

    auto* inputTensor = new TosaSerializationTensor(inputName, inputShape, DType::DType_INT8, {});
    auto* outputTensor = new TosaSerializationTensor(outputName, outputShape, DType::DType_INT8, {});

    // Create Operator
    auto op = new tosa::TosaSerializationOperator(Op::Op_RESCALE,
                                                  Attribute::Attribute_RescaleAttribute,
                                                  &attribute,
                                                  {inputName},
                                                  {outputName});

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("rescale", "main", {op}, {inputTensor, outputTensor}, {inputName}, {outputName});

    return block;
}

TEST(TOSA2SPIRV_GENERATOR, GeneratorTestExampleTosaFile)
{
    // Check that the example file is accessible.
    // It may not be if the test executable has been moved to a different device or the source files removed.
    // Skip the test if the example file cannot be found
    std::string tosaFile;
    if (const char* envPath = std::getenv("TOSA_INPUT_PATH"))
    {
        tosaFile = envPath;
    }
    else
    {
        std::string file_path = __FILE__;
        std::string dir_path = file_path.substr(0, file_path.rfind("GeneratorTests.cpp"));
        tosaFile = dir_path + "../../examples/simple_maxpool2d.tosa";
    }

    // Create a handler from the example file
    TosaSerializationHandler handler;
    tosa_err_t error = handler.LoadFileTosaFlatbuffer(tosaFile.c_str());
    if (error != tosa_err_t::TOSA_OK)

    {
        throw std::runtime_error("Error loading TOSA Flatbuffers file. Error code: " + std::to_string(error));
    }

    // Get the main block from the handler so that we can parse it
    auto mainBlock = handler.GetMainRegion()->GetBlockByName("main");
    if (mainBlock == nullptr)
    {
        throw std::runtime_error("Please ensure there is a block called \"main\" within the TOSA graph.");
    }

    // Initialize TosaSerializationParser with main block.
    auto parser = tosa2spirv::parsers::TosaSerializationParser(mainBlock);

    // Create SPIRV
    const auto module1 = parser.GenerateSPIRVModule("main");

    std::vector<uint32_t> spirv = tosa2spirv::WriteToBinary(module1);
    std::string outputStr(testutils::DisassembleSPIRV(spirv, true));
    // Check expected spirv and actual spirv
    const auto diff = testutils::CompareModules(module1, spirvmodels::SimpleMaxpool2dGenerator);
    EXPECT_TRUE(diff.empty());
}

TEST(TOSA2SPIRV_GENERATOR, GeneratorTestExampleJsonFile)
{
    // Check that the example file is accessible.
    // It may not be if the test executable has been moved to a different device or the source files removed.
    // Skip the test if the example file cannot be found
    std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("GeneratorTests.cpp"));

    std::string jsonFile;
    if (const char* envPath = std::getenv("JSON_INPUT_PATH"))
    {
        jsonFile = envPath;
    }
    else
    {
        std::string file_path = __FILE__;
        std::string dir_path = file_path.substr(0, file_path.rfind("GeneratorTests.cpp"));
        jsonFile = dir_path + "../../examples/simple_maxpool2d.json";
    }
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

static bool fileExists(const char* filename)
{
    const std::ifstream file(filename);
    return file.good();
}

TEST(TOSA2SPIRV_GENERATOR, GeneratorWriteVgfFile)
{
    // Create a simple MaxPool2d model block.
    TosaSerializationBasicBlock block = GeneratorSimpleMaxpool2dModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    tosa2spirv::parsers::TosaSerializationParser parser(&block);

    std::string modelName = "quantized_max_pool_single_layer";
    std::string vgfDir = std::getenv("VGF_OUTPUT_PATH") ? std::getenv("VGF_OUTPUT_PATH") : ".";
    std::string vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto module = parser.GenerateSPIRVModule("main");
    auto spirv = tosa2spirv::WriteToBinary(module);
    auto constants = parser.GetExternalConstants();

    tosa2spirv::vgfwriter::WriteVGF(module,
                                    tosa2spirv::parsers::ConvertConstantDataToVoid(std::move(constants)),
                                    modelName,
                                    vgfDir,
                                    vgfName);

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
                "code_size": 200,
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
                        "vk_descriptor_type": "VK_DESCRIPTOR_TYPE_TENSOR_ARM",
                        "vk_format": "VK_FORMAT_R8_SINT"
            },
            {
                "category": "OUTPUT",
                        "index": 1,
                        "shape": [1, 1, 3, 3],
                "stride": [],
                        "vk_descriptor_type": "VK_DESCRIPTOR_TYPE_TENSOR_ARM",
                        "vk_format": "VK_FORMAT_R8_SINT"
            }
            ]
            }
)";

    EXPECT_EQ(mlsdk::vgf_dump::getFile(vgfName), nlohmann::json::parse(expectedJson));

    // Open spv file
    std::vector<uint32_t> spvData;
    mlsdk::vgf_dump::getSpirv(vgfName, 0, [&spvData](const uint32_t* data, size_t size) {
        spvData.assign(data, data + size);
    });

    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(testutils::DisassembleSPIRV(spvData, true));

    // Check as expected
    const auto diff = testutils::CompareModules(module, spirvmodels::SimpleMaxpool2d);
    EXPECT_TRUE(diff.empty());
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
    tosa2spirv::parsers::TosaSerializationParser parser(&block);

    std::string modelName = "quantized_conv2d_single_layer";
    std::string vgfDir = std::getenv("VGF_OUTPUT_PATH") ? std::getenv("VGF_OUTPUT_PATH") : ".";
    std::string vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto module = parser.GenerateSPIRVModule("main");
    auto spirv = tosa2spirv::WriteToBinary(module);
    auto constants = parser.GetExternalConstants();

    tosa2spirv::vgfwriter::WriteVGF(module,
                                    tosa2spirv::parsers::ConvertConstantDataToVoid(std::move(constants)),
                                    modelName,
                                    ".",
                                    vgfName);

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
        }]
        ,
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
                        "mrt_index": 1,
                        "name": "input_0"
            }
            ],
            "outputs": [
            {
                "binding": 1,
                        "index": 0,
                        "mrt_index": 2,
                        "name": "output_0"
            }
            ],
            "segments": [
            {
                "constants": [0],
                "descriptor_set_infos": [
                [
                {
                "binding": 0,
                "index": 0,
                "mrt_index": 1
                },
                {
                "binding": 1,
                "index": 1,
                "mrt_index": 2
                }
                ]
                ],
                "dispatch_shape": [0, 0, 0],
                "index": 0,
                        "inputs": [
                {
                    "binding": 0,
                            "index": 0,
                            "mrt_index": 1
                }
                ],
                "module_index": 0,
                        "name": "segment_quantized_conv2d_single_layer",
                        "outputs": [
                {
                    "binding": 1,
                            "index": 0,
                            "mrt_index": 2
                }
                ],
                "push_constant_ranges": [],
                        "type": "GRAPH"
            }
            ]
        },
        "modules": [
        {
            "code_size": 248,
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
            "category": "INPUT",
                    "index": 1,
                    "shape": [1, 5, 5, 1],
            "stride": [],
                    "vk_descriptor_type": "VK_DESCRIPTOR_TYPE_TENSOR_ARM",
                    "vk_format": "VK_FORMAT_R8_SINT"
        },
        {
            "category": "OUTPUT",
                    "index": 2,
                    "shape": [1, 3, 3, 1],
            "stride": [],
                    "vk_descriptor_type": "VK_DESCRIPTOR_TYPE_TENSOR_ARM",
                    "vk_format": "VK_FORMAT_R32_SINT"
        }
        ]
    }
)";

    EXPECT_EQ(mlsdk::vgf_dump::getFile(vgfName), nlohmann::json::parse(expectedJson));

    // Open spv file
    std::vector<uint32_t> spvData;
    mlsdk::vgf_dump::getSpirv(vgfName, 0, [&spvData](const uint32_t* data, size_t size) {
        spvData.assign(data, data + size);
    });

    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(testutils::DisassembleSPIRV(spvData, true));
    const auto module0 = testutils::LoadSPIRVDisassembly(outputStr);
    const auto module1 = testutils::LoadSPIRVDisassembly(spirvmodels::SimpleConv2d);
    // Check as expected
    const auto diff = testutils::CompareModules(module1, module0);

    EXPECT_TRUE(diff.empty());

    std::vector<uint8_t> weightData;
    mlsdk::vgf_dump::getConstant(vgfName, 0, [&weightData](const uint8_t* data, size_t size) {
        weightData.assign(data, data + size);
    });
    const std::vector<int8_t> expectedWeightData{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(std::equal(weightData.begin(), weightData.begin() + weightData.size(), expectedWeightData.begin()), 1);

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
    tosa2spirv::parsers::TosaSerializationParser parser(&block);

    std::string modelName = "quantized_rescale_single_layer";
    std::string vgfDir = std::getenv("VGF_OUTPUT_PATH") ? std::getenv("VGF_OUTPUT_PATH") : ".";
    std::string vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto module = parser.GenerateSPIRVModule("main");
    auto spirv = tosa2spirv::WriteToBinary(module);
    EXPECT_FALSE(spirv.empty());

    auto constants = parser.GetExternalConstants();

    tosa2spirv::vgfwriter::WriteVGF(module,
                                    tosa2spirv::parsers::ConvertConstantDataToVoid(std::move(constants)),
                                    modelName,
                                    ".",
                                    vgfName);

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
            "code_size": 194,
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
            "vk_descriptor_type": "VK_DESCRIPTOR_TYPE_TENSOR_ARM",
            "vk_format": "VK_FORMAT_R8_SINT"
        },
        {
            "category": "OUTPUT",
            "index": 3,
            "shape": [1, 1, 4, 4],
            "stride": [],
            "vk_descriptor_type": "VK_DESCRIPTOR_TYPE_TENSOR_ARM",
            "vk_format": "VK_FORMAT_R8_SINT"
        }
    ]
})";

    EXPECT_EQ(mlsdk::vgf_dump::getFile(vgfName), nlohmann::json::parse(expectedJson));

    // Open spv file
    std::vector<uint32_t> spvData;
    mlsdk::vgf_dump::getSpirv(vgfName, 0, [&spvData](const uint32_t* data, size_t size) {
        spvData.assign(data, data + size);
    });

    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(testutils::DisassembleSPIRV(spvData, true));

    // Check as expected
    testutils::CompareModules(module, spirvmodels::simpleRescale);
    const std::vector<int32_t> expectedMultData{60, 60, 60, 60};
    const std::vector<int8_t> expectedShiftData{30, 30, 30, 30};

    std::vector<uint8_t> multData;
    mlsdk::vgf_dump::getConstant(vgfName, 0, [&multData](const uint8_t* data, size_t size) {
        multData.assign(data, data + size);
    });

    // Ensure vect is multiple of 4
    EXPECT_EQ(multData.size() % 4, 0);

    // Build up the int32 vector again from uint8
    std::vector<int32_t> resultMultVect;
    for (size_t i = 0; i < multData.size(); i += 4)
    {
        int32_t value = 0;
        value |= static_cast<int32_t>(multData[i]) << 0;
        value |= static_cast<int32_t>(multData[i + 1]) << 8;
        value |= static_cast<int32_t>(multData[i + 2]) << 16;
        value |= static_cast<int32_t>(multData[i + 3]) << 24;
        resultMultVect.push_back(value);
    }
    EXPECT_EQ(
        std::equal(resultMultVect.begin(), resultMultVect.begin() + resultMultVect.size(), expectedMultData.begin()),
        1);

    std::vector<uint8_t> shiftData;
    mlsdk::vgf_dump::getConstant(vgfName, 1, [&shiftData](const uint8_t* data, size_t size) {
        shiftData.assign(data, data + size);
    });

    EXPECT_EQ(std::equal(shiftData.begin(), shiftData.begin() + shiftData.size(), expectedShiftData.begin()), 1);

    // Delete the .vgf
    if (std::remove(vgfName.c_str()) != 0)
    {
        FAIL() << "Error deleting vgf file.";
    }
}
