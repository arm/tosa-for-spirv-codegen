//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <AssemblyUtils.hpp>
#include <TosaSerializationParser.hpp>
#include <VgfWriter.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <gtest/gtest.h>
#include <spirvmodels/Conv2d.hpp>
#include <spirvmodels/MaxPool.hpp>
#include <spirvmodels/Rescale.hpp>
#include <vgf/decoder.hpp>
#include <vgf/vgf_dump.hpp>

#include <fstream>

using namespace tosa;

std::unique_ptr<TosaSerializationBasicBlock> GeneratorSimpleMaxpool2dModel()
{
    // Create Attribute
    DType dType = DType::DType_INT8;
    std::vector<int32_t> kernel = {2, 2};
    std::vector<int32_t> stride = {2, 2};
    std::vector<int32_t> pad = {1, 1, 1, 1};
    int32_t nan_mode = 1;
    TosaMaxPool2dAttribute attribute(kernel, stride, pad, static_cast<NanPropagationMode>(nan_mode));

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_MAX_POOL2D,
                                                                Attribute::Attribute_MaxPool2dAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    auto block = std::make_unique<TosaSerializationBasicBlock>("max_pool2d",
                                                               "main",
                                                               std::move(ops),
                                                               std::move(tensors),
                                                               std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                                               std::vector<std::string>{inputName},
                                                               std::vector<std::string>{outputName});
    return std::move(block);
}

// CONV 2D
std::unique_ptr<TosaSerializationBasicBlock> GeneratorSimpleConv2dModel()
{
    // Create Attribute
    std::vector<int32_t> pad = {1, 1, 1, 1};
    std::vector<int32_t> dilation = {1, 1};
    std::vector<int32_t> stride = {2, 2};
    int32_t acc_type = 1;
    bool local_bound = true;
    TosaConv2dAttribute attribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string inputName = "input1";
    std::string weightName = "weights";
    std::string biasName = "bias";
    std::string outputName = "output1";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::vector<int32_t> inputShape = {1, 5, 5, 1};
    std::vector<int32_t> weightShape = {1, 3, 6, 1};
    std::vector<int32_t> biasShape = {1, 3, 6, 1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));

    auto weightTensor = std::make_unique<TosaSerializationTensor>(
        weightName,
        weightShape,
        DType::DType_INT8,
        std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18});
    tensors.push_back(std::move(weightTensor));

    auto biasTensor = std::make_unique<TosaSerializationTensor>(
        biasName,
        biasShape,
        DType::DType_INT32,
        std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18});
    tensors.push_back(std::move(biasTensor));

    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto weightOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{weightName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(weightOp));

    auto biasOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                    Attribute::Attribute_NONE,
                                                                    nullptr,
                                                                    std::vector<std::string>{},
                                                                    std::vector<std::string>{biasName},
                                                                    TosaOpLocation{});
    ops.push_back(std::move(biasOp));

    auto input_zpTensor = std::make_unique<TosaSerializationTensor>(input_zpName,
                                                                    input_zpShape,
                                                                    DType::DType_INT8,
                                                                    std::vector<uint8_t>{1});
    tensors.push_back(std::move(input_zpTensor));

    auto input_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                        Attribute::Attribute_NONE,
                                                                        nullptr,
                                                                        std::vector<std::string>{},
                                                                        std::vector<std::string>{input_zpName},
                                                                        TosaOpLocation{});
    ops.push_back(std::move(input_zpOp));

    auto weight_zpTensor = std::make_unique<TosaSerializationTensor>(weight_zpName,
                                                                     weight_zpShape,
                                                                     DType::DType_INT8,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(weight_zpTensor));

    auto weight_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{weight_zpName},
                                                                         TosaOpLocation{});
    ops.push_back(std::move(weight_zpOp));

    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &attribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    auto block = std::make_unique<TosaSerializationBasicBlock>("max_pool2d",
                                                               "main",
                                                               std::move(ops),
                                                               std::move(tensors),
                                                               std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                                               std::vector<std::string>{inputName},
                                                               std::vector<std::string>{outputName});

    return std::move(block);
}

