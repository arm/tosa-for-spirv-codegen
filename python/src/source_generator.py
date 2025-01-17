#!/usr/bin/env python3

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

import sys

# Add tosa.py directory to paths python searches for modules
sys.path.append('./external/tosa_specification/tools')
# Add tosa_spirv.py directory to paths python searches for modules
sys.path.append('./external/ngp-runtime/scripts')

import tosa
import tosa_spirv

from .file_writer import FileWriter
from .operators import *

class SourceGenerator:
    def __init__(self):

        self.graph_header_path = "include/Graph.hpp"
        self.op_def_path = "src/OperatorDefinitions.cpp"
        self.op_utils_path = "src/OperatorUtils.cpp"
        self.op_def_header_path = "include/OperatorDefinitions.hpp"
        self.spec = tosa.TOSASpec("./external/tosa_specification/tosa.xml")

        self.tosa_version = "{}.{}.{}".format(self.spec.version_major, self.spec.version_minor, self.spec.version_patch)

        operators = Operators()
        self._operator_list = operators.operator_list
        self._operator_name_list = operators.operator_name_list

    def generate_graph_header(self):
        file = FileWriter(self.graph_header_path)
        print("[Code Generator] Generating {} ...".format(file.get_dir()))

        file.start_block()
        for op in self._operator_list:
            generate_add_operator_declare(op, file)
            generate_add_operator_body(op, file)

    def generate_op_enum_map(self):
        file = FileWriter(self.op_utils_path)
        print("[Code Generator] Generating {} ...".format(file.get_dir()))

        file.start_block()

        for op in self._operator_list:
            file.start_block()
            file.write("case OperatorEnum::" + operator_name_to_pascal_case(op.name) + ":  return TOSA" + op.name + ";\n")
            file.end_block()

        file.write("    default: throw std::invalid_argument(\"Invalid OperatorEnum\");\n")
        file.write("}\n")

    def generate_op_definitions_header(self):
        file = FileWriter(self.op_def_header_path)
        print("[Code Generator] Generating {} ...".format(file.get_dir()))

        file.write("enum class OperatorEnum\n")
        file.write("{\n")

        for name in self._operator_name_list:
            file.start_block()
            file.write(name + ",\n")
            file.end_block()

        file.write("};\n")

    def generate_op_definitions_src(self):
        file = FileWriter(self.op_def_path)
        print("[Code Generator] Generating {} ...".format(file.get_dir()))

        file.write("std::string GetOperatorName(const OperatorEnum op)\n")
        file.write("{\n")
        file.start_block()
        file.write("switch(op)\n")
        file.write("{\n")

        for name in self._operator_name_list:
            file.start_block()
            file.write("case OperatorEnum::" + name +
                           ": { return {\"" + operator_name_to_pascal_case(name) + "\"};}\n")
            file.end_block()

        file.write("    default: throw std::invalid_argument(\"Invalid OperatorEnum\");\n")
        file.write("}\n")
        file.end_block()
        file.write('}\n\n')

        file.write("OperatorDefinition GetOperatorDefinition(const OperatorEnum op)\n")
        file.write("{\n")
        file.start_block()
        file.write("switch(op)\n")
        file.write("{\n")

        for op in self._operator_list:
            generate_operator_definition(op, file)

        file.set_indentation(8)
        file.write("default: return {};\n")
        file.write("}\n    }\n")

