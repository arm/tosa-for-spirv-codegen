//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <AssemblyUtils.hpp>
#include <EnumMaps.hpp>
#include <Graph.hpp>
#include <Module.hpp>
#include <OperatorDefinitions.hpp>
#include <OperatorEnum.hpp>
#include <TosaOperator.hpp>
#include <TosaRegen.hpp>
#include <TosaForSpirvCodegen.hpp>

#include <tosa_serialization_handler.h>

namespace tfsc::regen
{

void Usage()
{
    std::puts("tosa-regen usage:");
    std::puts("./tosa-regen <spirv disassembly file> <ops|tests|tosa> [options...]");
    std::puts(" ops ..... generate C/C++ strings for present TOSA Operators");
    std::puts("   options:");
    std::puts("   --unique: only show operators with unique opcodes");
    std::puts(" tests ... generate C/C++ source files containing Operator Tests");
    std::puts("   options:");
    std::puts("   --dir <output test files directory>");
    std::puts("   --overwrite: overwrite tests rather than appending to existing files if found");
    std::puts("   --unique: only add a single test case for each opcode");
    std::puts(" tosa .... generate TOSA Flatbuffer file (.tosa format) from disassembly");
    std::puts("   options:");
    std::puts("   --output <output file name>");
}

std::shared_ptr<spirv::Module> LoadSpirvFile(const std::ifstream& spirvFile)
{
    std::string spirvDisassembly;
    try
    {
        std::ostringstream spirvBuffer;
        spirvBuffer << spirvFile.rdbuf();
        spirvDisassembly = spirvBuffer.str();
    }
    catch (std::exception& e)
    {
        std::string errText = "Error loading file data: ";
        errText += e.what();
        throw std::runtime_error{errText};
    }

    try
    {
        return testutils::LoadSPIRVDisassembly(spirvDisassembly);
    }
    catch (std::invalid_argument& e)
    {
        std::string errText = "Error processing loaded SPIR-V data: ";
        errText += e.what();
        throw std::runtime_error{errText};
    }
}

int Ops(const int argc, char** argv, const std::ifstream& spirvFile)
{
    tosaregen::OperatorComparator opComparator = tosaregen::defaultOpComparator;

    for (size_t argIdx = 3; argIdx < argc; ++argIdx)
    {
        if (std::strcmp(argv[argIdx], "--unique") == 0)
        {
            opComparator = [](const tosaregen::TosaOperator& lhs, const tosaregen::TosaOperator& rhs) -> bool
            { return lhs.op < rhs.op; };
        }
    }

    const auto& module = LoadSpirvFile(spirvFile);
    const std::vector<std::string> ops = tosaregen::Spirv2tosa(module, opComparator);

    std::puts("------");
    for (const auto& opString : ops)
    {
        std::puts(opString.c_str());
        std::puts("------");
    }

    return 0;
}

void Tosa(const int argc, char** argv, const std::ifstream& spirvFile)
{
    std::string outputFile = "output.tosa";
    for (size_t argIdx = 3; argIdx < argc; ++argIdx)
    {
        if (std::strcmp(argv[argIdx], "--output") == 0)
        {
            if (++argIdx < argc)
            {
                outputFile = argv[argIdx];
            }
        }
    }

    const auto& module = LoadSpirvFile(spirvFile);
    auto handler = tosaregen::GetTosaSerializationHandler(module);
    handler->SaveFileTosaFlatbuffer(outputFile.c_str());
}

std::string GetTestDefinition(const tosaregen::TosaOperator op,
                              const std::string& operatorDefinition,
                              const std::string& testHash,
                              const std::string& testName)
{
    std::string testDefinition = "// TEST HASH " + testHash + "\nTEST(TOSA_FOR_SPIRV_CODEGEN_OPERATOR, " + testName + ")\n{\n";
    testDefinition += "// Operator Definition, separated for reuse in the test fixture\n" + operatorDefinition + "\n\n";

    testDefinition += "// Adding operator using Graph API\n"
                      "std::shared_ptr<tfsc::spirv::Module> module = "
                      "tfsc::CreateModule(tfsc::TOSAVersion::v1_0);\n"
                      "Graph graph{module};\n\n";
    testDefinition += tosaregen::OperatorToGraphDefinition(op, "graph", "inputs", "outputs", "attributes");

    testDefinition += "\n\n// Validating generated SPIR-V Module\n"
                      "testutils::CheckModule(module, op, inputs, outputs, attributes);\n}\n\n";
    return testDefinition;
}

int Tests(const int argc, char** argv, const std::ifstream& spirvFile, std::string testFileHeader)
{
    std::string outputDir = ".";
    bool overwrite = false;
    tosaregen::OperatorComparator opComparator = tosaregen::defaultOpComparator;

    for (size_t argIdx = 3; argIdx < argc; ++argIdx)
    {
        if (strcmp(argv[argIdx], "--overwrite") == 0)
        {
            overwrite = true;
        }
        else if (strcmp(argv[argIdx], "--dir") == 0)
        {
            if (++argIdx < argc)
            {
                outputDir = argv[argIdx];
            }
        }
        else if (strcmp(argv[argIdx], "--unique") == 0)
        {
            opComparator = [](const tosaregen::TosaOperator& lhs, const tosaregen::TosaOperator& rhs) -> bool
            { return lhs.op < rhs.op; };
        }
    }

    const auto& module = LoadSpirvFile(spirvFile);
    const std::vector<tosaregen::TosaOperator> ops = tosaregen::Spirv2operators(module, opComparator);

    std::map<tosa::OperatorEnum, std::pair<std::fstream, uint32_t>> opFiles;
    for (const auto& op : ops)
    {
        // If file for current operator tests not open yet, open it
        if (opFiles.count(op.op) == 0)
        {
            std::string testFileName = outputDir + "/" + tosa::GetOperatorName(op.op) + "OperatorTest.cpp";
            auto& file = opFiles[op.op].first;
            uint32_t& lastTestId = opFiles[op.op].second;
            lastTestId = 0;
            // If test file for current operator not open yet, open it
            if (overwrite)
            {
                file.open(testFileName, std::ios::in | std::ios::out | std::ios::trunc);
            }
            else
            {
                file.open(testFileName, std::ios::in | std::ios::out | std::ios::app);
                file.seekg(0, std::ios::end);
            }
            // Check that file opened successfully
            if (!file.is_open())
            {
                std::string errText = "Error: Couldn't open file '" + testFileName + "' to generate test case";
                throw std::runtime_error{errText};
            }
            // Finding last test ID if file not empty
            if (file.tellp() != 0)
            {
                file.seekg(0);
                std::string line;
                while (std::getline(file, line))
                {
                    size_t pos = line.find("OperatorTest");
                    if (pos != std::string::npos)
                    {
                        std::string number = line.substr(pos);
                        std::sscanf(number.c_str(), "OperatorTest%d)", &lastTestId);
                    }
                }
                ++lastTestId;
                file.clear();
                file.seekg(0, std::ios::end);
            }
        }
        auto& file = opFiles[op.op].first;
        uint32_t& lastTestId = opFiles[op.op].second;
        // If file for current operator tests empty, insert the preset top
        if (file.tellp() == 0)
        {
            file << testFileHeader;
        }
        // Append test case for the current operator
        const std::string opDefinition = tosaregen::OperatorToString(op);
        const std::string testHash = std::to_string(std::hash<std::string>{}(opDefinition));
        const std::string testName = tosa::GetOperatorName(op.op) + "OperatorTest" + std::to_string(lastTestId);

        const std::streampos oldPos = file.tellp();
        file.seekg(0);
        std::stringstream fileBuffer;
        fileBuffer << file.rdbuf();
        if (fileBuffer.str().find(testHash) == std::string::npos)
        {
            file.clear();
            file.seekg(oldPos);
            file << GetTestDefinition(op, opDefinition, testHash, testName);
            ++lastTestId;
        }
    }
    return 0;
}

} // namespace tfsc::regen

