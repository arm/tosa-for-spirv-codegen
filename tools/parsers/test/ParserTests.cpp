//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <OpTestUtils.hpp>
#include <TosaSerializationParser.hpp>

#include <AssemblyUtils.hpp>
#include <spirvmodels/Add.hpp>
#include <spirvmodels/Conv2d.hpp>
#include <spirvmodels/MaxPool.hpp>

#include <tosa_serialization_handler.h>

#include <gtest/gtest.h>

using namespace ::tosa;
using namespace tfsc::parsers;

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, ConstInputOpTest)
{
    // Create Conv2d Attribute
    std::vector<int> pad = {0, 0, 0, 0};
    std::vector<int> stride = {2, 2};
    std::vector<int> dilation = {1, 1};
    bool local_bound = true;
    TosaConv2dAttribute conv2dAttribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Rescale Attribute
    bool scale32 = true;
    int32_t rounding_mode = 1;
    bool per_channel = true;
    bool input_unsigned = true;
    bool output_unsigned = true;
    std::vector<int> multiplier = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> shift = {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42};
    TosaRescaleAttribute rescaleAttribute(scale32,
                                          static_cast<const RoundingMode>(rounding_mode),
                                          per_channel,
                                          input_unsigned,
                                          output_unsigned);

    // Create Tensor Shapes
    std::vector<int32_t> inputShape = {1, 1, 1, 1};
    std::vector<int32_t> weightShape = {1, 1, 1, 1};
    std::vector<int32_t> biasShape = {1, 1, 1, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};
    std::vector<int32_t> rescaleShape = {1, 8, 8, 16};

    std::vector<int32_t> multiplierShape = {1};
    std::vector<int32_t> shiftShape = {1};
    std::vector<int32_t> input_zpRescaleShape = {1};
    std::vector<int32_t> output_zpShape = {1};

    // Create Tosa Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string outputName = "output";
    std::string rescaleIntermediateName = "layer_intermediate0_4";
    std::string multiplierName = "multiplier";
    std::string shiftName = "shift";
    std::string input_zpRescaleName = "input_zp_rescale";
    std::string output_zpName = "output_zp";

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto weightTensor = std::make_unique<TosaSerializationTensor>(
        weightName,
        weightShape,
        DType::DType_INT8,
        std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    tensors.push_back(std::move(weightTensor));
    auto weightOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{weightName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(weightOp));

    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName,
                                                  biasShape,
                                                  DType::DType_INT32,
                                                  std::vector<uint8_t>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    tensors.push_back(std::move(biasTensor));
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

    auto rescaleTensor = std::make_unique<TosaSerializationTensor>(rescaleIntermediateName,
                                                                   rescaleShape,
                                                                   DType::DType_INT32,
                                                                   std::vector<uint8_t>{});
    tensors.push_back(std::move(rescaleTensor));

    auto multiplierTensor = std::make_unique<TosaSerializationTensor>(multiplierName,
                                                                      multiplierShape,
                                                                      DType::DType_INT32,
                                                                      std::vector<uint8_t>{{0, 0, 0, 1}});
    tensors.push_back(std::move(multiplierTensor));

    auto multiplierOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                          Attribute::Attribute_NONE,
                                                                          nullptr,
                                                                          std::vector<std::string>{},
                                                                          std::vector<std::string>{multiplierName},
                                                                          TosaOpLocation{});
    ops.push_back(std::move(multiplierOp));

    auto shiftTensor =
        std::make_unique<TosaSerializationTensor>(shiftName, shiftShape, DType::DType_INT8, std::vector<uint8_t>{1});
    tensors.push_back(std::move(shiftTensor));

    auto shiftOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{shiftName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(shiftOp));

    auto input_zpRescaleTensor = std::make_unique<TosaSerializationTensor>(input_zpRescaleName,
                                                                           input_zpShape,
                                                                           DType::DType_INT32,
                                                                           std::vector<uint8_t>{0, 0, 0, 1});
    tensors.push_back(std::move(input_zpRescaleTensor));

    auto input_zpRescaleOp =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{},
                                                          std::vector<std::string>{input_zpRescaleName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(input_zpRescaleOp));

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

    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    auto conv2d = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{rescaleIntermediateName},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d));

    // Create Rescale Operator
    auto rescaleOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_RESCALE,
                                                                       Attribute::Attribute_RescaleAttribute,
                                                                       &rescaleAttribute,
                                                                       std::vector<std::string>{rescaleIntermediateName,
                                                                                                multiplierName,
                                                                                                shiftName,
                                                                                                input_zpRescaleName,
                                                                                                output_zpName},
                                                                       std::vector<std::string>{outputName},
                                                                       TosaOpLocation{});
    ops.push_back(std::move(rescaleOp));

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock block("main",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                      {inputName},
                                      {outputName});

    TosaSerializationParser parser(&block);
    auto binarySpirv = parser.GenerateSPIRV("main");
    std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));
    EXPECT_TRUE(!outputStr.empty()) << "Disassembled SPIR-V is empty when it shouldn't be.";
    ;

    EXPECT_TRUE(!binarySpirv.empty()) << "BinarySpirv is empty when it shouldn't be.";
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, UnsupportedOperator)
{
    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_UNKNOWN,
                                                                Attribute::Attribute_NONE,
                                                                nullptr,
                                                                std::vector<std::string>{},
                                                                std::vector<std::string>{},
                                                                TosaOpLocation{});

    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    // The raw pointers of operators will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("unknown", "main", std::move(ops), {}, std::vector<std::unique_ptr<TosaSerializationShape>>{}, {}, {});

    TosaSerializationParser parser(&block);

    // Pass an unsupport operator to the parser
    // Text of exception is: ERROR: Operator Op_UNKNOWN(0) is currently unsupported
    EXPECT_ANY_THROW(parser.GenerateSPIRV("main"));
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, InvalidDtype)
{
    // Create Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> kernel = {2, 2};
    std::vector<int> stride = {2, 2};
    int32_t nan_mode = 1;
    DType dType = DType::DType_UNKNOWN;
    TosaMaxPool2dAttribute attribute(kernel, stride, pad, static_cast<NanPropagationMode>(nan_mode));

    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    auto inputTensor = std::make_unique<TosaSerializationTensor>(inputName, inputShape, dType, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, dType, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_MAX_POOL2D,
                                                                Attribute::Attribute_MaxPool2dAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});

    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    ops.push_back(std::move(op));

    // Create a tosa single-op basic block
    TosaSerializationBasicBlock block("unknown",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                      {inputName},
                                      {outputName});

    TosaSerializationParser parser(&block);

    // Pass an unsupport DType to the parser
    // Text of exception is: ERROR: No Corresponding DataType for DType UNKNOWN (0).
    EXPECT_ANY_THROW(parser.GenerateSPIRV("main"));
}