def generate_operator_definition(op, file):
    file.set_indentation(8)
    file.write("case OperatorEnum::" + operator_name_to_pascal_case(op.name) + ":\n")
    file.write("{\n")

    file.start_block()
    file.write("return {")
    file.end_block()
    file.set_indentation(0)

    inputs = []
    attributes = []
    outputs = []

    for idx, arg in enumerate(op.arguments):
        is_input = has_argument_category_name(arg, 'input')
        is_output = has_argument_category_name(arg, 'output')
        is_attribute = has_argument_category_name(arg, 'attribute')
        if is_input and is_attribute:
            is_input = False

        if arg.tensor_element_type == 'shape_t':
            meta_data = "Shape"
        else:
            meta_data = "None"

        category =  tosa_spirv.TOSAType.type_category_for_arg(arg)
        tensor_type = "Ranked"
        if category == tosa_spirv.TOSAType.ARRAY:
            tensor_type = "Array"
        elif category == tosa_spirv.TOSAType.ENUM:
            tensor_type = "Enum"
        elif category == tosa_spirv.TOSAType.SCALAR:
            tensor_type = "Scalar"
        elif arg.tensor_element_type == tosa_spirv.TOSAType.SHAPED_TENSOR:
            tensor_type = "Shaped"
        elif category == tosa_spirv.TOSAType.RANKED_TENSOR:
            tensor_type = "Ranked"
        elif (category == tosa_spirv.TOSAType.TENSOR_LIST
              or category == tosa_spirv.TOSAType.TENSOR_LIST_UNIFORM_ETYPE):
            tensor_type = "TensorList"
        elif category == tosa_spirv.TOSAType.GRAPH:
            tensor_type = "Graph"

        argument_string = ", Category::" + tensor_type + ", Metadata::" + meta_data + "}"

        if is_input:
            inputs.append("{Argument::Input" + argument_string)
        if is_output:
            outputs.append("{Argument::Output" + argument_string)
        if is_attribute:
            attributes.append("{Argument::Attribute" + argument_string)

    # make sure the order is inputs, attributes, outputs
    for input in inputs:
        file.write(input + ",\n")
        file.set_indentation(19)
    for attribute in attributes:
        file.write(attribute + ",\n")
    for output in outputs[:len(outputs) -1]:
        file.write(output + ",\n")

    file.write(outputs[-1] + "};\n")
    file.set_indentation(8)
    file.write("}\n")

def generate_add_operator_body(op, file):
    num_outputs = 0
    for argument in op.arguments:
        if has_argument_category_name(argument, 'output'):
            num_outputs +=1

    file.start_block()
    tosa_enum =  "OperatorEnum::" + operator_name_to_pascal_case(op.name)
    first_line = 'return AddOperator('
    num_indentation = len(first_line)  + 8

    # if the operator arguments are larger than 5, use multiple lines
    delimiter =  " " if len(op.arguments) < 5 else "\n" + num_indentation * " "
    first_line += tosa_enum + "," + delimiter
    end_string = "})[0];\n" if num_outputs == 1 else "});\n"
    inputs =  "{"
    attributes = delimiter + "{"
    outputs = delimiter + "{"

    has_attributes = False
    for arg in op.arguments:
        is_input = has_argument_category_name(arg, 'input')
        is_output = has_argument_category_name(arg, 'output')
        is_attribute = has_argument_category_name(arg, 'attribute')

        if is_input and not is_attribute:
            inputs += arg.name + ", "
        elif is_output:
            outputs +=  arg.name + ", "
        elif is_attribute:
            attributes +=  arg.name + ", "
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
    file.write('}\n\n')

def generate_add_operator_declare(op, file):
    op_name_camel_case = operator_name_to_pascal_case(op.name)
    file.write('/// Function used to add a {} operator to the Graph\n'.format(op_name_camel_case))
    for argument in op.arguments:
        file.write('/// @param[in] {} Tensor representing the {} to the {} Operator\n'
                   .format(argument.name, argument.name, op_name_camel_case))
    num_outputs = 0
    for argument in op.arguments:
        if has_argument_category_name(argument, 'output'):
            num_outputs +=1

    if num_outputs == 1:
        function_header = 'ResId Add{}Operator('.format(op_name_camel_case)
    else:
        function_header = 'std::vector<ResId> Add{}Operator('.format(op_name_camel_case)
    file.write(function_header)

    num_indentation = len(function_header) + 4
    # if the operator arguments are larger than 5, use multiple lines
    delimiter =  " " if len(op.arguments) < 5 else "\n" + num_indentation * " "

    for argument in op.arguments:
        if argument.type == "tensor_list_t":
            file.append('{} {}'.format("const std::vector<Tensor>&", argument.name))
        else: #"tensor_t"
            file.append('{} {}'.format("const Tensor&", argument.name))

        # don't write comma after last argument
        if argument is not op.arguments[-1]:
            file.append(',' + delimiter)
        else:
            file.append(')\n')

    file.write('{\n')