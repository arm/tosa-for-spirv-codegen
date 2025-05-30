//
// Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <OpTestUtils.hpp>

#include <iostream>
#include <regex>
#include <sstream>
#include <stack>

#include <effcee/effcee.h>
#include <gtest/gtest.h>

namespace testutils
{

std::string ExpandValues(const std::vector<int>& values)
{
    std::string result;

    for (const int i : values)
    {
        result += "%uint_" + std::to_string(i) + ' ';
    }

    return result;
}

std::string AddSpacers(const int index)
{
    std::string spacer;

    for (int i = 0; i != index; ++i)
    {
        spacer += R"({{%\w+}} )";
    }

    return spacer;
};

std::string CreateEffceeCheckFromDataType(DataType dataType)
{
    switch (dataType)
    {
        // Note types are all signless in TOSA SPIRV so for example int8_t will always be uint8_t i.e. signedness = 0.
        case DataType::int4_t: return R"(CHECK: [[DATATYPE:%\w+]] = OpTypeInt 4 0)";
        case DataType::int8_t:
        case DataType::uint8_t: return R"(CHECK: [[DATATYPE:%\w+]] = OpTypeInt 8 0)";
        case DataType::int16_t:
        case DataType::uint16_t: return R"(CHECK: [[DATATYPE:%\w+]] = OpTypeInt 16 0)";
        case DataType::int32_t:
        case DataType::uint32_t: return R"(CHECK: [[DATATYPE:%\w+]] = OpTypeInt 32 0)";
        case DataType::float16_t: return R"(CHECK: [[DATATYPE:%\w+]] = OpTypeFloat 16)";
        case DataType::float32_t: return R"(CHECK: [[DATATYPE:%\w+]] = OpTypeFloat 32)";
        case DataType::bfloat16_t: return R"(CHECK: [[DATATYPE:%\w+]] = OpTypeFloat 16 BFLOAT16)";
        case DataType::bool_t: return R"(CHECK: [[DATATYPE:%\w+]] = OpTypeBool)";
        default: throw std::invalid_argument("Data type not recognised!");
    }
}

static std::string CreateEffceeCheckFromDataTypeDAG(DataType dataType)
{
    switch (dataType)
    {
        // Note types are all signless in TOSA SPIRV so for example int8_t will always be uint8_t i.e. signedness = 0.
        case DataType::int4_t: return R"(CHECK-DAG: [[DATATYPE:%\w+]] = OpTypeInt 4 0)";
        case DataType::int8_t:
        case DataType::uint8_t: return R"(CHECK-DAG: [[DATATYPE:%\w+]] = OpTypeInt 8 0)";
        case DataType::int16_t:
        case DataType::uint16_t: return R"(CHECK-DAG: [[DATATYPE:%\w+]] = OpTypeInt 16 0)";
        case DataType::int32_t:
        case DataType::uint32_t: return R"(CHECK-DAG: [[DATATYPE:%\w+]] = OpTypeInt 32 0)";
        case DataType::float16_t: return R"(CHECK-DAG: [[DATATYPE:%\w+]] = OpTypeFloat 16)";
        case DataType::float32_t: return R"(CHECK-DAG: [[DATATYPE:%\w+]] = OpTypeFloat 32)";
        case DataType::bfloat16_t: return R"(CHECK-DAG: [[DATATYPE:%\w+]] = OpTypeFloat 16 BFLOAT16)";
        case DataType::bool_t: return R"(CHECK-DAG: [[DATATYPE:%\w+]] = OpTypeBool)";
        default: throw std::invalid_argument("Data type not recognised!");
    }
}

static effcee::Result CheckInputTensorDAG(const std::vector<int>& values,
                                          DataType dataType,
                                          const std::string& instruction,
                                          const std::string& goldenSPIRV)
{
    const std::string check = R"(
        CHECK:      [[LENGTH:%\w+]] = OpConstant {{%\w+}} )" +
                              std::to_string(values.size()) + R"(
        CHECK-DAG:      [[ARRAY:%\w+]] = OpTypeArray {{%\w+}} [[LENGTH]]
        )" + CreateEffceeCheckFromDataTypeDAG(dataType) +
                              R"(
        CHECK-DAG: [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        CHECK: [[INPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK:      [[INPUT:%\w+]] = OpGraphInputARM [[INPUT_TYPE]]
        CHECK:                       )" +
                              instruction + R"({{.*}}[[INPUT]]
    )";

    return effcee::Match(goldenSPIRV, check);
}