// Rescale
std::unique_ptr<TosaSerializationBasicBlock> GeneratorSimpleRescaleModel()
{
    // Create Attribute
    std::vector<int8_t> input_zp = {20};
    std::vector<int8_t> output_zp = {-128};
    std::vector<int32_t> multiplier = {60, 60, 60, 60};
    std::vector<int32_t> shift = {30, 30, 30, 30};

    bool scale32 = true;
    int32_t rounding_mode = 1;
    bool per_channel = true;
    bool input_unsigned = true;
    bool output_unsigned = true;
    // Create Attribute
    TosaRescaleAttribute attribute(scale32,
                                   static_cast<const RoundingMode>(rounding_mode),
                                   per_channel,
                                   input_unsigned,
                                   output_unsigned);

    // Create Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string inputName = "input1";
    std::string multiplierName = "multiplier";
    std::string shiftName = "shift";
    std::string input_zpName = "input_zp";
    std::string output_zpName = "output_zp";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> multiplierShape = {18};
    std::vector<int32_t> shiftShape = {18};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> output_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 4, 4};

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    auto multiplierTensor = std::make_unique<TosaSerializationTensor>(
        multiplierName,
        multiplierShape,
        DType::DType_INT32,
        std::vector<uint8_t>{{0, 0, 0, 1,  0, 0, 0, 2,  0, 0, 0, 3,  0, 0, 0, 4,  0, 0, 0, 5,  0, 0, 0, 6,
                              0, 0, 0, 7,  0, 0, 0, 8,  0, 0, 0, 9,  0, 0, 0, 10, 0, 0, 0, 11, 0, 0, 0, 12,
                              0, 0, 0, 13, 0, 0, 0, 14, 0, 0, 0, 15, 0, 0, 0, 16, 0, 0, 0, 17, 0, 0, 0, 18}});
    tensors.push_back(std::move(multiplierTensor));
    auto multiplierOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                          Attribute::Attribute_NONE,
                                                                          nullptr,
                                                                          std::vector<std::string>{},
                                                                          std::vector<std::string>{multiplierName},
                                                                          TosaOpLocation{});
    ops.push_back(std::move(multiplierOp));
    auto shiftTensor = std::make_unique<TosaSerializationTensor>(
        shiftName,
        shiftShape,
        DType::DType_INT8,
        std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18});
    tensors.push_back(std::move(shiftTensor));
    auto shiftOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{shiftName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(shiftOp));
    auto input_zpTensor = std::make_unique<TosaSerializationTensor>(input_zpName,
                                                                    input_zpShape,
                                                                    DType::DType_INT8,
                                                                    std::vector<uint8_t>{1});
    tensors.push_back(std::move(input_zpTensor));
    auto input_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                        Attribute::Attribute_NONE,
                                                                        nullptr,
                                                                        std::vector<std::string>{},
                                                                        std::vector<std::string>{input_zpName},
                                                                        TosaOpLocation{});
    ops.push_back(std::move(input_zpOp));
    auto output_zpTensor = std::make_unique<TosaSerializationTensor>(output_zpName,
                                                                     output_zpShape,
                                                                     DType::DType_INT8,
                                                                     std::vector<uint8_t>{1});
    tensors.push_back(std::move(output_zpTensor));
    auto output_zpOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                         Attribute::Attribute_NONE,
                                                                         nullptr,
                                                                         std::vector<std::string>{},
                                                                         std::vector<std::string>{output_zpName},
                                                                         TosaOpLocation{});
    ops.push_back(std::move(output_zpOp));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_RESCALE,
        Attribute::Attribute_RescaleAttribute,
        &attribute,
        std::vector<std::string>{inputName, multiplierName, shiftName, input_zpName, output_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(op));
    // Create a tosa single-op basic block
    auto block = std::make_unique<TosaSerializationBasicBlock>("rescale",
                                                               "main",
                                                               std::move(ops),
                                                               std::move(tensors),
                                                               std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                                               std::vector<std::string>{inputName},
                                                               std::vector<std::string>{outputName});

    return std::move(block);
}

TEST(TOSA_FOR_SPIRV_CODEGEN_GENERATOR, GeneratorTestExampleTosaFile)
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
        tosaFile = dir_path + "../../../examples/simple_maxpool2d.tosa";
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
    auto parser = tfsc::parsers::TosaSerializationParser(mainBlock);

    // Create SPIRV
    const auto module1 = parser.GenerateSPIRVModule("main");


    std::vector<uint32_t> spirv = tfsc::WriteToBinary(module1);
    std::string outputStr(testutils::DisassembleSPIRV(spirv, true));

    EXPECT_EQ(outputStr, spirvmodels::SimpleMaxpool2dGenerator);
}

