#!/usr/bin/env python3

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

from src.source_generator import SourceGenerator
from src.test_generator import TestGenerator

if __name__ == '__main__':

    import argparse

    parser = argparse.ArgumentParser(description='Frontend Code Generator to generate Graph.hpp, Graph.cpp, and layer classes')
    parser.add_argument('-g', '--graph_enabled', help='-g 1 (by default) to generate include/Graph.hpp, -g 0 to disable', default=1)
    parser.add_argument('-d', '--definitions_enabled', help='-g 1 (by default) to generate include/OperatorDefinitions.hpp and src/OperatorDefinitions.cpp file, -d 0 to disable', default=1)
    parser.add_argument('-u', '--unit_test_enabled', help='-u 1 (by default) to generate unit tests with unimplementation exceptions to src/test/layerTests, -u 0 to disable', default=1)
    parser.add_argument('-f', '--force_generate', help='(-f 0 by default) -f 1 to enable force generate mode to overwrite any layers except for the one in IgnoreFiles.txt, -f 0 to disable', default=0)

    args = parser.parse_args()

    code_generator = SourceGenerator()

    if int(args.graph_enabled) == 1:
        code_generator.generate_graph_header()

    if int(args.definitions_enabled) == 1:
        code_generator.generate_op_definitions_src()
        code_generator.generate_op_definitions_header()
        code_generator.generate_op_enum_map()

    xml_path = "./external/tosa_specification/tosa.xml"
    input_template_dir = "python/templates"
    input_template_dict = {
        "parser_tests_path": "IParserTests.cpp"
    }

    for input_name, file in input_template_dict.items():
        input_template_dict[input_name] = input_template_dir + '/' + file

    tosa_serialization_parser_path = "parsers"
    layer_test_path = "src/test/layerTests"
    cmake_file_path = "python/ClassFiles.txt"

    if int(args.unit_test_enabled) == 1:
        code_generator = TestGenerator(input_template_dict,
                                       tosa_serialization_parser_path,
                                       layer_test_path,
                                       xml_path,
                                       cmake_file_path,
                                       bool(int(args.force_generate)))
        code_generator.generate_unit_tests()
        code_generator.generate_parser_tests()