static effcee::Result CheckInputTensorNonDAG(const std::vector<int>& values,
                                             DataType dataType,
                                             const std::string& instruction,
                                             const std::string& goldenSPIRV)
{
    const std::string check = R"(
        CHECK:      [[LENGTH:%\w+]] = OpConstant {{%\w+}} )" +
                              std::to_string(values.size()) + R"(
        CHECK-DAG:      [[ARRAY:%\w+]] = OpTypeArray {{%\w+}} [[LENGTH]]
        )" + CreateEffceeCheckFromDataType(dataType) +
                              R"(
        CHECK-DAG: [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        CHECK: [[INPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK:      [[INPUT:%\w+]] = OpGraphInputARM [[INPUT_TYPE]]
        CHECK:                       )" +
                              instruction + R"({{.*}}[[INPUT]]
    )";

    return effcee::Match(goldenSPIRV, check);
}

static effcee::Result CheckInputTensorConstant(const std::vector<int>& values,
                                               DataType dataType,
                                               const std::string& instruction,
                                               const std::string& goldenSPIRV)
{
    const std::string check = R"(
        CHECK-DAG:      [[LENGTH:%\w+]] = OpConstant {{%\w+}} )" +
                              std::to_string(values.size()) + R"(
        CHECK-DAG:      [[ARRAY:%\w+]] = OpTypeArray {{%\w+}} [[LENGTH]]
        )" + CreateEffceeCheckFromDataTypeDAG(dataType) +
                              R"(
        CHECK-DAG: [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        CHECK: [[INPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK: [[INPUT:%\w+]] = OpGraphConstantARM [[INPUT_TYPE]]
        CHECK:                       )" +
                              instruction + R"({{.*}}[[INPUT]]
    )";

    return effcee::Match(goldenSPIRV, check);
}

static effcee::Result CheckInputTensorDTypeFirst(const std::vector<int>& values,
                                                 DataType dataType,
                                                 const std::string& instruction,
                                                 const std::string& goldenSPIRV)
{
    const std::string check = CreateEffceeCheckFromDataType(dataType) + R"(
        CHECK:      [[LENGTH:%\w+]] = OpConstant {{%\w+}} )" +
                              std::to_string(values.size()) + R"(
        CHECK-DAG:      [[ARRAY:%\w+]] = OpTypeArray {{%\w+}} [[LENGTH]]
        CHECK-DAG: [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        CHECK: [[INPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK:      [[INPUT:%\w+]] = OpGraphInputARM [[INPUT_TYPE]]
        CHECK:                       )" +
                              instruction + R"({{.*}}[[INPUT]]
    )";

    return effcee::Match(goldenSPIRV, check);
}

void CheckInputTensor(const std::vector<int>& values,
                      DataType dataType,
                      const std::string& instruction,
                      const std::string& goldenSPIRV)
{

    auto resultDAG = CheckInputTensorDAG(values, dataType, instruction, goldenSPIRV);
    auto resultNonDAG = CheckInputTensorNonDAG(values, dataType, instruction, goldenSPIRV);
    auto resultDTypeFirst = CheckInputTensorDTypeFirst(values, dataType, instruction, goldenSPIRV);
    auto resultConstant = CheckInputTensorConstant(values, dataType, instruction, goldenSPIRV);

    EXPECT_TRUE(resultDAG.message().empty() || resultNonDAG.message().empty() || resultDTypeFirst.message().empty() ||
                resultConstant.message().empty());
}

