#!/usr/bin/env python3

#
# Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

import sys

# Add tosa.py directory to paths python searches for modules
sys.path.append("./external/tosa_specification/tools")
sys.path.append('./external/ngp-runtime/scripts/tosaspv-1.0')

import tosa
import tosa_spirv

from .file_writer import FileWriter
from .operators import *


class SourceGenerator:
    def __init__(self):

        self.graph_header_path = "include/tosa/Graph.hpp"
        self.op_def_path = "src/OperatorDefinitions.cpp"
        self.parser_utils_path = "tools/parsers/src/ParserUtils.hpp"
        self.tosa_operator_enum_path = "include/tosa/OperatorEnum.hpp"
        self.serialization_attr_path = "tools/spirv2tosa/src/spirv2tosa.cpp"
        self.spec = tosa.TOSASpec("./external/tosa_specification/tosa.xml")

        self.tosa_version = "{}.{}.{}".format(self.spec.version_major, self.spec.version_minor, self.spec.version_patch)

        operators = Operators()
        self._operator_list = operators.operator_list
        self._operator_name_list = operators.operator_name_list

    def generate_graph_header(self):
        file = FileWriter(
            self.graph_header_path, "GRAPH OPERATOR HELPER FUNCTION BEGIN", "GRAPH OPERATOR HELPER FUNCTION END"
        )

        print("[Code Generator] Generating {} ...".format(file.get_dir()))

        file.start_block()
        for op in self._operator_list:
            generate_add_operator_declare(op, file)
            generate_add_operator_body(op, file)

    def generate_op_name_map(self):
        file = FileWriter(self.op_def_path,
                        "OPERATOR ENUM MAP SECTION BEGIN",
                         "OPERATOR ENUM MAP SECTION END")
        print("[Code Generator] Generating {} ...".format(file.get_dir()))

        file.start_block()
        file.write("switch (operatorType){")
        for op in self._operator_list:
            file.start_block()
            file.write(
                "case OperatorEnum::" + operator_name_to_pascal_case(op.name) + ":  return TOSA" + op.name + ";\n"
            )
            file.end_block()

    def generate_op_enum_map(self):
        file = FileWriter(self.parser_utils_path,
                        "OPERATOR NAME MAP SECTION BEGIN",
                         "OPERATOR NAME MAP SECTION END")
        print("[Code Generator] Generating {} ...".format(file.get_dir()))

        file.start_block()
        for op in self._operator_list:
            file.start_block()
            file.write("case Op_" + op.name + ":  return OperatorEnum::" + operator_name_to_pascal_case(op.name) + ";\n")
            file.end_block()

    def generate_op_enums(self):
        file = FileWriter(self.tosa_operator_enum_path, "OPERATOR ENUM SECTION BEGIN", "OPERATOR ENUM SECTION END")
        print("[Code Generator] Generating {} ...".format(file.get_dir()))


        file.write("enum class OperatorEnum" + "\n{\n")
        for name in self._operator_name_list:
            file.start_block()
            file.write(name + ",\n")
            file.end_block()

    def generate_op_names(self):
        file = FileWriter(self.op_def_path, "OPERATOR ENUM NAME SECTION BEGIN", "OPERATOR ENUM NAME SECTION END")
        print("[Code Generator] Generating {} ...".format(file.get_dir()))

        file.start_block()
        file.write("switch (op){")
        for name in self._operator_name_list:
            file.start_block()
            file.write("case OperatorEnum::" + name + ': { return {"' + name + '"};}\n')
            file.end_block()

    def generate_op_definitions(self):
        file = FileWriter(self.op_def_path, "OPERATOR DEFINITION SECTION BEGIN", "OPERATOR DEFINITION SECTION END")

        file.start_block()
        file.write("switch (op){")
        operator_definition_dict = {}
        for op in self._operator_list:
            key = generate_operator_definition(op)
            value = operator_name_to_pascal_case(op.name)
            operator_definition_dict.setdefault(key, []).append(value)

        file.set_indentation(8)
        for definition in operator_definition_dict:
            for op in operator_definition_dict[definition]:
                file.write("case OperatorEnum::" + op + ":\n")
            file.write("{\n")
            file.start_block()
            file.write(definition)
            file.end_block()
            file.write("}\n")

    def generate_serialization_attribute_definitions(self, attr_file_path):
        with open(attr_file_path, "r") as attr_file:
            file = FileWriter(self.serialization_attr_path, "GET TOSA ATTRIBUTE CODE GENERATION BEGIN", "GET TOSA ATTRIBUTE CODE GENERATION END")

            argument_map = {}
            for op in self._operator_list:
                operator_name = operator_name_to_pascal_case(op.name)
                if operator_name == "Fft2d":
                    operator_name = "FFT2d"
                elif operator_name == "Rfft2d":
                    operator_name = "RFFT2d"
                elif operator_name == "Matmul":
                    operator_name = "MatMul"
                argument_map[operator_name] = op.arguments

            file.start_block()
            attr_case = generate_attribute_case(attr_file, argument_map, "Const")
            while attr_case:
                file.write(attr_case)
                attr_case = generate_attribute_case(attr_file, argument_map, "Const")
            file.end_block()


