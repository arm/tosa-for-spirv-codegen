//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <AssemblyUtils.hpp>
#include <ModelParsers.hpp>
#include <TosaSerializationParser.hpp>
#include <fstream>
#include <iostream>
#include <tosa_serialization_handler.h>
#include <vgf/decoder.hpp>

constexpr uint32_t SPIRV_MAGIC = 0x07230203;

std::shared_ptr<tfsc::spirv::Module> LoadSPIRV(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        return nullptr;
    }

    uint32_t magicMaybe = 0;
    file.read(reinterpret_cast<char*>(&magicMaybe), sizeof(magicMaybe));

    if (!file || file.gcount() != sizeof(magicMaybe))
    {
        // Could not read the full magic number
        return nullptr;
    }

    if (magicMaybe == SPIRV_MAGIC)
    {
        return LoadSPIRVAssembly(filename);
    }
    return LoadSPIRVDisassembly(filename);
}

std::shared_ptr<tfsc::spirv::Module> LoadSPIRVAssembly(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file)
    {
        return nullptr;
    }

    const std::streamsize size = file.tellg();
    if (size % sizeof(uint32_t) != 0)
    {
        return nullptr;
    }

    std::vector<uint32_t> spirv(size / sizeof(uint32_t));
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(spirv.data()), size);

    const auto disassembly = testutils::DisassembleSPIRV(spirv, true);
    return testutils::LoadSPIRVDisassembly(disassembly);
}

std::shared_ptr<tfsc::spirv::Module> LoadSPIRVDisassembly(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string disassembly = buffer.str();

    return testutils::LoadSPIRVDisassembly(disassembly);
}

std::shared_ptr<tfsc::spirv::Module> LoadTOSAFile(const std::string& filename)
{
    using namespace tosa;
    TosaSerializationHandler handler;
    // Load Flatbuffers file into serialization handler using
    // LoadFileTosaFlatbuffer() We can also load using JSON (LoadFileSchema and
    // LoadFileJson), but this requires Flatbuffers schema.
    auto error = handler.LoadFileTosaFlatbuffer(filename.c_str());
    if (error != TOSA_OK)
    {
        throw std::runtime_error("Error loading TOSA Flatbuffers file. Error code: " + std::to_string(error));
    }
    error = handler.LoadFileTosaFlatbuffer(filename.c_str());
    if (error != TOSA_OK)
    {
        throw std::runtime_error("Error loading TOSA Flatbuffers file. Error code: " + std::to_string(error));
    }

    auto* mainRegion = handler.GetMainRegion();
    if (mainRegion == nullptr)
    {
        throw std::runtime_error("No main region found from the handler.");
    }

    // Try to get the main block from the handler.
    const auto mainBlock = mainRegion->GetBlockByName("main");

    if (mainBlock == nullptr)
    {
        std::cerr << "Please ensure there is a block called \"main\" within the "
                     "TOSA graph."
                  << std::endl;
        return nullptr;
    }

    // Initialize TosaSerializationParser with main block.
    auto parser = tfsc::parsers::TosaSerializationParser(mainBlock);
    return parser.GenerateSPIRVModule("");
}

// Stolen from samples/2_decode_simple_graph_sample.cpp
std::shared_ptr<tfsc::spirv::Module> LoadVGFFile(const std::string& filename)
{
    using namespace mlsdk;

    // Open the file
    std::ifstream vgf_file(filename, std::ios::binary);
    if (!vgf_file.is_open())
    {
        throw std::runtime_error("Failed to open VGF file: " + filename);
    }

    // Determine file size
    vgf_file.seekg(0, std::ios_base::end);
    std::streampos file_size = vgf_file.tellg();
    vgf_file.seekg(0, std::ios_base::beg);

    // Validate header size
    size_t header_size = vgflib::HeaderSize();
    if (header_size > static_cast<size_t>(file_size))
    {
        throw std::runtime_error("VGF file '" + filename + "' is too small (" + std::to_string(file_size) +
                                 " bytes) for expected header size (" + std::to_string(header_size) + " bytes)");
    }

    // Read header
    std::vector<char> header_mem(header_size);
    vgf_file.read(header_mem.data(), static_cast<std::streamsize>(header_mem.size()));
    if (!vgf_file)
    {
        throw std::runtime_error("Error reading header from VGF file: " + filename);
    }

    // Decode header
    std::unique_ptr<vgflib::HeaderDecoder> header_decoder = vgflib::CreateHeaderDecoder(header_mem.data());

    // Read module table
    size_t table_size = header_decoder->GetModuleTableSize();
    std::streamoff table_offset = static_cast<std::streamoff>(header_decoder->GetModuleTableOffset());
    std::vector<char> module_table_data(table_size);
    vgf_file.seekg(table_offset);
    vgf_file.read(module_table_data.data(), static_cast<std::streamsize>(table_size));
    if (!vgf_file)
    {
        throw std::runtime_error("Error reading module table from VGF file: " + filename);
    }

    // Decode modules
    std::vector<char> decoder_mem(vgflib::ModuleTableDecoderSize());
    vgflib::ModuleTableDecoder* modules_decoder =
        vgflib::CreateModuleTableDecoderInPlace(module_table_data.data(), decoder_mem.data());

    // Validate module count
    size_t module_count = modules_decoder->size();
    if (module_count != 1)
    {
        throw std::runtime_error("Expected 1 graph module in VGF file '" + filename + "', but found " +
                                 std::to_string(module_count));
    }

    // Validate module type
    if (modules_decoder->getModuleType(0) != vgflib::ModuleType::GRAPH)
    {
        throw std::runtime_error("Module 0 in VGF file '" + filename + "' is not a GRAPH module");
    }

    // Ensure SPIR-V is present
    if (!modules_decoder->hasSPIRV(0))
    {
        throw std::runtime_error("Module 0 in VGF file '" + filename + "' does not contain embedded SPIR-V code");
    }

    // Verify code size
    auto code_view = modules_decoder->getModuleCode(0);
    if (code_view.size() == 0)
    {
        throw std::runtime_error("SPIR-V code for module 0 in VGF file '" + filename + "' is empty");
    }

    // Disassemble and load
    std::vector<uint32_t> spirvBinary(code_view.begin(), code_view.end());
    auto disassembly = testutils::DisassembleSPIRV(spirvBinary, true);
    return testutils::LoadSPIRVDisassembly(disassembly);
}
