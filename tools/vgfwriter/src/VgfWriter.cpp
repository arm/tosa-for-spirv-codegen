//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <spirv/Module.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <vgf/encoder.hpp>
#include <vgf/vulkan_helpers.generated.hpp>

#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>

namespace tfsc::vgfwriter
{

std::vector<int64_t> ExtractShapeFromTensor(const spirv::Instruction* tensorInstruction)
{
    const auto tensorShape = tensorInstruction->m_Operands[3].m_InstructionPtr;

    if (tensorShape->m_Opcode == spv::OpConstantCompositeReplicateEXT)
    {
        const unsigned int value = tensorShape->m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord;
        const auto array = tensorShape->m_Operands[0].m_InstructionPtr;
        const unsigned int size = array->m_Operands[2].m_InstructionPtr->m_Operands[2].m_LiteralWord;
        return std::vector<int64_t>(size, value);
    }
    std::vector<int64_t> shapeVector;
    for (auto operandIt = std::next(tensorShape->m_Operands.begin(), 2); operandIt != tensorShape->m_Operands.end();
         ++operandIt)
    {
        shapeVector.push_back(operandIt->m_InstructionPtr->m_Operands[2].m_LiteralWord);
    }
    return shapeVector;
}

static std::string GetVulkanFormatFromTensor(const spirv::Instruction* tensorInstruction)
{
    using namespace spv;
    const auto typeInstruction = tensorInstruction->m_Operands[1].m_InstructionPtr;

    const Op& dataType = typeInstruction->m_Opcode;
    const uint32_t rank = typeInstruction->m_Operands[1].m_LiteralWord;
    switch (dataType)
    {
        case OpTypeBool: return "VK_FORMAT_R8_BOOL_ARM";
        case OpTypeInt:
            switch (rank)
            {
                case 8u: return "VK_FORMAT_R8_SINT";
                case 16u: return "VK_FORMAT_R16_SINT";
                case 32u: return "VK_FORMAT_R32_SINT";
                case 64u: return "VK_FORMAT_R64_SINT";
                default:;
            }
        case OpTypeFloat:
        {
            switch (rank)
            {
                case 16u: return "VK_FORMAT_R16_SFLOAT";
                case 32u: return "VK_FORMAT_R32_SFLOAT";
                default:;
            }
        }
        default: throw std::runtime_error("GetVulkanFormatFromDataType: Unsupported DataType.");
    }
}

std::vector<mlsdk::vgflib::ConstantRef> CollectConstants(mlsdk::vgflib::Encoder& encoder,
                                                         const spirv::Module& module,
                                                         std::vector<std::pair<const void*, size_t>> dataVector)
{
    std::vector<mlsdk::vgflib::ConstantRef> constantRefs;
    auto [constantsBegin, constantsEnd] = module.GetInstructionsOfType(spv::OpGraphConstantARM);

    auto constantData = dataVector.begin();
    for (auto it = constantsBegin; it != constantsEnd; ++it, ++constantData)
    {
        if (constantData->first == nullptr)
        {
            continue;
        }

        const auto tensor = it->m_Operands[0].m_InstructionPtr;
        auto tensorType = GetVulkanFormatFromTensor(tensor);
        std::vector<int64_t> constant_shape = ExtractShapeFromTensor(tensor);

        size_t size = constantData->second;

        const mlsdk::vgflib::ResourceRef constResRef =
            encoder.AddConstantResource(mlsdk::vgflib::NameToFormatType(tensorType), constant_shape, {}
                                        // empty strides are interpreted as packed linear
            );

        const void* data = constantData->first;
        mlsdk::vgflib::ConstantRef constRef = encoder.AddConstant(constResRef, data, size);
        constantRefs.emplace_back(constRef);
    }

    // Free dataVector
    std::vector<std::pair<const void*, size_t>>().swap(dataVector);
    return constantRefs;
}

constexpr uint16_t VK_HEADER_VERSION = 999;

void WriteVGF(std::shared_ptr<spirv::Module>& module,
              std::vector<std::pair<const void*, size_t>>&& data,
              const std::string& moduleName,
              const std::string& path,
              const std::string& filename)
{
    // Create an encoder object
    std::unique_ptr<mlsdk::vgflib::Encoder> encoder = mlsdk::vgflib::CreateEncoder(VK_HEADER_VERSION);

    std::vector<mlsdk::vgflib::ConstantRef> constantRefs = CollectConstants(*encoder, *module, std::move(data));

    std::vector<mlsdk::vgflib::BindingSlotRef> inputBindingRefs;
    std::vector<mlsdk::vgflib::BindingSlotRef> outputBindingRefs;
    std::vector<mlsdk::vgflib::BindingSlotRef> allBindingRefs;

    const std::string resourceType = "VK_DESCRIPTOR_TYPE_TENSOR_ARM";

    std::unordered_map<const spirv::Instruction*, unsigned int> bindingMap;
    auto [decorateBegin, decorateEnd] = module->GetInstructionsOfType(spv::OpDecorate);
    for (auto it = decorateBegin; it != decorateEnd; ++it)
    {
        if (it->m_Operands[1].m_LiteralWord != spv::DecorationBinding)
            continue;

        const auto opVariable = it->m_Operands[0].m_InstructionPtr;
        bindingMap.emplace(opVariable, it->m_Operands[2].m_LiteralWord);
    }

    auto graphType = module->GetInstructionsOfType(spv::OpTypeGraphARM).first;
    const auto inputSize = graphType->m_Operands[1].m_LiteralWord;

    auto graphEntryPointInstruction = module->GetInstructionsOfType(spv::OpGraphEntryPointARM).first;
    const auto& entryPoints = graphEntryPointInstruction->m_Operands;

    auto inputBegin = entryPoints.begin() + 2;
    const auto inputEnd = inputBegin + inputSize;

    for (; inputBegin < inputEnd; ++inputBegin)
    {
        const auto inputOpVariable = inputBegin->m_InstructionPtr;
        const auto uniformConstantPtr = inputOpVariable->m_Operands[0].m_InstructionPtr;
        const auto inputTensor = uniformConstantPtr->m_Operands[2].m_InstructionPtr;

        auto tensorType = GetVulkanFormatFromTensor(inputTensor);
        std::vector<int64_t> shape = ExtractShapeFromTensor(inputTensor);

        mlsdk::vgflib::ResourceRef inputResRef =
            encoder->AddInputResource(mlsdk::vgflib::NameToDescriptorType(resourceType),
                                      mlsdk::vgflib::NameToFormatType(tensorType),
                                      shape,
                                      {} // empty strides are interpreted as packed linear
            );
        const auto inputBindingId = bindingMap.at(inputOpVariable);
        mlsdk::vgflib::BindingSlotRef bindingRef = encoder->AddBindingSlot(inputBindingId, inputResRef);
        allBindingRefs.emplace_back(bindingRef);
        inputBindingRefs.emplace_back(bindingRef);
    }

    for (auto outputBegin = inputBegin; outputBegin < entryPoints.end(); ++outputBegin)
    {
        const auto outputOpVariable = outputBegin->m_InstructionPtr;
        const auto uniformConstantPtr = outputOpVariable->m_Operands[0].m_InstructionPtr;
        const auto outputTensor = uniformConstantPtr->m_Operands[2].m_InstructionPtr;

        auto tensorType = GetVulkanFormatFromTensor(outputTensor);
        std::vector<int64_t> shape = ExtractShapeFromTensor(outputTensor);

        mlsdk::vgflib::ResourceRef outputResRef =
            encoder->AddOutputResource(mlsdk::vgflib::NameToDescriptorType(resourceType),
                                       mlsdk::vgflib::NameToFormatType(tensorType),
                                       shape,
                                       {} // empty strides are interpreted as packed linear
            );
        const auto outputBindingId = bindingMap.at(outputOpVariable);
        mlsdk::vgflib::BindingSlotRef bindingRef = encoder->AddBindingSlot(outputBindingId, outputResRef);
        allBindingRefs.emplace_back(bindingRef);
        outputBindingRefs.emplace_back(bindingRef);
    }

    mlsdk::vgflib::DescriptorSetInfoRef descInfoRef = encoder->AddDescriptorSetInfo(allBindingRefs);
    const mlsdk::vgflib::ModuleRef graphRef =
        encoder->AddModule(mlsdk::vgflib::ModuleType::GRAPH, moduleName, "main", WriteToBinary(module));

    encoder->AddSegmentInfo(graphRef,
                            "segment_" + moduleName,
                            {descInfoRef},
                            inputBindingRefs,
                            outputBindingRefs,
                            constantRefs,
                            {}, // segment for a graph module so doesn't require any dispatch shape
                            {}  // push constants.
    );

    encoder->AddModelSequenceInputsOutputs(inputBindingRefs, {"input_0"}, outputBindingRefs, {"output_0"});

    encoder->Finish();

    // lambda function to allow boolean check for file existing
    auto fileExists = [](const char* filename) -> bool {
        const std::ifstream file(filename);
        return file.good();
    };

    // Check if the vgf file already exists
    auto archivePath = path + "/" + filename;
    if (fileExists(archivePath.c_str()))
    {
        std::cerr << "Vgf file " + archivePath + " already exists; companion file will not be created." << std::endl;
    }
    else
    {
        std::ofstream vgf_file(archivePath.c_str(), std::ofstream::binary | std::ofstream::trunc);
        encoder->WriteTo(vgf_file);
        vgf_file.close();
    }
}
// tfsc::vgfwriter namespace
} // namespace tfsc::vgfwriter