def generate_operator_definition(op):
    input_size = 0
    output_size = 0
    attribute_definitions = []
    for idx, arg in enumerate(op.arguments):
        if has_argument_category_name(arg, "input") and not has_argument_category_name(arg, "attribute"):
            input_size += 1
            continue
        if has_argument_category_name(arg, "output"):
            output_size += 1
            continue

        if arg.tensor_element_type == "shape_t":
            meta_data = "Shape"
        else:
            meta_data = "None"

        category = tosa_spirv.TOSAType.type_category_for_arg(arg)
        tensor_type = "Ranked"
        if category == tosa_spirv.TOSAType.SHAPE:
            tensor_type = "Shape"
        elif category == tosa_spirv.TOSAType.ENUM:
            tensor_type = "Enum"
        elif category == tosa_spirv.TOSAType.SCALAR:
            tensor_type = "Scalar"
        elif arg.tensor_element_type == tosa_spirv.TOSAType.SHAPED_TENSOR:
            tensor_type = "Shaped"
        elif category == tosa_spirv.TOSAType.RANKED_TENSOR:
            tensor_type = "Ranked"
        elif category == tosa_spirv.TOSAType.TENSOR_LIST or category == tosa_spirv.TOSAType.TENSOR_LIST_UNIFORM_ETYPE:
            tensor_type = "TensorList"
        elif category == tosa_spirv.TOSAType.GRAPH:
            tensor_type = "Graph"

        if has_argument_category_name(arg, "input") and has_argument_category_name(arg, "attribute"):
            tensor_type = "GraphConstant"

        attribute_definitions.append("{Category::" + tensor_type + ", Metadata::" + meta_data + "}")

    attribute_size = len(attribute_definitions)

    indent = 12 * " "
    if attribute_size > 0:
        argument_string = "static constexpr std::array<AttributeDefinition," + str(attribute_size) + "> definitions{{"
        delimiter = " " if attribute_size < 1 else "\n" + indent + len(argument_string) * " "
        for attribute_definition in attribute_definitions:
            argument_string += attribute_definition + "," + delimiter
        index = 1 + len(delimiter)
        argument_string = argument_string[:-index] + "}};\n"

        argument_string += indent + "return { " + str(input_size) + ", " + str(output_size) + ", " + str(attribute_size)
        argument_string += ", definitions.data() };\n"
    else:
        argument_string = "return { " + str(input_size) + ", " + str(output_size) + ", 0, nullptr };\n"

    return argument_string


def generate_add_operator_body(op, file):
    num_outputs = 0
    for argument in op.arguments:
        if has_argument_category_name(argument, "output"):
            num_outputs += 1

    file.start_block()
    tosa_enum = "OperatorEnum::" + operator_name_to_pascal_case(op.name)
    first_line = "return AddOperator("
    num_indentation = len(first_line) + 8

    # if the operator arguments are larger than 5, use multiple lines
    delimiter = " " if len(op.arguments) < 5 else "\n" + num_indentation * " "
    first_line += tosa_enum + "," + delimiter
    end_string = "})[0];\n" if num_outputs == 1 else "});\n"
    inputs = "{"
    attributes = delimiter + "{"
    outputs = delimiter + "{"

    has_attributes = False
    for arg in op.arguments:
        is_input = has_argument_category_name(arg, "input")
        is_output = has_argument_category_name(arg, "output")
        is_attribute = has_argument_category_name(arg, "attribute")

        if is_input and not is_attribute:
            inputs += arg.name + ", "
        elif is_output:
            outputs += arg.name + ", "
        elif is_attribute:
            attributes += arg.name + ", "
            has_attributes = True

    file.write(first_line)
    inputs = inputs[:-2] + "},"
    file.append(inputs)

    if has_attributes:
        outputs = outputs[:-2] + "},"
        file.append(outputs)

        attributes = attributes[:-2] + end_string
        file.append(attributes)
    else:
        outputs = outputs[:-2] + end_string
        file.append(outputs)

    file.end_block()
    file.write("}\n\n")


