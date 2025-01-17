//
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//
#include <GeneratorUtils.hpp>
#include <ResourceInfoVisitor.hpp>

#include <tosa_serialization_handler.h>
#include <vgf/vulkan_helpers.generated.hpp>
#include <vgf/encoder.hpp>

#include <iostream>

constexpr uint16_t VK_HEADER_VERSION = 999;

namespace tosa2spirv::generator
{

template <typename T>
static size_t SizeOfVector(const std::vector<T> &vec) { return vec.size() * sizeof(T); }

static std::vector<uint8_t> ConvertInt32ToUint8(const std::vector<int32_t>& int32Vector) {
    std::vector<uint8_t> uint8Vect;
    for (int32_t value : int32Vector)
    {
        // Break the int32 value down into 4 bytes and push into uint8Vect
        uint8Vect.push_back(static_cast<uint8_t>((value >> 0) & 0xFF));
        uint8Vect.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        uint8Vect.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        uint8Vect.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    }
    return uint8Vect;
}

static std::vector<uint8_t> ConvertInt16ToUint8(const std::vector<int16_t>& int16Vector) {
    std::vector<uint8_t> uint8Vect;
    for (int16_t value : int16Vector)
    {
        // Break the int16 value down into 4 bytes and push into uint8Vect
        uint8Vect.push_back(static_cast<uint8_t>((value >> 0) & 0xFF));
        uint8Vect.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    }
    return uint8Vect;
}

static std::vector<uint8_t> ConvertFloat32ToUint8(const std::vector<float>& float32Vector) {
    std::vector<uint8_t> uint8Vect;
    for (int16_t value : float32Vector)
    {
        // Break the float value down into 4 bytes and push into uint8Vect
        uint32_t* uint32Val = reinterpret_cast<uint32_t*>(&value);
        uint8Vect.push_back(*uint32Val & 0xFF);
        uint8Vect.push_back((*uint32Val >> 8) & 0xFF);
        uint8Vect.push_back((*uint32Val >> 16) & 0xFF);
        uint8Vect.push_back((*uint32Val >> 24) & 0xFF);
    }
    return uint8Vect;
}

static std::vector<uint8_t> GetConstantVec(tosa2spirv::parsers::IParser* parser, uint32_t idx)
{
    if (idx >= parser->GetGraphConstants().size())
    {
        throw std::invalid_argument("GetConstantVec(): Error: idx outside the bounds of the Constants vector.");
    }
    auto tensor = parser->GetGraphConstants()[idx];

    if (std::holds_alternative<tosa::TosaSerializationTensor*>(tensor))
    {
        auto ptr = std::get<tosa::TosaSerializationTensor*>(tensor);
        return ptr->GetData();
    }
    else if (std::holds_alternative<std::vector<int8_t>>(tensor))
    {
        auto vect = std::get<std::vector<int8_t>>(tensor);
        return std::vector<uint8_t>(vect.begin(), vect.end());
    }
    else if (std::holds_alternative<std::vector<int16_t>>(tensor))
    {
        auto vect = std::get<std::vector<int16_t>>(tensor);
        return ConvertInt16ToUint8(vect);
    }
    else if (std::holds_alternative<std::vector<int32_t>>(tensor))
    {
        auto vect = std::get<std::vector<int32_t>>(tensor);
        return ConvertInt32ToUint8(vect);
    }
    else if (std::holds_alternative<std::vector<float>>(tensor))
    {
        auto vect = std::get<std::vector<float>>(tensor);
        return ConvertFloat32ToUint8(vect);
    }
    else
    {
        throw std::invalid_argument("Error: No data to write to .npy file.");
    }
}

static void EncodeGraphToVgf(tosa2spirv::parsers::IParser* parser,
                             vgflib::Encoder* encoder,
                             std::vector<uint32_t>& spirv,
                             const std::string& modelName)
{
    ResourceInfoVisitor vkVisitor;
    parser->Accept(vkVisitor);
    ResourceInfo& info = vkVisitor.GetResourceInfo();

    std::vector<vgflib::ConstantRef> constantRefs;
    std::vector<vgflib::ResourceRef> inputRefs;
    std::vector<vgflib::ResourceRef> outputRefs;

    std::vector<vgflib::BindingSlotRef> inputBindingRefs;
    std::vector<vgflib::BindingSlotRef> outputBindingRefs;
    std::vector<vgflib::BindingSlotRef> allBindingRefs;

    // Constants
    // Note: vgf_encoder adds constant in order, not taking account of the id we've given it.
    for (auto& constant : info.GetConstants())
    {
        std::vector<int64_t> int64Vec(constant.second.m_Shape.begin(), constant.second.m_Shape.end());
        vgflib::ResourceRef constResRef = encoder->AddConstantResource(
            vgflib::NameToFormatType(constant.second.m_Format),
            int64Vec,
            {} // empty strides are interpreted as packed linear
        );

        std::vector<uint8_t> vec = GetConstantVec(parser, constant.first);

        vgflib::ConstantRef constRef =
            encoder->AddConstant(constResRef, vec.data(), SizeOfVector(vec));
        constantRefs.emplace_back(constRef);
    }

    // Inputs
    for (const auto& input : info.GetInputs())
    {
        std::vector<int64_t> int64Vec(input.m_Shape.begin(), input.m_Shape.end());
        vgflib::ResourceRef inputResRef =
            encoder->AddInputResource(
                vgflib::NameToDescriptorType(input.m_ResourceType),
                vgflib::NameToFormatType(input.m_Format),
                int64Vec,
                {} // empty strides are interpreted as packed linear
            );
        inputRefs.emplace_back(inputResRef);

        vgflib::BindingSlotRef bindingRef = encoder->AddBindingSlot(
            input.m_BindingId,
            inputResRef
        );
        inputBindingRefs.emplace_back(bindingRef);
        allBindingRefs.emplace_back(bindingRef);
    }

    // Outputs
    for (const auto& output : info.GetOutputs())
    {
        std::vector<int64_t> int64Vec(output.m_Shape.begin(), output.m_Shape.end());
        vgflib::ResourceRef outputResRef =
            encoder->AddOutputResource(
                vgflib::NameToDescriptorType(output.m_ResourceType),
                vgflib::NameToFormatType(output.m_Format),
                int64Vec,
                {} // empty strides are interpreted as packed linear
            );
        outputRefs.emplace_back(outputResRef);

        vgflib::BindingSlotRef bindingRef = encoder->AddBindingSlot(
            output.m_BindingId,
            outputResRef
        );
        outputBindingRefs.emplace_back(bindingRef);
        allBindingRefs.emplace_back(bindingRef);
    }
    vgflib::DescriptorSetInfoRef descInfoRef = encoder->AddDescriptorSetInfo(allBindingRefs);

    vgflib::ModuleRef graphRef =
        encoder->AddModule(vgflib::ModuleType::GRAPH,
                           modelName,
                           "main",
                           spirv
        );

    vgflib::SegmentInfoRef segmentRef =
        encoder->AddSegmentInfo(graphRef,
                                "segment_" + modelName,
                                {descInfoRef},
                                inputBindingRefs,
                                outputBindingRefs,
                                constantRefs,
                                {}, // segment for a graph module so doesn't require any dispatch shape
                                {}  // push constants.
        );

    encoder->AddModelSequenceInputsOutputs(
        inputBindingRefs,
        {"input_0"},
        outputBindingRefs,
        {"output_0"}
    );
}

void WriteVgfFile(tosa2spirv::parsers::IParser* parser,
                  std::vector<uint32_t>& spirv,
                  const std::string& modelName,
                  const std::string& outputDir,
                  const std::string& vgfName)
{
    // Create an encoder object
    std::unique_ptr<vgflib::Encoder> encoder = vgflib::CreateEncoder(VK_HEADER_VERSION);

    // Get the resource info json that we will add to the vgf file.
    EncodeGraphToVgf(parser, encoder.get(), spirv, modelName);

    encoder->Finish();

    // lambda function to allow boolean check for file existing
    auto fileExists = [](const char* filename) -> bool
    {
        std::ifstream file(filename);
        return file.good();
    };

    // Check if the vgf file already exists
    auto archivePath = outputDir + "/" + vgfName;
    if (fileExists(archivePath.c_str()))
    {
        std::cerr << "Vgf file " + archivePath +
                     " already exists; companion file will not be created." << std::endl;
    }
    else
    {
        std::ofstream vgf_file(archivePath.c_str(), std::ofstream::binary | std::ofstream::trunc);
        encoder->WriteTo(vgf_file);
        vgf_file.close();
    }
}

} // tosa2spirv::generator
