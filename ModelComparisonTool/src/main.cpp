//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <AssemblyUtils.hpp>
#include <ModelParsers.hpp>
#include <ModuleComparator.hpp>
#include <TosaSerializationParser.hpp>
#include <tosa2spirv.hpp>

#include <argparse.hpp>
#include <tosa_serialization_handler.h>

#include <iostream>
#include <string>
#include <vgf/decoder.hpp>

namespace mlsdk::vgflib
{
class HeaderDecoder;
}

using namespace tosa;

namespace tosa2spirv::SPIRVComparison
{

// Parse SPIR-V, VGF, or TOSA file
std::shared_ptr<spirv::Module> ParseSpirvFile(const std::string& spirvFile)
{
    if (spirvFile.find(".tosa") != std::string::npos)
        return LoadTOSAFile(spirvFile);
    if (spirvFile.find(".spv") != std::string::npos)
        return LoadSPIRV(spirvFile);
    if (spirvFile.find(".vgf") != std::string::npos)
        return LoadVGFFile(spirvFile);

    // Fallback to disassembly
    return LoadSPIRVDisassembly(spirvFile);
}

} // namespace tosa2spirv::SPIRVComparison

int main(const int argc, char** argv)
{
    argparse::ArgumentParser parser("SPIRVComparisonTool");
    parser.add_description("Compare two SPIR-V/TOSA/VGF files for differences, or disassemble a single file.\n"
                           "\n"
                           "If both 'lhs' and 'rhs' are provided, the tool will parse and compare them.\n"
                           "If only 'lhs' is provided, the tool will disassemble and print the module.\n");
    parser.add_argument("lhs").help("Path to first input (.vgf/.spv/.tosa)");
    parser.add_argument("rhs")
        .nargs('?') // accept 0 or 1 values
        .default_value(std::string{})
        .help("Path to second input (.vgf/.spv/.tosa)");
    parser.add_argument("--short").default_value(false).implicit_value(true).help("Show short diff (default: false)");

    parser.add_argument("--full").default_value(false).implicit_value(true).help("Show full diff (default: false)");

    parser.add_argument("--help").default_value(false).implicit_value(true).nargs(0).help("Display this help menu");

    try
    {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n" << parser << std::endl;
        return EXIT_FAILURE;
    }

    if (parser.get<bool>("--help"))
    {
        std::cout << parser << std::endl;
        return EXIT_SUCCESS;
    }

    const std::string lhsPath = parser.get("lhs");
    const std::string rhsPath = parser.get("rhs");

    const auto lhsModule = tosa2spirv::SPIRVComparison::ParseSpirvFile(lhsPath);

    if (!lhsModule)
    {
        std::cerr << "Error: failed to parse file: " << lhsPath << std::endl;
        return EXIT_FAILURE;
    }

    if (rhsPath.empty())
    {
        if (!lhsModule)
        {
            std::cerr << "Error: failed to parse file: " << lhsPath << std::endl;
            return EXIT_FAILURE;
        }

        const auto binary = tosa2spirv::WriteToBinary(lhsModule);
        const auto disassembly = testutils::DisassembleSPIRV(binary);
        std::cout << disassembly << std::endl;
        return EXIT_SUCCESS;
    }

    const auto rhsModule = tosa2spirv::SPIRVComparison::ParseSpirvFile(rhsPath);

    if (!lhsModule || !rhsModule)
    {
        std::cerr << "Error: failed to load one or both modules." << std::endl;
        return EXIT_FAILURE;
    }

    const bool showShort = parser.get<bool>("--short");
    const bool showFull = parser.get<bool>("--full");

    testutils::ComparatorOptions co;

    if (showShort)
    {
        co.m_modelView = testutils::ModelView::problemsOnly;
    }
    if (showFull)
    {
        co.m_modelView = testutils::ModelView::module;
    }
    auto diff = testutils::CompareModules(lhsModule, rhsModule, co);

    std::cout << diff << std::endl;

    return EXIT_SUCCESS;
}