// MAX POOL
std::unique_ptr<TosaSerializationBasicBlock> ParserGeneratorSimpleMaxpool2dModel()
{
    // Create Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> kernel = {2, 2};
    std::vector<int> stride = {2, 2};
    int32_t nan_mode = 1;
    DType dType = DType::DType_INT8;
    TosaMaxPool2dAttribute attribute(kernel, stride, pad, static_cast<NanPropagationMode>(nan_mode));

    // Create Tensors
    std::string inputName = "input1";
    std::string outputName = "output1";
    std::vector<int32_t> inputShape = {1, 1, 4, 4};
    std::vector<int32_t> outputShape = {1, 1, 3, 3};

    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    auto inputTensor = std::make_unique<TosaSerializationTensor>(inputName, inputShape, dType, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, dType, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Operator
    auto op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_MAX_POOL2D,
                                                                Attribute::Attribute_MaxPool2dAttribute,
                                                                &attribute,
                                                                std::vector<std::string>{inputName},
                                                                std::vector<std::string>{outputName},
                                                                TosaOpLocation{});
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
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

// Simple Parser API usage test referenced in the README.md
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Readme)
{
    // Create a simple MaxPool2d model block.
    auto block = ParserGeneratorSimpleMaxpool2dModel();

    // Construct TosaSerializationParser object using TosaSerializationBlock or TosaSerializationHandler
    TosaSerializationParser parser(block.get());

    // Call GenerateSPIRV, which returns a SPIR-V binary vector.
    // This can then be disassembled into human-readable SPIR-V or passed for execution.
    const auto module = parser.GenerateSPIRVModule("main");

    const auto binary0= tfsc::WriteToBinary(module);
    const auto actualText= testutils::DisassembleSPIRV(binary0, /*friendlyNames=*/false);
    const auto actualBinary= tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(actualText));
    const auto expectedBinary= tfsc::WriteToBinary(
                                        testutils::LoadSPIRVDisassembly(spirvmodels::SimpleMaxpool2d));
    EXPECT_EQ(actualBinary, expectedBinary);
}