def generate_add_operator_declare(op, file):
    op_name_camel_case = operator_name_to_pascal_case(op.name)
    file.write("/// Function used to add a {} operator to the Graph\n".format(op_name_camel_case))
    for argument in op.arguments:
        param_type = "ResId"
        if argument.type == "tensor_list_t":
            param_type = "std::vector<ResId>"
        elif has_argument_category_name(argument, "attribute"):
            param_type = "Attribute"
        elif has_argument_category_name(argument, "output"):
            param_type = "Tensor"

        file.write(
            "/// @param[in] {} {} representing the {} to the {} Operator\n".format(
                argument.name, param_type, argument.name, op_name_camel_case
            )
        )
    num_outputs = 0
    for argument in op.arguments:
        if has_argument_category_name(argument, "output"):
            num_outputs += 1

    if num_outputs == 1:
        function_header = "ResId Add{}Operator(".format(op_name_camel_case)
    else:
        function_header = "std::vector<ResId> Add{}Operator(".format(op_name_camel_case)
    file.write(function_header)

    num_indentation = len(function_header) + 4
    # if the operator arguments are larger than 5, use multiple lines
    delimiter = " " if len(op.arguments) < 5 else "\n" + num_indentation * " "

    for argument in op.arguments:
        if argument.type == "tensor_list_t":
            file.append("{} {}".format("const std::vector<ResId>&", argument.name))
        elif has_argument_category_name(argument, "attribute"):
            file.append("{} {}".format("const Attribute&", argument.name))
        elif has_argument_category_name(argument, "output"):
            file.append("{} {}".format("const Tensor&", argument.name))
        else:
            file.append("{} {}".format("const ResId&", argument.name))

        # don't write comma after last argument
        if argument is not op.arguments[-1]:
            file.append("," + delimiter)
        else:
            file.append(")\n")

    file.write("{\n")

# Looks for next `DEF_ATTRIBUTE(` line
def generate_attribute_case(attributes_file, argument_map, first_unsupported = ""):
    line = attributes_file.readline()
    while not line.startswith("DEF_ATTRIBUTE("):
        if line == "":
            return False
        line = attributes_file.readline()

    attr_params = line[14:].strip(", )\n").split(",")
    attr_name = attr_params[0].strip()
    arg_count = int(attr_params[1].strip())

    if attr_name == first_unsupported:
        return False

    # Getting argument names/positions from TOSA spec instead of attributes_file
    # to ensure correct order of attributes from params
    spec_args = argument_map[attr_name]
    spec_attr_names = []
    for arg in spec_args:
        if has_argument_category_name(arg, "attribute"):
            spec_attr_names.append(arg.name)

    result = "case tfsc::tosa::OperatorEnum::"
    if attr_name == "FFT2d":
        result += "Fft2d"
    elif attr_name == "RFFT2d":
        result += "Rfft2d"
    elif attr_name == "MatMul":
        result += "Matmul"
    else:
        result += attr_name
    result += ":\n"

    if arg_count > 0:
        result += "{\n"
        last_line = "return std::make_unique<Tosa{}Attribute>(".format(attr_name)
        for i in range(arg_count):
            line = attributes_file.readline()
            arg = line.strip(", )\n").split(",")
            arg_type = arg[0].strip()
            arg_name = arg[2].strip()
            arg_idx = spec_attr_names.index(arg_name)
            if arg[1].strip() == "V":
                result += "std::vector<{dtype}> {name} = {{attributes[{idx}].GetData().begin(), attributes[{idx}].GetData().end()}};\n".format(dtype = arg_type, name = arg_name, idx = arg_idx)
            else:
                if arg_type == "DType":
                    result += "DType {name} = GetTosaAccType(attributes[{idx}].GetData()[0]);".format(name = arg_name, idx = arg_idx)
                else:
                    result += "{dtype} {name} = static_cast<{dtype}>(attributes[{idx}].GetData()[0]);\n".format(dtype = arg_type, name = arg_name, idx = arg_idx)
            last_line += arg_name + ", "
        result += last_line.rstrip(", ") + ");\n"
        result += "}\n"
    else:
        result += "return std::make_unique<::tosa::Tosa{}Attribute>();\n".format(attr_name)

    return result