int main(const int argc, char** argv)
{
    if (argc < 3)
    {
        tfsc::regen::Usage();
        return 1;
    }

    // Loading file
    std::ifstream spirvFile{argv[1]};
    if (!spirvFile)
    {
        std::string errText = "Error: could not open file: ";
        errText += argv[1];
        throw std::runtime_error{errText};
    }

    // Processing requested operation string
    int result = 1;
    if (strcmp(argv[2], "ops") == 0)
    {
        result = tfsc::regen::Ops(argc, argv, spirvFile);
    }
    else if (strcmp(argv[2], "tests") == 0)
    {
        /* REUSE-IgnoreStart */
        std::string fileHeader = "//\n"
                                 "// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.\n"
                                 "// SPDX-License-Identifier: Apache-2.0\n"
                                 "//\n\n"
                                 "// Generated automatically by tosa-regen for TOSA 1.0\n\n"
                                 "#include <OpTestUtils.hpp>\n"
                                 "#include <TosaForSpirvCodegen.hpp>\n\n"
                                 "#include <gtest/gtest.h>\n\n"
                                 "using namespace tfsc::tosa;\n"
                                 "using namespace testutils;\n"
                                 "\n";
        /* REUSE-IgnoreEnd */
        result = tfsc::regen::Tests(argc, argv, spirvFile, fileHeader);
    }
    else if (strcmp(argv[2], "tosa") == 0)
    {
        tfsc::regen::Tosa(argc, argv, spirvFile);
    }
    else
    {
        std::printf("Error: unsupported operation: %s\n", argv[2]);
    }

    return result;
}
