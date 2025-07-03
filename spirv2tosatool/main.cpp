//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// TODO Plans:
// First version of exe tool, takes one CLI argument which is a filename
// File needs to contain a SPIRV disassembly string
// File is converted into a SPIR-V module, converted to TOSA op strings, printed to terminal

#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>

#include <AssemblyUtils.hpp>
#include <OpGenerator.hpp>
#include <stdexcept>

namespace tosa2spirv::spirv2tosatool
{

void usage()
{
    std::puts("spirv2tosatool usage:");
    std::puts("./spirv2tosatool <spirv disassembly file> [--unique]");
    std::puts(" --unique: optionally specify to only parse TOSA operators of unique types");
}

} // namespace tosa2spirv::spirv2tosatool

int main(const int argc, char** argv)
{
    if (argc < 2)
    {
        tosa2spirv::spirv2tosatool::usage();
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

    // Loading other args
    testutils::OperatorComparator opComparator = testutils::defaultOpComparator;
    for (size_t argIdx = 2; argIdx < argc; ++argIdx)
    {
        if (std::strcmp(argv[argIdx], "--unique") == 0)
        {
            opComparator = [](const testutils::TosaOperator& lhs, const testutils::TosaOperator& rhs) -> bool
            { return lhs.op < rhs.op; };
        }
    }

    std::string spirvDisassembly;
    try
    {
        std::ostringstream spirvBuffer;
        spirvBuffer << spirvFile.rdbuf();
        spirvDisassembly = spirvBuffer.str();
    }
    catch (std::exception e)
    {
        std::string errText = "Error loading file data: ";
        errText += e.what();
        throw std::runtime_error{errText};
    }

    std::vector<std::string> strings;
    try
    {
        const auto module = testutils::LoadSPIRVDisassembly(spirvDisassembly);
        strings = testutils::Spirv2tosa(module, opComparator);
    }
    catch (std::exception e)
    {
        std::string errText = "Error processing loaded SPIR-V data: ";
        errText += e.what();
        throw std::runtime_error{errText};
    }

    std::puts("------");
    for (const auto opString : strings)
    {
        std::puts(opString.c_str());
        std::puts("------");
    }
    return 0;
}