std::string GetLastInstructionIDs(const std::string& text, const std::string& instruction)
{
    std::string result;

    // Create a regex pattern with the specified instruction
    std::string pattern = instruction + R"(\s+((%\w+\s*)+(?=\n)))";
    std::regex instructionRegex(pattern);
    std::smatch match;
    std::string::const_iterator searchStart(text.cbegin());
    std::string lastMatch;

    // Search through the text for all occurrences of the specified instruction
    while (std::regex_search(searchStart, text.cend(), match, instructionRegex))
    {
        lastMatch = match[1];               // Capture the entire match
        searchStart = match.suffix().first; // Move the search start position forward
    }

    // If we found a match, extract the IDs
    if (!lastMatch.empty())
    {
        std::regex idRegex(R"(%\w+)");
        std::sregex_iterator it(lastMatch.begin(), lastMatch.end(), idRegex);
        std::sregex_iterator end;
        while (it != end)
        {
            result += " "; // Add a space between IDs
            result += it->str();
            ++it;
        }
    }

    return result;
}

static effcee::Result CheckOutputTensorNonDAG(const std::vector<int>& values,
                                              DataType dataType,
                                              const std::string& instruction,
                                              const std::string& goldenSPIRV)
{
    const std::string check = R"(
        CHECK:      [[LENGTH_SHAPE_ARRAY:%\w+]] = OpConstant %uint )" +
                              std::to_string(values.size()) + R"(
        )" + CreateEffceeCheckFromDataType(dataType) +
                              R"(
        CHECK:       [[ARRAY:%\w+]] = OpTypeArray %uint [[LENGTH_SHAPE_ARRAY]]
        CHECK:  [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        CHECK: [[OUTPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK:        [[OUTPUT:%\w+]] = OpExtInst [[OUTPUT_TYPE]] {{%\w+}} )" +
                              instruction + GetLastInstructionIDs(goldenSPIRV, instruction) + R"(
        CHECK:                        OpGraphSetOutputARM [[OUTPUT]]
    )";

    return effcee::Match(goldenSPIRV, check);
}

static effcee::Result CheckOutput4Tensor(const std::vector<int>& values,
                                         DataType dataType,
                                         const std::string& instruction,
                                         const std::string& goldenSPIRV)
{
    const std::string check = R"(
        CHECK:      [[LENGTH_SHAPE_ARRAY:%\w+]] = OpConstant %uint )" +
                              std::to_string(values.size()) + R"(
        CHECK:       [[ARRAY:%\w+]] = OpTypeArray %uint [[LENGTH_SHAPE_ARRAY]]
        CHECK:  [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        )" + CreateEffceeCheckFromDataType(dataType) +
                              R"(
        CHECK: [[OUTPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK:        [[OUTPUT:%\w+]] = OpExtInst [[OUTPUT_TYPE]] {{%\w+}} )" +
                              instruction + GetLastInstructionIDs(goldenSPIRV, instruction) + R"(
        CHECK:                        OpGraphSetOutputARM [[OUTPUT]]
    )";

    return effcee::Match(goldenSPIRV, check);
}

std::string ReverseString(const std::string& s)
{
    std::stack<std::string> stringStack;
    std::string instruction;
    for (auto it = s.begin(); it != s.end(); ++it)
    {
        if (*it != '\n')
        {
            instruction.push_back(*it);
        }
        else
        {
            stringStack.push(instruction);
            instruction.clear();
        }
    }
    std::string rString;

    while (!stringStack.empty())
    {
        rString += stringStack.top();
        stringStack.pop();
    }

    return rString;
}