// Multi Layer Conv2d reusing the weight and bias GraphConstantId
std::unique_ptr<TosaSerializationBasicBlock> GeneratorConv2DRescaleConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> stride = {2, 2};
    std::vector<int> dilation = {1, 1};
    bool local_bound = true;
    TosaConv2dAttribute conv2dAttribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Rescale Attribute
    bool scale32 = true;
    int32_t rounding_mode = 1;
    bool per_channel = true;
    bool input_unsigned = true;
    bool output_unsigned = true;
    std::vector<int> multiplier = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> shift = {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42};
    TosaRescaleAttribute rescaleAttribute(scale32,
                                          static_cast<const RoundingMode>(rounding_mode),
                                          per_channel,
                                          input_unsigned,
                                          output_unsigned);

    // Create Tensors
    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> weightShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> rescaleInShape = {1, 5, 5, 1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<int32_t> multiplierShape = {1};
    std::vector<int32_t> shiftShape = {1};
    std::vector<int32_t> input_zpRescaleShape = {1};
    std::vector<int32_t> output_zpShape = {1};

    // Create Tosa Tensors
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;
    std::string inputName = "input";
    std::string weightName = "weight";
    std::string biasName = "bias";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string rescaleOutName = "output";
    std::string output2Name = "output2";
    std::string rescaleIntermediateName = "layer_intermediate0_4";
    std::string multiplierName = "multiplier";
    std::string shiftName = "shift";
    std::string input_zpRescaleName = "input_zp_rescale";
    std::string output_zpName = "output_zp";

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto weightTensor = std::make_unique<TosaSerializationTensor>(
        weightName,
        weightShape,
        DType::DType_INT8,
        std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    tensors.push_back(std::move(weightTensor));
    auto weightOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{weightName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(weightOp));

    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName,
                                                  biasShape,
                                                  DType::DType_INT32,
                                                  std::vector<uint8_t>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    tensors.push_back(std::move(biasTensor));
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

    auto rescaleTensor = std::make_unique<TosaSerializationTensor>(rescaleIntermediateName,
                                                                   rescaleInShape,
                                                                   DType::DType_INT32,
                                                                   std::vector<uint8_t>{});
    tensors.push_back(std::move(rescaleTensor));

    auto multiplierTensor = std::make_unique<TosaSerializationTensor>(multiplierName,
                                                                      multiplierShape,
                                                                      DType::DType_INT32,
                                                                      std::vector<uint8_t>{{0, 0, 0, 1}});
    tensors.push_back(std::move(multiplierTensor));

    auto multiplierOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                          Attribute::Attribute_NONE,
                                                                          nullptr,
                                                                          std::vector<std::string>{},
                                                                          std::vector<std::string>{multiplierName},
                                                                          TosaOpLocation{});
    ops.push_back(std::move(multiplierOp));

    auto shiftTensor =
        std::make_unique<TosaSerializationTensor>(shiftName, shiftShape, DType::DType_INT8, std::vector<uint8_t>{1});
    tensors.push_back(std::move(shiftTensor));

    auto shiftOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{shiftName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(shiftOp));

    auto input_zpRescaleTensor = std::make_unique<TosaSerializationTensor>(input_zpRescaleName,
                                                                           input_zpShape,
                                                                           DType::DType_INT32,
                                                                           std::vector<uint8_t>{0, 0, 0, 1});
    tensors.push_back(std::move(input_zpRescaleTensor));

    auto input_zpRescaleOp =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{},
                                                          std::vector<std::string>{input_zpRescaleName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(input_zpRescaleOp));

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

    auto rescaleOutTensor = std::make_unique<TosaSerializationTensor>(rescaleOutName,
                                                                      outputShape,
                                                                      DType::DType_INT8,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(rescaleOutTensor));

    auto outputTensor2 =
        std::make_unique<TosaSerializationTensor>(output2Name, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor2));

    auto conv2d = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{rescaleIntermediateName},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d));

    // Create Rescale Operator
    auto rescaleOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_RESCALE,
                                                                       Attribute::Attribute_RescaleAttribute,
                                                                       &rescaleAttribute,
                                                                       std::vector<std::string>{rescaleIntermediateName,
                                                                                                multiplierName,
                                                                                                shiftName,
                                                                                                input_zpRescaleName,
                                                                                                output_zpName},
                                                                       std::vector<std::string>{rescaleOutName},
                                                                       TosaOpLocation{});
    ops.push_back(std::move(rescaleOp));

    auto conv2d2 = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{rescaleOutName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{output2Name},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d2));

    // Create a tosa single-op basic block
    auto block = std::make_unique<TosaSerializationBasicBlock>("main",
                                                               "main",
                                                               std::move(ops),
                                                               std::move(tensors),
                                                               std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                                               std::vector<std::string>{inputName},
                                                               std::vector<std::string>{output2Name});

    return std::move(block);
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Conv2DRescaleConv2D)
{
    // Create a model with 2 Conv2D operators which reuse the same weight and bias GraphConstantId
    auto block = GeneratorConv2DRescaleConv2DModel();

    TosaSerializationParser parser(block.get());

    const auto module1 = parser.GenerateSPIRVModule("main");
    const auto text1 = testutils::DisassembleSPIRV(tfsc::WriteToBinary(module1), false);
    const auto text2 = spirvmodels::Conv2DRescaleConv2D;

    const auto actualBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text1));
    const auto expectedBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text2));

    EXPECT_EQ(actualBinary, expectedBinary);
}

