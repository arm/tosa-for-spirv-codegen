//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <TosaSerializationParser.hpp>

#include <argparse.hpp>
#include <tosa_serialization_handler.h>

#include <VgfWriter.hpp>
#include <iostream>

using namespace tosa;

namespace tfsc::generator
{

void LoadGraph(TosaSerializationHandler& handler,
               const std::string& tosaFile,
               const std::string& jsonFile,
               const std::string& schemaFile);

} // namespace tfsc::generator

int main(const int argc, char** argv)
{
    using namespace tfsc::generator;
    std::string tosaFile;
    std::string jsonFile;
    std::string schemaFile;
    std::string outputFile;

    // Parse options using argparse
    try
    {
        argparse::ArgumentParser parser("tfsc-generator");
        parser.add_argument("-t", "--tosa-file")
            .help("TOSA Flatbuffers input file. Supports .tosa extension.")
            .default_value(std::string{""});

        parser.add_argument("-j", "--json-file")
            .help("JSON descriptor file. Requires Flatbuffers Schema to be supplied.")
            .default_value(std::string{""});

        parser.add_argument("-s", "--schema-file")
            .help("Flatbuffers Schema file. Required if using JSON descriptor file.")
            .default_value(std::string{""});

        parser.add_argument("-o", "--output-file")
            .help("Name of vgf file to package SPIR-V model binary and JSON companion file into;"
                  " may include path otherwise it will output to the same directory as the Generator"
                  " executable. Filename must end in '.vgf'.")
            .required()
            .default_value(std::string{""});

        parser.parse_args(argc, argv);

        tosaFile = parser.get("--tosa-file");
        jsonFile = parser.get("--json-file");
        schemaFile = parser.get("--schema-file");
        outputFile = parser.get("--output-file");

        if (outputFile.empty())
        {
            std::cerr << "Both the output file (.vgf) path specified is empty,"
                         " please specify a file to write to for one or both of them."
                      << std::endl;
            std::cerr << parser << std::endl;
            return EXIT_FAILURE;
        }

        if (!outputFile.empty() && outputFile.find(".vgf") == std::string::npos)
        {
            std::cerr << "Output file (.vgf) name must end in: .vgf" << std::endl;
            std::cerr << parser << std::endl;
            return EXIT_FAILURE;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    TosaSerializationHandler handler;
    try
    {
        // Load Flatbuffers file into serialization handler using LoadFileTosaFlatbuffer()
        // We can also load using JSON (LoadFileSchema and LoadFileJson), but this requires Flatbuffers schema.
        LoadGraph(handler, tosaFile, jsonFile, schemaFile);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Ensure handler has regions before using GetMainRegion()
    if (handler.GetRegions().empty())
    {
        std::cerr << "Handler did not produce any regions." << std::endl;
        return EXIT_FAILURE;
    }

    // Try to get the main region from the handler.
    auto* mainRegion = handler.GetMainRegion();
    if (mainRegion == nullptr)
    {
        std::cerr << "No main region found from the handler." << std::endl;
        return EXIT_FAILURE;
    }

    // Try to get the main block from the handler.
    auto mainBlock = mainRegion->GetBlockByName("main");

    if (mainBlock == nullptr)
    {
        std::cerr << "Please ensure there is a block called \"main\" within the TOSA graph." << std::endl;
        return EXIT_FAILURE;
    }

    // Initialize TosaSerializationParser with main block.
    auto parser = tfsc::parsers::TosaSerializationParser(mainBlock);

    auto module = parser.GenerateSPIRVModule("main");

    if (!outputFile.empty())
    {
        // Write the companion file
        const auto splitPosition = outputFile.find_last_of('/');
        std::string path;
        // If there is a slash then split into path and file
        if (splitPosition != std::string::npos)
        {
            path = outputFile.substr(0, splitPosition);
            outputFile = outputFile.substr(splitPosition + 1);
        }
        else
        {
            // Otherwise, set path to current directory
            path = ".";
        }

        // Remove .vgf extension to retrieve modelName
        const auto vgfPosition = outputFile.find_last_of(".vgf");
        const std::string modelName = outputFile.substr(0, vgfPosition - 3);

        tfsc::vgfwriter::WriteVGF(module,
                                        tfsc::parsers::ConvertConstantDataToVoid(parser.GetExternalConstants()),
                                        modelName,
                                        path,
                                        outputFile);
    }

    return EXIT_SUCCESS;
}

namespace tfsc::generator
{

void LoadGraph(TosaSerializationHandler& handler,
               const std::string& tosaFile,
               const std::string& jsonFile,
               const std::string& schemaFile)
{
    tosa_err_t error;

    if ((!jsonFile.empty() && schemaFile.empty()) || (jsonFile.empty() && !schemaFile.empty()))
    {
        throw std::runtime_error("Both JSON file and Schema file options must be supplied, to parse JSON file.");
    }

    if (!jsonFile.empty() && !schemaFile.empty())
    {
        error = handler.LoadFileSchema(schemaFile.c_str());
        if (error != tosa_err_t::TOSA_OK)
        {
            throw std::runtime_error("Error loading Schema file. Error code: " + std::to_string(error));
        }

        error = handler.LoadFileJson(jsonFile.c_str());
        if (error != tosa_err_t::TOSA_OK)
        {
            throw std::runtime_error("Error loading JSON file. Error code: " + std::to_string(error));
        }
    }
    else
    {
        if (tosaFile.empty())
        {
            throw std::runtime_error("Please supply a TOSA Flatbuffers file.");
        }

        error = handler.LoadFileTosaFlatbuffer(tosaFile.c_str());
        if (error != tosa_err_t::TOSA_OK)
        {
            throw std::runtime_error("Error loading TOSA Flatbuffers file. Error code: " + std::to_string(error));
        }
    }
}

} // tfsc::generator