void CheckOutput(const std::vector<int>& values,
                 DataType dataType,
                 const std::string& instruction,
                 const std::string& spirv)
{
    const auto rSpirv = ReverseString(spirv);

    const std::string check = R"(
        CHECK:                        OpGraphSetOutputARM [[OUTPUT]] %uint_0
        CHECK:        [[OUTPUT:%\w+]] = OpExtInst [[OUTPUT_TYPE]] {{%\w+}} )" +
                              instruction + R"(
        CHECK: [[OUTPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK:  [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        CHECK:       [[ARRAY:%\w+]] = OpTypeArray {{%\w+}} [[LENGTH]]
    )";
    const auto m = effcee::Match(rSpirv, check).message();
    std::cout << m;
    EXPECT_TRUE(m.empty());
    //    CHECK:      [[LENGTH:%\w+]] = OpConstant {{%\w+}} )" + std::to_string(values.size()) + R"(
}

static effcee::Result CheckOutputTensorDTypeFirst(const std::vector<int>& values,
                                                  DataType dataType,
                                                  const std::string& instruction,
                                                  const std::string& goldenSPIRV)
{
    const std::string check = CreateEffceeCheckFromDataType(dataType) + R"(
        CHECK:      [[LENGTH:%\w+]] = OpConstant {{%\w+}} )" +
                              std::to_string(values.size()) + R"(
        CHECK:       [[ARRAY:%\w+]] = OpTypeArray {{%\w+}} [[LENGTH]]
        CHECK:  [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        CHECK: [[OUTPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK:        [[OUTPUT:%\w+]] = OpExtInst [[OUTPUT_TYPE]] {{%\w+}} )" +
                              instruction + R"(
        CHECK:                        OpGraphSetOutputARM [[OUTPUT]] %uint_0
    )";

    return effcee::Match(goldenSPIRV, check);
}

static effcee::Result CheckOutputTensorTypeStruct(const std::vector<int>& values,
                                                  DataType dataType,
                                                  const std::string& instruction,
                                                  const std::string& goldenSPIRV)
{
    const std::string check = R"(
        CHECK-DAG:      [[LENGTH_SHAPE_ARRAY:%\w+]] = OpConstant %uint )" +
                              std::to_string(values.size()) + R"(
        )" + CreateEffceeCheckFromDataTypeDAG(dataType) +
                              R"(
        CHECK:       [[ARRAY:%\w+]] = OpTypeArray %uint [[LENGTH_SHAPE_ARRAY]]
        CHECK:  [[CONST_COMP:%\w+]] = OpConstantComposite [[ARRAY]] )" +
                              ExpandValues(values) + R"(
        CHECK: [[OUTPUT_TYPE:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} [[CONST_COMP]]
        CHECK: [[STRUCT_TYPE:%\w+]] = OpTypeStruct [[OUTPUT_TYPE]] {{%\w+}}
        CHECK: [[OUTPUT_STRUCT:%\w+]] = OpExtInst [[STRUCT_TYPE]] {{%\w+}} )" +
                              instruction + GetLastInstructionIDs(goldenSPIRV, instruction) + R"(
        CHECK: [[OUTPUT:%\w+]] = OpCompositeExtract [[OUTPUT_TYPE]] [[OUTPUT_STRUCT]]
        CHECK:                        OpGraphSetOutputARM [[OUTPUT]]
    )";

    return effcee::Match(goldenSPIRV, check);
}

void CheckOutputTensor(const std::vector<int>& values,
                       DataType dataType,
                       const std::string& instruction,
                       const std::string& goldenSPIRV)
{
    auto resultDataTypeFirst = CheckOutputTensorDTypeFirst(values, dataType, instruction, goldenSPIRV);
    auto resultNonDAG = CheckOutputTensorNonDAG(values, dataType, instruction, goldenSPIRV);
    auto resultTypeStruct = CheckOutputTensorTypeStruct(values, dataType, instruction, goldenSPIRV);
    auto res4 = CheckOutput4Tensor(values, dataType, instruction, goldenSPIRV);

    EXPECT_TRUE(resultDataTypeFirst.message().empty() || resultNonDAG.message().empty() ||
                resultTypeStruct.message().empty() || res4.message().empty());
}