// Invalid block with graph input and output directly connected via IdentityOps
// (input) - IDENTITY - IDENTITY - (output1) - CONV2D - (output2)
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, InvalidBlockInputOutputConnectWithIdentity)
{
    // Create Conv2D Attribute
    std::vector<int> pad = {0, 0, 0, 0};
    std::vector<int> stride = {2, 2};
    std::vector<int> dilation = {1, 1};
    bool local_bound = true;
    TosaConv2dAttribute conv2dAttribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Tensors
    std::string inputName = "input1";
    std::string output1Name = "output1";
    std::string output2Name = "output2";
    std::string identityInterTensorName = "output2";
    std::string weightName = "weights";
    std::string biasName = "bias";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> output1Shape = {1, 7, 7, 1};
    std::vector<int32_t> output2Shape = {1, 3, 3, 1};
    std::vector<int32_t> weightShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto weightTensor = std::make_unique<TosaSerializationTensor>(
        weightName,
        weightShape,
        DType::DType_INT8,
        std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    tensors.push_back(std::move(weightTensor));
    auto weightOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{weightName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(weightOp));

    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName,
                                                  biasShape,
                                                  DType::DType_INT32,
                                                  std::vector<uint8_t>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    tensors.push_back(std::move(biasTensor));
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
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(output1Name, output1Shape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));
    auto output2Tensor = std::make_unique<TosaSerializationTensor>(identityInterTensorName,
                                                                   output2Shape,
                                                                   DType::DType_INT8,
                                                                   std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create Identity Operators
    auto identity1 =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_IDENTITY,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{inputName},
                                                          std::vector<std::string>{identityInterTensorName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(identity1));
    auto identity2 =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_IDENTITY,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{inputName},
                                                          std::vector<std::string>{identityInterTensorName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(identity2));

    // Create Operators Conv2D
    auto conv2d = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{output1Name, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{output2Name},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d));

    // Create a tosa single-op basic block
    TosaSerializationBasicBlock block("identity",
                                      "main",
                                      std::move(ops),
                                      std::move(tensors),
                                      std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                      {inputName},
                                      {output1Name, output2Name});

    TosaSerializationParser parser(&block);

    // Pass an invalid IdentityOp only block to the parser
    // Text of exception is:
    // TosaSerializationParser: Graph input/output should not be connected directly with Identity Op!
    EXPECT_ANY_THROW(parser.GenerateSPIRV({}));
}

// Multi Layer IDENTITY - CONV2D - IDENTITY
std::unique_ptr<TosaSerializationBasicBlock> GeneratorIdentityConv2DIdentityModel()
{
    // Create Conv2D Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> stride = {2, 2};
    std::vector<int> dilation = {1, 1};
    bool local_bound = true;
    TosaConv2dAttribute conv2dAttribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Tensors
    std::string identity1InputName = "identity1_input";
    std::string identity1OutputName = "identity1_output";
    std::string conv2DWeightsName = "weights";
    std::string conv2DBiasName = "bias";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string identity2InputName = "identity2_input";
    std::string identity2OutputName = "identity2_output";

    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> weightsShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;

    auto identity1InputTensor = std::make_unique<TosaSerializationTensor>(identity1InputName,
                                                                          inputShape,
                                                                          DType::DType_INT8,
                                                                          std::vector<uint8_t>{});
    tensors.push_back(std::move(identity1InputTensor));
    auto identity1OutputTensor = std::make_unique<TosaSerializationTensor>(identity1OutputName,
                                                                           inputShape,
                                                                           DType::DType_INT8,
                                                                           std::vector<uint8_t>{});
    tensors.push_back(std::move(identity1OutputTensor));

    auto weightTensor = std::make_unique<TosaSerializationTensor>(
        conv2DWeightsName,
        weightsShape,
        DType::DType_INT8,
        std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    tensors.push_back(std::move(weightTensor));
    auto weightOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{conv2DWeightsName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(weightOp));
    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(conv2DBiasName,
                                                  biasShape,
                                                  DType::DType_INT32,
                                                  std::vector<uint8_t>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    tensors.push_back(std::move(biasTensor));
    auto biasOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                    Attribute::Attribute_NONE,
                                                                    nullptr,
                                                                    std::vector<std::string>{},
                                                                    std::vector<std::string>{conv2DBiasName},
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

    auto identity2InputTensor = std::make_unique<TosaSerializationTensor>(identity2InputName,
                                                                          outputShape,
                                                                          DType::DType_INT32,
                                                                          std::vector<uint8_t>{});
    tensors.push_back(std::move(identity2InputTensor));
    auto identity2OutputTensor = std::make_unique<TosaSerializationTensor>(identity2OutputName,
                                                                           outputShape,
                                                                           DType::DType_INT32,
                                                                           std::vector<uint8_t>{});
    tensors.push_back(std::move(identity2OutputTensor));

    // Create Operators Conv2D, Rescale, Conv2D
    auto identity1 = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_IDENTITY,
                                                                       Attribute::Attribute_NONE,
                                                                       nullptr,
                                                                       std::vector<std::string>{identity1InputName},
                                                                       std::vector<std::string>{identity1OutputName},
                                                                       TosaOpLocation{});
    ops.push_back(std::move(identity1));

    // Create Operators Conv2D
    auto conv2d = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{identity1OutputName, conv2DWeightsName, conv2DBiasName, input_zpName, weight_zpName},
        std::vector<std::string>{identity2InputName},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d));

    auto identity2 = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_IDENTITY,
                                                                       Attribute::Attribute_NONE,
                                                                       nullptr,
                                                                       std::vector<std::string>{identity2InputName},
                                                                       std::vector<std::string>{identity2OutputName},
                                                                       TosaOpLocation{});
    ops.push_back(std::move(identity2));

    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    auto block = std::make_unique<TosaSerializationBasicBlock>("multi_Layer",
                                                               "main",
                                                               std::move(ops),
                                                               std::move(tensors),
                                                               std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                                               std::vector<std::string>{identity1InputName},
                                                               std::vector<std::string>{identity2OutputName});

    return std::move(block);
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, IdentityConv2dIdentity)
{
    // Create a IdentityConv2dIdentity model block.
    auto block = GeneratorIdentityConv2DIdentityModel();

    TosaSerializationParser parser(block.get());

    const auto &spirvModule = parser.GenerateSPIRVModule("main");

    testutils::CheckModule(spirvModule,
                           TOSACONV2D,
                           {{std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 7, 7, 1}},
                            {std::initializer_list<uint32_t>{}, DataType::int8_t, {1, 3, 3, 1}},
                            {{0}, DataType::int32_t, {1}},
                            {{1}, DataType::int8_t, {1}},
                            {{1}, DataType::int8_t, {1}}},
                           {{DataType::int32_t, {1, 3, 3, 1}}},
                           {{{1, 1, 1, 1}, DataType::int32_t, {1}},
                            {{2, 2}, DataType::int32_t, {1}},
                            {{1, 1}, DataType::int32_t, {1}},
                            {{1}, DataType::int32_t, {1}},
                            {{1}, DataType::bool_t, {1}}});
}

