#!/usr/bin/env python3

#
# Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

from src.source_generator import SourceGenerator
from src.test_generator import TestGenerator

if __name__ == "__main__":

    import argparse

    parser = argparse.ArgumentParser(
        description="Frontend Code Generator to generate Graph.hpp, Graph.cpp, and layer classes"
    )
    parser.add_argument(
        "-g", "--graph_enabled", help="-g 1 (by default) to generate include/Graph.hpp, -g 0 to disable", default=1
    )
    parser.add_argument(
        "-d",
        "--definitions_enabled",
        help="-g 1 (by default) to generate include/OperatorEnum.hpp and src/OperatorDefinitions.cpp file,"
             " -d 0 to disable",
        default=1,
    )
    parser.add_argument(
        "-u",
        "--unit_test_enabled",
        help="-u 1 (by default) to generate unit tests, -u 0 to disable",
        default=1,
    )
    # DEPRECATED, For backwards compatibility with pipeline scripts for now
    parser.add_argument(
        "-f",
        "--force_generate",
        help="(-f 0 by default) -f 1 to enable force generate mode to overwrite any layers"
             " except for the one in IgnoreFiles.txt, -f 0 to disable",
        default=0,
    )

    parser.add_argument(
        "--serialization_attr_path",
        help="--serialization_attr_path <path_to_file> to specify a custom .def file for TOSA serialization_lib attributes",
        default="external/serialization_lib/include/attribute.def"
    )

    args = parser.parse_args()

    code_generator = SourceGenerator()

    if int(args.graph_enabled) == 1:
        code_generator.generate_graph_header()

    if int(args.definitions_enabled) == 1:
        code_generator.generate_op_names()
        code_generator.generate_op_name_map()
        code_generator.generate_op_enum_map()
        code_generator.generate_op_definitions()
        code_generator.generate_op_enums()
        if args.serialization_attr_path:
            code_generator.generate_serialization_attribute_definitions(args.serialization_attr_path)

    xml_path = "./external/tosa_specification/tosa.xml"
    input_template_dir = "python/templates"
    input_template_dict = {"parser_tests_path": "IParserTests.cpp"}

    for input_name, file in input_template_dict.items():
        input_template_dict[input_name] = input_template_dir + "/" + file

    tosa_serialization_parser_path = "tools/parsers"
    cmake_file_path = "python/ClassFiles.txt"

    if int(args.unit_test_enabled) == 1:
        code_generator = TestGenerator(
            input_template_dict,
            tosa_serialization_parser_path,
            xml_path,
            cmake_file_path,
        )
        code_generator.generate_parser_tests()