TEST(TOSA_FOR_SPIRV_CODEGEN_GENERATOR, GeneratorTestExampleJsonFile)
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
        jsonFile = dir_path + "../../../examples/simple_maxpool2d.json";
    }
    std::ifstream fileStream;
    fileStream.open(jsonFile);

    auto companion = nlohmann::json::parse(fileStream);

    std::string expectedJson = nlohmann::json::parse(R"json(
        {
          "version": {"_major":1,"_minor":0,"_patch":0,"_draft":false},
          "regions": [{
            "name":"main",
            "blocks":[{
              "name":"main",
              "operators":[{
                "op":"MAX_POOL2D",
                "attribute_type":"MaxPool2dAttribute",
                "attribute":{"kernel":[2,2],"stride":[2,2],"pad":[0,0,0,0],"nan_mode":"PROPAGATE"},
                "inputs":["TosaInput_0"],
                "outputs":["TosaOutput_0"],
                "location":{"text":"loc(unknown)"} } ],
              "tensors":[
                {"name":"TosaInput_0","shape":[1,16,16,16],"type":"INT8","data":[],"variable":false,"is_unranked":false,"variable_name":""},
                {"name":"TosaOutput_0","shape":[1,8,8,16],"type":"INT8","data":[],"variable":false,"is_unranked":false,"variable_name":""}],
              "inputs":["TosaInput_0"],"outputs":["TosaOutput_0"],"shapes":[] }]}]
        }
    )json").dump();

    EXPECT_EQ(companion.dump(), expectedJson);
}

static bool fileExists(const char* filename)
{
    const std::ifstream file(filename);
    return file.good();
}