// Multi Layer CONV2D - IDENTITY - RESCALE - CONV2D reusing the weight and bias GraphConstantId
// It should be identical to CONV2D - RESCALE - CONV2D
std::unique_ptr<TosaSerializationBasicBlock> GeneratorConv2DIdentityConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> stride = {2, 2};
    std::vector<int> dilation = {1, 1};
    bool local_bound = true;
    TosaConv2dAttribute conv2dAttribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Rescale Attribute
    bool scale32 = true;
    int32_t rounding_mode = 1;
    bool per_channel = true;
    bool input_unsigned = true;
    bool output_unsigned = true;
    std::vector<int> multiplier = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> shift = {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42};
    TosaRescaleAttribute rescaleAttribute(scale32,
                                          static_cast<const RoundingMode>(rounding_mode),
                                          per_channel,
                                          input_unsigned,
                                          output_unsigned);

    // Create Tensors
    std::string inputName = "input1";
    std::string weightName = "weights";
    std::string biasName = "bias";
    std::string identityInName = "identity_input";
    std::string identityOutName = "identity_output";
    std::string rescaleOutName = "rescale_output";
    std::string outputName = "output1";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string multiplierName = "multiplier";
    std::string shiftName = "shift";
    std::string input_zpRescaleName = "input_zp_rescale";
    std::string output_zpName = "output_zp";
    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> weightShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> InterTensorShape = {1, 5, 5, 1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<int32_t> multiplierShape = {1};
    std::vector<int32_t> shiftShape = {1};
    std::vector<int32_t> output_zpShape = {1};
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));

    auto weightTensor = std::make_unique<TosaSerializationTensor>(
        weightName,
        weightShape,
        DType::DType_INT8,
        std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    tensors.push_back(std::move(weightTensor));

    auto weightOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{weightName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(weightOp));

    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName,
                                                  biasShape,
                                                  DType::DType_INT32,
                                                  std::vector<uint8_t>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    tensors.push_back(std::move(biasTensor));

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

    auto identityInTensor = std::make_unique<TosaSerializationTensor>(identityInName,
                                                                      InterTensorShape,
                                                                      DType::DType_INT32,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(identityInTensor));

    auto identityOutTensor = std::make_unique<TosaSerializationTensor>(identityOutName,
                                                                       InterTensorShape,
                                                                       DType::DType_INT32,
                                                                       std::vector<uint8_t>{});
    tensors.push_back(std::move(identityOutTensor));

    auto rescaleOutTensor = std::make_unique<TosaSerializationTensor>(rescaleOutName,
                                                                      outputShape,
                                                                      DType::DType_INT8,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(rescaleOutTensor));

    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    auto multiplierTensor = std::make_unique<TosaSerializationTensor>(multiplierName,
                                                                      multiplierShape,
                                                                      DType::DType_INT32,
                                                                      std::vector<uint8_t>{{0, 0, 0, 1}});
    tensors.push_back(std::move(multiplierTensor));

    auto multiplierOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                          Attribute::Attribute_NONE,
                                                                          nullptr,
                                                                          std::vector<std::string>{},
                                                                          std::vector<std::string>{multiplierName},
                                                                          TosaOpLocation{});
    ops.push_back(std::move(multiplierOp));

    auto shiftTensor =
        std::make_unique<TosaSerializationTensor>(shiftName, shiftShape, DType::DType_INT8, std::vector<uint8_t>{1});
    tensors.push_back(std::move(shiftTensor));

    auto shiftOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{shiftName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(shiftOp));

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

    auto input_zpRescaleTensor = std::make_unique<TosaSerializationTensor>(input_zpRescaleName,
                                                                           input_zpShape,
                                                                           DType::DType_INT32,
                                                                           std::vector<uint8_t>{0, 0, 0, 1});
    tensors.push_back(std::move(input_zpRescaleTensor));

    auto input_zpRescaleOp =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{},
                                                          std::vector<std::string>{input_zpRescaleName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(input_zpRescaleOp));

    // Create Operators Conv2D, Rescale, Conv2D
    auto conv2d = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{identityInName},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d));
    auto identity1 = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_IDENTITY,
                                                                       Attribute::Attribute_NONE,
                                                                       nullptr,
                                                                       std::vector<std::string>{identityInName},
                                                                       std::vector<std::string>{identityOutName},
                                                                       TosaOpLocation{});
    ops.push_back(std::move(identity1));
    auto rescaleOp = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_RESCALE,
        Attribute::Attribute_RescaleAttribute,
        &rescaleAttribute,
        std::vector<std::string>{identityOutName, multiplierName, shiftName, input_zpRescaleName, output_zpName},
        std::vector<std::string>{rescaleOutName},
        TosaOpLocation{});
    ops.push_back(std::move(rescaleOp));
    auto conv2d2 = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{rescaleOutName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d2));

    // Create a tosa single-op basic block
    auto block = std::make_unique<TosaSerializationBasicBlock>("multi_layer",
                                                               "main",
                                                               std::move(ops),
                                                               std::move(tensors),
                                                               std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                                               std::vector<std::string>{inputName},
                                                               std::vector<std::string>{outputName});

    return std::move(block);
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Conv2dIdentityConv2d)
{
    // Create a IdentityConv2dIdentity model block.
    auto block = GeneratorConv2DIdentityConv2DModel();

    TosaSerializationParser parser(block.get());
    auto module1 = parser.GenerateSPIRVModule("main");

    std::vector<uint32_t> spirv = tfsc::WriteToBinary(module1);
    const auto text1 = testutils::DisassembleSPIRV(spirv, false);
    const auto text2 = spirvmodels::Conv2DRescaleConv2D;

    const auto actualBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text1));
    const auto expectedBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text2));
    EXPECT_EQ(actualBinary, expectedBinary);
}