void CheckConstCompositeTensor(const std::vector<int>& values,
                               const std::string& instruction,
                               const std::string& goldenSPIRV,
                               const int index)
{
    // Parse the golden spir-v, find the instruction, then split out its arguments for later reference

    std::istringstream iss(goldenSPIRV);
    std::string line;
    std::vector<std::string> args;

    while (std::getline(iss, line))
    {
        if (auto pos = line.find(instruction); pos != std::string::npos)
        {
            std::string args_list = std::move(line.substr(pos + instruction.size() + 1));

            iss.clear();
            iss.str(args_list);

            std::string arg;
            while (std::getline(iss, arg, ' '))
            {
                args.push_back(std::move(arg));
            }
        }
    }
    if (args.empty())
    {
        FAIL() << "CheckConstCompositeTensor(): Instruction: '" << instruction << "' not found in SPIR-V.";
    }
    if (values.empty())
    {
        FAIL() << "CheckConstCompositeTensor(): Must not have empty values vector.";
    }

    // Example path of Effcee check for MAX_POOL2D
    //  %9 = OpTypeTensorARM %uint %uint_1 %8
    //  %10 = OpConstantComposite %9 %uint_1 %uint_1
    //  %2 = OpExtInst %18 %3 MAX_POOL2D %10 %10 %14 %22

    auto replicatedComposite = true;
    for (auto& value : values)
    {
        if (value != values.front())
        {
            replicatedComposite = false;
            break;
        }
    }

    std::string check;
    if (replicatedComposite)
    {
        check = R"(
        CHECK: )" +
                args[index] + R"( = OpConstantCompositeReplicateEXT{{.*}})" + ExpandValues({values[0]}) + R"(
        )";
    }
    else
    {
        check = R"(
        CHECK: )" +
                args[index] + R"( = OpConstantComposite{{.*}})" + ExpandValues(values) + R"(
        )";
    }

    auto result = effcee::Match(goldenSPIRV, check);

    EXPECT_EQ(result.message(), "");
}

void CheckGraphConstant(const std::vector<int>& values,
                        DataType dataType,
                        const std::string& instruction,
                        const std::string& goldenSPIRV)
{
    const std::string check = CreateEffceeCheckFromDataType(dataType) + R"(
        CHECK:      [[TENSOR:%\w+]] = OpTypeTensorARM [[DATATYPE]] {{%\w+}} {{%\w+}}
        CHECK: [[GRAPH_CONST:%\w+]] = OpGraphConstantARM [[TENSOR]]
        CHECK:                        )" +
                              instruction + R"({{.*}}[[GRAPH_CONST]]
    )";

    auto result = effcee::Match(goldenSPIRV, check);

    EXPECT_EQ(result.message(), "");
}

void CheckConstant(DataType dataType,
                   const std::string& instruction,
                   const std::string& goldenSPIRV,
                   const uint32_t value,
                   const int index)
{
    // Example path of Effcee check for CLAMP
    //  5:             %uchar = OpTypeInt 8 0
    //  7:  %uchar_4294967295 = OpConstant %uchar 4294967295
    // 21:                 %2 = OpExtInst %13 %4 CLAMP %uchar_1 %uchar_4294967295 %16

    const std::string check = CreateEffceeCheckFromDataType(dataType) + R"(
        CHECK: [[CONST:%\w+]] = OpConstant [[DATATYPE]] )" +
                              std::to_string(value) + R"(
        CHECK:                  )" +
                              instruction + R"( )" + AddSpacers(index) + R"([[CONST]]
    )";

    auto result = effcee::Match(goldenSPIRV, check);

    EXPECT_EQ(result.message(), "");
}

void CheckBoolConstant(DataType dataType,
                       const std::string& instruction,
                       const std::string& goldenSPIRV,
                       const bool flag,
                       const int index)
{
    // Example path of Effcee check for RESCALE
    // 21:              %false = OpConstantFalse %bool
    // 32:                  %2 = OpExtInst %24 %4 RESCALE %uint_20 %uint_128 %8 %14 %true %false %false %false %true %27

    std::string flagStr = flag ? "%true" : "%false";
    const std::string check = CreateEffceeCheckFromDataType(dataType) + R"(
        CHECK: )" + flagStr + R"( = OpConstant{{True|False}} [[DATATYPE]]
        CHECK:                   )" +
                              instruction + R"( )" + AddSpacers(index) + flagStr; //+ R"(
    //)";

    auto result = effcee::Match(goldenSPIRV, check);

    EXPECT_EQ(result.message(), "");
}

} // namespace testutils