TEST(TOSA_FOR_SPIRV_CODEGEN_GENERATOR, GeneratorWriteVgfFile)
{
    auto block = GeneratorSimpleMaxpool2dModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    tfsc::parsers::TosaSerializationParser parser(block.get());

    std::string modelName = "quantized_max_pool_single_layer";
    std::string vgfDir = std::getenv("VGF_OUTPUT_PATH") ? std::getenv("VGF_OUTPUT_PATH") : ".";
    std::string vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto module = parser.GenerateSPIRVModule("main");
    auto spirv = tfsc::WriteToBinary(module);
    auto constants = parser.GetExternalConstants();

    tfsc::vgfwriter::WriteVGF(module,
                                    tfsc::parsers::ConvertConstantDataToVoid(std::move(constants)),
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
                        "minor": 4,
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
                "code_size": 201,
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
    mlsdk::vgf_dump::getSpirv(vgfName,
                              0,
                              [&spvData](const uint32_t* data, size_t size) { spvData.assign(data, data + size); });

    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(testutils::DisassembleSPIRV(spvData, true));
    EXPECT_EQ(outputStr, spirvmodels::SimpleMaxpool2d);

    // Delete the .vgf
    if (std::remove(vgfName.c_str()) != 0)
    {
        FAIL() << "Error deleting vgf file.";
    }
}

TEST(TOSA_FOR_SPIRV_CODEGEN_GENERATOR, GeneratorWriteNpyFiles)
{
    // Create a simple MaxPool2d model block.
    auto block = GeneratorSimpleConv2dModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    tfsc::parsers::TosaSerializationParser parser(block.get());

    std::string modelName = "quantized_conv2d_single_layer";
    std::string vgfDir = std::getenv("VGF_OUTPUT_PATH") ? std::getenv("VGF_OUTPUT_PATH") : ".";
    std::string vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto module = parser.GenerateSPIRVModule("main");
    auto spirv = tfsc::WriteToBinary(module);
    auto constants = parser.GetExternalConstants();

    tfsc::vgfwriter::WriteVGF(module,
                                    tfsc::parsers::ConvertConstantDataToVoid(std::move(constants)),
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
        },
        {
            "index": 1,
                    "mrt_index": 1,
                    "sparsity_dimension": -1
        }
        ],
        "header": {
            "major": 0,
                    "minor": 4,
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
            "code_size": 265,
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
                    "shape": [1, 3, 6, 1],
            "stride": [],
                    "vk_descriptor_type": "none",
                    "vk_format": "VK_FORMAT_R8_SINT"
        },
        {
            "category": "CONSTANT",
                    "index": 1,
                    "shape": [1, 3, 6, 1],
            "stride": [],
                    "vk_descriptor_type": "none",
                    "vk_format": "VK_FORMAT_R32_SINT"
        },
        {
            "category": "INPUT",
                    "index": 2,
                    "shape": [1, 5, 5, 1],
            "stride": [],
                    "vk_descriptor_type": "VK_DESCRIPTOR_TYPE_TENSOR_ARM",
                    "vk_format": "VK_FORMAT_R8_SINT"
        },
        {
            "category": "OUTPUT",
                    "index": 3,
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
    mlsdk::vgf_dump::getSpirv(vgfName,
                              0,
                              [&spvData](const uint32_t* data, size_t size) { spvData.assign(data, data + size); });

    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(testutils::DisassembleSPIRV(spvData, true));
    const auto module0 = testutils::LoadSPIRVDisassembly(outputStr);
    const auto module1 = testutils::LoadSPIRVDisassembly(spirvmodels::SimpleConv2d);
    EXPECT_EQ(module1, module0);

    std::vector<uint8_t> weightData;
    mlsdk::vgf_dump::getConstant(vgfName,
                                 0,
                                 [&weightData](const uint8_t* data, size_t size)
                                 { weightData.assign(data, data + size); });
    const std::vector<int8_t> expectedWeightData{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
    EXPECT_EQ(std::equal(weightData.begin(), weightData.begin() + weightData.size(), expectedWeightData.begin()), 1);

    std::vector<uint8_t> biasDataVector;
    mlsdk::vgf_dump::getConstant(vgfName,
                                 1,
                                 [&biasDataVector](const uint8_t* data, size_t size)
                                 { biasDataVector.assign(data, data + size); });
    uint8_t biasData = biasDataVector[0];
    // Bias was added as int32 to cast it as such
    auto castBiasData = static_cast<int32_t>(biasData);
    int32_t expectedBiasData{1};
    EXPECT_EQ(castBiasData, expectedBiasData);

    // Delete the .vgf
    if (std::remove(vgfName.c_str()) != 0)
    {
        FAIL() << "Error deleting vgf file.";
    }
}

TEST(TOSA_FOR_SPIRV_CODEGEN_GENERATOR, GeneratorWriteNpyFilesRescale)
{
    // Create a simple MaxPool2d model block.
    auto block = GeneratorSimpleRescaleModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    tfsc::parsers::TosaSerializationParser parser(block.get());

    std::string modelName = "quantized_rescale_single_layer";
    std::string vgfDir = std::getenv("VGF_OUTPUT_PATH") ? std::getenv("VGF_OUTPUT_PATH") : ".";
    std::string vgfName = modelName + ".vgf";

    //  Write the vgf file.
    auto module = parser.GenerateSPIRVModule("main");
    auto spirv = tfsc::WriteToBinary(module);
    EXPECT_FALSE(spirv.empty());

    auto constants = parser.GetExternalConstants();

    tfsc::vgfwriter::WriteVGF(module,
                                    tfsc::parsers::ConvertConstantDataToVoid(std::move(constants)),
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
        "minor": 4,
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
            "code_size": 204,
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
            "shape": [18],
            "stride": [],
            "vk_descriptor_type": "none",
            "vk_format": "VK_FORMAT_R32_SINT"
        },
        {
            "category": "CONSTANT",
            "index": 1,
            "shape": [18],
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
    mlsdk::vgf_dump::getSpirv(vgfName,
                              0,
                              [&spvData](const uint32_t* data, size_t size) { spvData.assign(data, data + size); });

    if (spvData.empty())
    {
        FAIL() << "GeneratorWriteVgfFile: Invalid Spirv.";
    }

    // Disassemble the Spir-V and validate
    const std::string outputStr(testutils::DisassembleSPIRV(spvData, true));

    // Check as expected
    EXPECT_EQ(spirvmodels::simpleRescale, outputStr);
    const std::vector<int32_t> expectedMultData{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
    const std::vector<int8_t> expectedShiftData{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

    std::vector<uint8_t> multData;
    mlsdk::vgf_dump::getConstant(vgfName,
                                 0,
                                 [&multData](const uint8_t* data, size_t size) { multData.assign(data, data + size); });

    // Ensure vect is multiple of 4
    EXPECT_EQ(multData.size() % 4, 0);

    // Build up the int32 vector again from uint8
    std::vector<int32_t> resultMultVect;
    for (size_t i = 0; i < multData.size(); i += 4)
    {
        int32_t value = (static_cast<int32_t>(multData[i + 0]) << 24) | // MSB
                        (static_cast<int32_t>(multData[i + 1]) << 16) | (static_cast<int32_t>(multData[i + 2]) << 8) |
                        (static_cast<int32_t>(multData[i + 3]) << 0); // LSB
        resultMultVect.push_back(value);
    }
    EXPECT_EQ(
        std::equal(resultMultVect.begin(), resultMultVect.begin() + resultMultVect.size(), expectedMultData.begin()),
        1);

    std::vector<uint8_t> shiftData;
    mlsdk::vgf_dump::getConstant(vgfName,
                                 1,
                                 [&shiftData](const uint8_t* data, size_t size)
                                 { shiftData.assign(data, data + size); });

    EXPECT_EQ(std::equal(shiftData.begin(), shiftData.begin() + shiftData.size(), expectedShiftData.begin()), 1);

    // Delete the .vgf
    if (std::remove(vgfName.c_str()) != 0)
    {
        FAIL() << "Error deleting vgf file.";
    }
}