// Multi Layer CONV2D - IDENTITY - (Graph Output) - IDENTITY - RESCALE - CONV2D
// reusing the weight and bias GraphConstantId
// It should be identical to CONV2D - RESCALE - CONV2D with an extra graph output from the first CONV2D
std::unique_ptr<TosaSerializationBasicBlock> GeneratorConv2DIdentityOutputConv2DModel()
{
    // Create Conv2D Attribute
    std::vector<int> pad = {1, 1, 1, 1};
    std::vector<int> stride = {2, 2};
    std::vector<int> dilation = {1, 1};
    bool local_bound = true;
    TosaConv2dAttribute conv2dAttribute(pad, stride, dilation, local_bound, DType_INT32);

    // Create Rescale Attribute
    //    std::vector<int8_t> rescale_input_zp = {0};
    //    std::vector<int8_t> output_zp        = {0};
    //    std::vector<int32_t> multiplier      = {8};
    //    std::vector<int32_t> shift           = {8};
    //    bool scale32       = true;
    //    bool double_round  = false;
    //    bool per_channel   = true;
    //    bool input_signed  = true;
    //    bool output_signed = true;
    //    TosaRescaleAttribute rescaleAttribute(
    //        rescale_input_zp[0], output_zp[0], multiplier, shift, scale32,
    //        double_round, per_channel, input_signed, output_signed);
    bool scale32 = true;
    int32_t rounding_mode = 1;
    bool per_channel = true;
    bool input_unsigned = true;
    bool output_unsigned = true;
    std::vector<int> multiplier = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> shift = {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42};
    TosaRescaleAttribute rescaleAttribute(scale32,
                                          static_cast<const RoundingMode>(rounding_mode),
                                          per_channel,
                                          input_unsigned,
                                          output_unsigned);

    // Create Tensors
    std::string inputName = "input1";
    std::string weightName = "weights";
    std::string biasName = "bias";
    std::string identity1InName = "identity1_input";
    std::string identity1OutName = "identity1_output";
    std::string identity2OutName = "identity2_output";
    std::string rescaleOutName = "rescale_output";
    std::string outputName = "output1";
    std::string input_zpName = "input_zp";
    std::string weight_zpName = "weight_zp";
    std::string multiplierName = "multiplier";
    std::string shiftName = "shift";
    std::string input_zpRescaleName = "input_zp_rescale";
    std::string output_zpName = "output_zp";
    std::vector<int32_t> inputShape = {1, 7, 7, 1};
    std::vector<int32_t> weightShape = {1, 3, 3, 1};
    std::vector<int32_t> biasShape = {1};
    std::vector<int32_t> InterTensorShape = {1, 5, 5, 1};
    std::vector<int32_t> outputShape = {1, 3, 3, 1};
    std::vector<int32_t> input_zpShape = {1};
    std::vector<int32_t> weight_zpShape = {1};
    std::vector<int32_t> multiplierShape = {1};
    std::vector<int32_t> shiftShape = {1};
    std::vector<int32_t> output_zpShape = {1};
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(inputName, inputShape, DType::DType_INT8, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));

    auto weightTensor = std::make_unique<TosaSerializationTensor>(
        weightName,
        weightShape,
        DType::DType_INT8,
        std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    tensors.push_back(std::move(weightTensor));
    auto weightOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                      Attribute::Attribute_NONE,
                                                                      nullptr,
                                                                      std::vector<std::string>{},
                                                                      std::vector<std::string>{weightName},
                                                                      TosaOpLocation{});
    ops.push_back(std::move(weightOp));

    auto biasTensor =
        std::make_unique<TosaSerializationTensor>(biasName,
                                                  biasShape,
                                                  DType::DType_INT32,
                                                  std::vector<uint8_t>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    tensors.push_back(std::move(biasTensor));
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

    auto identityInTensor = std::make_unique<TosaSerializationTensor>(identity1InName,
                                                                      InterTensorShape,
                                                                      DType::DType_INT32,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(identityInTensor));

    auto identityOutTensor = std::make_unique<TosaSerializationTensor>(identity1OutName,
                                                                       InterTensorShape,
                                                                       DType::DType_INT32,
                                                                       std::vector<uint8_t>{});
    tensors.push_back(std::move(identityOutTensor));

    auto identityOut2Tensor = std::make_unique<TosaSerializationTensor>(identity2OutName,
                                                                        InterTensorShape,
                                                                        DType::DType_INT32,
                                                                        std::vector<uint8_t>{});
    tensors.push_back(std::move(identityOut2Tensor));

    auto rescaleOutTensor = std::make_unique<TosaSerializationTensor>(rescaleOutName,
                                                                      outputShape,
                                                                      DType::DType_INT8,
                                                                      std::vector<uint8_t>{});
    tensors.push_back(std::move(rescaleOutTensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    auto multiplierTensor = std::make_unique<TosaSerializationTensor>(multiplierName,
                                                                      multiplierShape,
                                                                      DType::DType_INT32,
                                                                      std::vector<uint8_t>{{0, 0, 0, 1}});
    tensors.push_back(std::move(multiplierTensor));

    auto multiplierOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                          Attribute::Attribute_NONE,
                                                                          nullptr,
                                                                          std::vector<std::string>{},
                                                                          std::vector<std::string>{multiplierName},
                                                                          TosaOpLocation{});
    ops.push_back(std::move(multiplierOp));

    auto shiftTensor =
        std::make_unique<TosaSerializationTensor>(shiftName, shiftShape, DType::DType_INT8, std::vector<uint8_t>{1});
    tensors.push_back(std::move(shiftTensor));

    auto shiftOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                                     Attribute::Attribute_NONE,
                                                                     nullptr,
                                                                     std::vector<std::string>{},
                                                                     std::vector<std::string>{shiftName},
                                                                     TosaOpLocation{});
    ops.push_back(std::move(shiftOp));

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

    auto input_zpRescaleTensor = std::make_unique<TosaSerializationTensor>(input_zpRescaleName,
                                                                           input_zpShape,
                                                                           DType::DType_INT32,
                                                                           std::vector<uint8_t>{0, 0, 0, 1});
    tensors.push_back(std::move(input_zpRescaleTensor));

    auto input_zpRescaleOp =
        std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST,
                                                          Attribute::Attribute_NONE,
                                                          nullptr,
                                                          std::vector<std::string>{},
                                                          std::vector<std::string>{input_zpRescaleName},
                                                          TosaOpLocation{});
    ops.push_back(std::move(input_zpRescaleOp));

    // Create Operators Conv2D, Rescale, Conv2D
    auto conv2d = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{inputName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{identity1InName},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d));
    auto identity1 = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_IDENTITY,
                                                                       Attribute::Attribute_NONE,
                                                                       nullptr,
                                                                       std::vector<std::string>{identity1InName},
                                                                       std::vector<std::string>{identity1OutName},
                                                                       TosaOpLocation{});
    ops.push_back(std::move(identity1));
    auto identity2 = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_IDENTITY,
                                                                       Attribute::Attribute_NONE,
                                                                       nullptr,
                                                                       std::vector<std::string>{identity1OutName},
                                                                       std::vector<std::string>{identity2OutName},
                                                                       TosaOpLocation{});
    ops.push_back(std::move(identity2));
    auto rescaleOp = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_RESCALE,
        Attribute::Attribute_RescaleAttribute,
        &rescaleAttribute,
        std::vector<std::string>{identity2OutName, multiplierName, shiftName, input_zpRescaleName, output_zpName},
        std::vector<std::string>{rescaleOutName},
        TosaOpLocation{});
    ops.push_back(std::move(rescaleOp));

    auto conv2d2 = std::make_unique<tosa::TosaSerializationOperator>(
        Op::Op_CONV2D,
        Attribute::Attribute_Conv2dAttribute,
        &conv2dAttribute,
        std::vector<std::string>{rescaleOutName, weightName, biasName, input_zpName, weight_zpName},
        std::vector<std::string>{outputName},
        TosaOpLocation{});
    ops.push_back(std::move(conv2d2));

    // Create a tosa single-op basic block
    auto block = std::make_unique<TosaSerializationBasicBlock>("multi_layer",
                                                               "main",
                                                               std::move(ops),
                                                               std::move(tensors),
                                                               std::vector<std::unique_ptr<TosaSerializationShape>>{},
                                                               std::vector<std::string>{inputName},
                                                               std::vector<std::string>{outputName, identity1OutName});

    return std::move(block);
}

TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, Conv2dIdentityConv2dDualOutput)
{
    // Create a IdentityConv2dIdentity model block.
    auto block = GeneratorConv2DIdentityOutputConv2DModel();

    TosaSerializationParser parser(block.get());

    const auto module1 = parser.GenerateSPIRVModule("main");
    std::vector<uint32_t> spirv = tfsc::WriteToBinary(module1);
    const auto text1 = testutils::DisassembleSPIRV(spirv, false);
    const auto text2 = spirvmodels::Conv2DRescaleConv2DDualOutput;

    const auto actualBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text1));
    const auto expectedBinary = tfsc::WriteToBinary(testutils::LoadSPIRVDisassembly(text2));
    EXPECT_EQ(actualBinary, expectedBinary);
}

// Test case to test that we correctly transform empty tensor shape to shape of { 1 } otherwise this would fail.
TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, EmptyTensor)
{
    // Create Tensors
    std::string input1Name = "input1";
    std::string input2Name = "input2";
    std::string outputName = "output";

    std::vector<int32_t> input1Shape = {};
    std::vector<int32_t> input2Shape = {1, 1, 1, 1};
    std::vector<int32_t> outputShape = {1, 1, 1, 1};
    std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;
    std::vector<std::unique_ptr<TosaSerializationOperator>> ops;

    auto inputTensor =
        std::make_unique<TosaSerializationTensor>(input1Name, input1Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(inputTensor));
    auto input2Tensor =
        std::make_unique<TosaSerializationTensor>(input2Name, input2Shape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(input2Tensor));
    auto outputTensor =
        std::make_unique<TosaSerializationTensor>(outputName, outputShape, DType::DType_INT32, std::vector<uint8_t>{});
    tensors.push_back(std::move(outputTensor));

    // Create an Add model block.
    auto addOp = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_ADD,
                                                                   Attribute::Attribute_AddAttribute,
                                                                   nullptr,
                                                                   std::vector<std::string>{input1Name, input2Name},
                                                                   std::vector<std::string>{outputName},
                                                                   TosaOpLocation{});
    ops.push_back(std::move(addOp));

    // Create a tosa single-op basic block
    // The raw pointers of operators and tensors will be deleted by the destructor of the block
    TosaSerializationBasicBlock
        block("add", "main", std::move(ops), std::move(tensors), {}, {input1Name, input2Name}, {outputName});

    TosaSerializationParser parser(&block);

    auto module1 = parser.GenerateSPIRVModule("main");
    const auto binarySpirv = tfsc::WriteToBinary(module1);
    const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));

    EXPECT_EQ(spirvmodels::AddZeroDimOutputTensor, outputStr);
}
