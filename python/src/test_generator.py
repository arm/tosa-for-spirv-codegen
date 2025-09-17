#!/usr/bin/env python3

#
# Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

import sys
import os

from collections import defaultdict

# Add tosa.py directory to paths python searches for modules
sys.path.append("./external/tosa_specification/tools")
# Add tosa_spirv.py directory to paths python searches for modules
sys.path.append("./external/ngp-runtime/scripts")

import tosa
import tosa_spirv

# unsupported operator group
unsupportedgroups = ['custom', 'control-flow', 'variable', 'shape']
TOSA_TO_SPV_TYPE_DECL_NAME_MAP = {
    'bool_t': 'bool',
    'i4_t': 'int4',
    'i8_t': 'int8',
    'i16_t': 'int16',
    'i32_t': 'int32',
    'i48_t': 'int64', # Not a typo. We map 48-bit TOSA to 64-bit SPIR-V
    'fp16_t' : 'float16',
    'fp32_t' : 'float32',
    'bf16_t' : 'bfloat16',
    'index_t' : 'int_index_t',
    'shape_t' : 'int32',
    'acc_type_t' : 'int32',
}


class FileWriter:
    def __init__(self, file_dir, tab=4, mode="w+"):
        self.file = open(file_dir, mode)

        self.file_dir = file_dir
        self.tab = tab
        self.indentation = 0
        self.start_line_num = 0
        self.end_line_num = 0
        self.lines = ""

    def start_block(self):
        self.indentation += self.tab

    def end_block(self):
        self.indentation -= self.tab
        if self.indentation < 0:
            self.indentation = 0

    def get_indentation(self):
        return self.indentation

    def set_indentation(self, indentation):
        self.indentation = indentation

    def get_tab(self):
        return self.tab

    def get_dir(self):
        return self.file_dir

    # write line with current indentation
    def write(self, line):
        self.file.write(" " * self.indentation + line)

    # write line without indentation (append content to the current line)
    def append(self, line):
        self.file.write(line)

    # copy from the input file
    def copy_from(self, file_dir, ignore_lines=0):
        origin_file = None

        try:
            origin_file = open(file_dir)
        except:
            raise Exception("Cannot open input file {}!".format(file_dir))

        read_lines = origin_file.readlines()

        for line in read_lines:
            if ignore_lines > 0:
                ignore_lines = ignore_lines - 1
                continue
            # avoid spaces before the '\n'
            elif line == "\n":
                self.append("\n")
            else:
                self.write(line)
        origin_file.close()

    def close(self):
        self.file.close()
        import subprocess
        import os
        from pathlib import Path

        try:
            result = subprocess.run(
                [
                    "clang-format",
                    "-i",
                    "--style=file",
                    self.file_dir,
                ],
                check=True,
                capture_output=True,
                text=True,
                cwd=Path.cwd(),
            )
            if result.stdout:
                print("[clang-format stdout]", result.stdout)
            if result.stderr:
                print("[clang-format stderr]", result.stderr)

        except subprocess.CalledProcessError as e:
            print("[clang-format failed]")
            print("stdout:", e.stdout)
            print("stderr:", e.stderr)


class TestGenerator:
    def __init__(
        self,
        input_template_dict,
        tosa_serialization_parser_path,
        xml_path,
        cmake_file_path
    ):

        self.input_template_dict = input_template_dict
        # set number of copyright lines which we can ignore from these input files
        self.num_copyright_lines = 5
        self.spec = tosa.TOSASpec(xml_path)
        self.tosa_serialization_parser_path = tosa_serialization_parser_path
        self.cmake_file_dir = cmake_file_path
        self.operator_exception = ['CUSTOM', 'CONST', "CONST_SHAPE", 'IDENTITY', 'DIM', 'VARIABLE', 'VARIABLE_WRITE', 'VARIABLE_READ']

        self.load_tosa_version()

        self._operator_list = []
        self._operator_name_list = []
        self.get_operators()

    def is_shape_arg(self, argument):
        return argument.type == 'shape_t'

    def load_tosa_version(self):
        self.tosa_version = "{}.{}.{}".format(
            self.spec.version_major, self.spec.version_minor, self.spec.version_patch
        )

    def get_operators(self):
        operator_list = []
        operator_name_list = []
        for group in self.spec.operatorgroups:
            if group.name not in unsupportedgroups:
                for op in group.operators:
                    if not op.name in self.operator_exception:
                        operator_list.append(op)
                        operator_name_list.append(
                            self.operator_name_to_pascal_case(op.name)
                        )
        return operator_name_list, operator_list

    # Header boilerplate
    def preamble(self):
        return """//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA {}. DO NOT EDIT!
// See tosa2spirv/python/code_generator.py and README

""".format(
            self.tosa_version
        )

    def preamble_free_edit(self):
        return """//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA {}.
// See tosa2spirv/python/code_generator.py and README

""".format(
            self.tosa_version
        )

    # convert operator name to PascalCase
    def name_to_pascal_case(self, name, separator):
        return "".join(w.capitalize() for w in name.split(separator))

    # convert operator name to PascalCase
    def operator_name_to_pascal_case(self, name):
        if name.upper() == "ARGMAX":
            return "ArgMax"
        if name.upper() == "INTDIV":
            return "IntDiv"
        if name.upper() == "FFT2D":
            return "Fft2d"
        if name.upper() == "RFFT2D":
            return "Rfft2d"
        return self.name_to_pascal_case(name, '_')

    def operator_name_to_pascal_case_tosa_attributes(self, name):
        if name.upper() == "ARGMAX":
            return "ArgMax"
        if name.upper() == "INTDIV":
            return "IntDiv"
        if name.upper() == "FFT2D":
            return "FFT2d"
        if name.upper() == "RFFT2D":
            return "RFFT2d"
        return self.name_to_pascal_case(name, '_')

    # convert operator name to PascalCase
    def operator_group_name_to_pascal_case(self, name):
        return self.name_to_pascal_case(name, "-")

    # make first letter lowercase
    def lowercase_first_letter(self, s):
        return "".join([s[:1].lower(), (s[1:])])

    # convert operator name to camelCase
    def operator_name_to_camel_case(self, name):
        camel_case = "".join(w.capitalize() for w in name.split("_"))
        camel_case = self.lowercase_first_letter(camel_case)
        return camel_case

    # convert operator name to underscore seperated e.g. MAX_POOL2D
    def operator_name_to_underscore_seperated(self, name):
        underscore_seperated = "_".join(w.capitalize() for w in name.split("_"))
        return underscore_seperated

    # Add casting for attributes where needed
    def get_attribute_casting(self, attr):
        if attr == "nan_mode":
            return "static_cast<NanPropagationMode>(nan_mode)"
        elif attr == "mode":
            return "static_cast<ResizeMode>(mode)"
        elif attr == "min_val":
            return "{static_cast<unsigned char>(min_val)}"
        elif attr == "max_val":
            return "{static_cast<unsigned char>(max_val)}"
        elif attr == "acc_type":
            return "DType_INT32"
        elif attr == "rounding_mode":
            return "static_cast<const RoundingMode>(rounding_mode)"
        else:
            return attr

    def apply_parser_datatype_fix(self, elementType):
        if elementType == "i8_t":
            return "int8_t"
        if elementType == "table_t":
            return "int16_t"
        else:
            return "int32_t"

    def find_input_argument(self, op):
        for idx, arg in enumerate(op.arguments):
            isTensorList = self.has_argument_type_name(arg, "tensor_list_t")
            isInput = self.has_argument_category_name(arg, "input")
            isInType = self.has_argument_tensor_element_type_name(
                arg, "in_t"
            ) or self.has_argument_tensor_element_type_name(arg, "in_out_t")
            isAttribute = self.has_argument_category_name(arg, "attribute")
            if isInput and isInType and not isAttribute and not isTensorList:
                return arg
        raise ValueError("find_input_argument: no inputs found.")

    def find_output_argument(self, op):
        output_args = []
        for idx, arg in enumerate(op.arguments):
            isTensorList = self.has_argument_type_name(arg, "tensor_list_t")
            isOutput = self.has_argument_category_name(arg, "output")
            if isOutput and not isTensorList:
                output_args.append(arg)
        if len(output_args) > 0:
            return output_args
        else:
            raise ValueError("find_output_argument: no outputs found.")

    def operator_walk_arguments_by_categories_and_tensor_element_type(
        self, op, catnames, types
    ):
        for arg in op.arguments:
            catindex = tosa_spirv.pick_arg_category_index(arg)
            if (
                arg.categories[catindex].name in catnames
                and arg.tensor_element_type in types
            ):
                yield arg

    def operator_walk_arguments_by_categories_and_type(self, op, catnames, types):
        for arg in op.arguments:
            catindex = tosa_spirv.pick_arg_category_index(arg)
            if arg.categories[catindex].name in catnames and arg.type in types:
                yield arg

    # resize fix, scale, offset and border changed to category input from category attribute incorrectly
    def print_parse_resize(self, file):
        file.copy_from(
            self.input_template_dict["parser_resize_fix_input_path"],
            self.num_copyright_lines,
        )

    def operator_walk_arguments_by_categories_and_not_tensor_element_type(
        self, op, catnames, types
    ):
        for arg in op.arguments:
            catindex = tosa_spirv.pick_arg_category_index(arg)
            if (
                arg.categories[catindex].name in catnames
                and arg.tensor_element_type not in types
            ):
                yield arg

    def operator_walk_arguments_by_categories_and_not_shapes(
        self, op, catnames, shapes
    ):
        for arg in op.arguments:
            catindex = tosa_spirv.pick_arg_category_index(arg)
            if arg.categories[catindex].name in catnames and arg.shape not in shapes:
                yield arg

    def get_group_of_operators(self):
        operator_map = {}
        for group in self.spec.operatorgroups:
            if group.name not in unsupportedgroups:
                operator_list_in_group = []
                for op in group.operators:
                    if not op.name in self.operator_exception:
                        operator_list_in_group.append(op)
                operator_map[group.name] = operator_list_in_group
        return operator_map

    def has_argument_category_name(self, arg, string):
        hasName = False
        for category in arg.categories:
            if category.name == string:
                hasName = True
        return hasName

    def is_dynamic_input(self, argument):
        is_input = any(cat.name == "input" for cat in argument.categories)
        is_not_attribute = not any(cat.name == "attribute" for cat in argument.categories)
        is_tensor = argument.type.endswith("_t")
        compile_time_const = tosa_spirv.TOSASPIRVSpec.is_operator_argument_ctc(argument)

        return is_input and is_not_attribute and is_tensor and not compile_time_const

    def is_constant_input(self, argument):
        is_input = any(cat.name == "input" for cat in argument.categories)
        is_not_attribute = not any(cat.name == "attribute" for cat in argument.categories)
        is_tensor = argument.type.endswith("_t")
        compile_time_const = tosa_spirv.TOSASPIRVSpec.is_operator_argument_ctc(argument)
        return is_input and is_not_attribute and is_tensor and compile_time_const

    def gather_arg_types(self, op):
        list = []
        for arg in op.arguments:
            type = self.get_arg_data_type(op, arg)
            if type not in list :
                list.append(type)
        return list

    def has_argument_of_type(self, op, string):
        for arg in op.arguments:
            if self.get_arg_data_type(op, arg) == string:
                return True
        return False

    def has_other_argument_of_type(self, op, string, argument):
        for arg in op.arguments:
            if arg.name != argument.name:
                if self.get_arg_type(op, arg) == string:
                    return True
        return False

    def has_other_non_output_argument_of_type(self, op, string, arg):
        for argument in op.arguments:
            print (str(op.name)  + " " + str(argument.name) + " " + string + " " + self.get_arg_type(op, arg))
            if arg.name != argument.name:
                if not self.has_argument_category_name(argument, 'output'):
                    if self.get_arg_type(op, argument) == string:
                        print("true: " + argument.name + " = "  + string)
                        return True
        return False

    def has_output_of_type(self, op, string):
        for arg in op.arguments:
            if self.has_argument_category_name(arg, 'output'):
                if self.get_arg_type(op, arg) == string:
                    return True
        return False

    def has_argument_category_name_containing(self, arg, string):
        hasName = False
        for category in arg.categories:
            if string in category.name:
                hasName = True
        return hasName

    def has_argument_type_name(self, arg, string):
        hasName = False
        if arg.type == string:
            hasName = True
        return hasName

    def count_op_args(self, op):
        numArgs = 0
        for arg in op.arguments:
            numArgs += 1
        return numArgs

    def has_argument_tensor_element_type_name(self, arg, string):
        hasName = False
        if arg.tensor_element_type == string:
            hasName = True
        return hasName

    def has_not_argument_tensor_element_type_name(self, arg, string):
        hasNotName = True
        if arg.tensor_element_type == string:
            hasNotName = False
        return hasNotName

    def has_argument_max_rank_greater_than_one(self, arg, op):
        if arg.rank[1] == "MAX_RANK":
            return True
        if arg.rank[1] == "MAX_RANK - 1":
            return True
        return int(arg.rank[1]) > 1

    def has_tosa_type_array(self, arg):
        return (
            tosa_spirv.TOSAType.type_category_for_arg(arg) == tosa_spirv.TOSAType.ARRAY
        )

    def get_cpp_data_type(self, datatype):
        if datatype == "int32":
            return "int32_t"
        elif datatype == "int16":
            return "int16_t"
        elif datatype == "int8":
            return "int8_t"
        elif datatype == "bool":
            return "bool"
        elif datatype == "float16":
            return "float"
        elif datatype == "float32":
            return "float"
        # default to Int32
        else:
            return "int32_t"

    def get_dtype(self, datatype):
        if datatype == "int32":
            return "DType::DType_INT32"
        elif datatype == "int16":
            return "DType::DType_INT16"
        elif datatype == "int8":
            return "DType::DType_INT8"
        elif datatype == "bool":
            return "DType::DType_BOOL"
        elif datatype == "float16":
            return "DType::DType_FP16"
        elif datatype == "float32":
            return "DType::DType_FP32"
        # default to Int32
        else:
            return "DType::DType_INT32"

    def reorder_acc_type_local_bound(self, attr_list):
        i = 0
        while i < len(attr_list) - 1:
            if attr_list[i] == 'acc_type' and attr_list[i+1] == 'local_bound':
                attr_list[i], attr_list[i+1] = attr_list[i+1], attr_list[i]
                i += 2
            elif attr_list[i] == 'local_bound' and attr_list[i+1] == 'acc_type':
                attr_list[i], attr_list[i+1] = attr_list[i+1], attr_list[i]
                i += 2
            else:
                i += 1
        return attr_list

    def generate_attribute_arguments(self, op, argument, file):
        numValues, values = self.get_num_values_template(argument)
        datatype = self.get_arg_data_type(op, argument)

        attr_name = []

        is_attr = False
        if int(numValues) > 1:
            # Add vector to store values for attributes
            if any(category.name == "attribute" for category in argument.categories) and tosa_spirv.TOSAType.type_category_for_arg(argument) != tosa_spirv.TOSAType.RANKED_TENSOR:
                is_attr = True
                if datatype == "bool":
                    file.write(
                        "bool {} = {};\n".format(
                            argument.name, "true" if values == "1" else "false"
                        )
                    )
                else:
                    file.write('std::vector<{}> {} = {{{}}};\n'.format(self.get_cpp_data_type(datatype),
                                                                       argument.name,
                                                                       values))
        else:
            # general attribute
            if any(category.name == "attribute" for category in argument.categories) and tosa_spirv.TOSAType.type_category_for_arg(argument) != tosa_spirv.TOSAType.RANKED_TENSOR:
                is_attr = True
                if datatype == 'bool':
                    file.write('bool {} = {};\n'.format(argument.name, 'true' if values == '1' and not op.name == "RFFT2D" else 'false'))
                else:
                        file.write('{} {} = {};\n'.format(self.get_cpp_data_type(datatype), argument.name, values))
        return is_attr, attr_name

    def get_arg_data_type(self, op, argument):
        # find a data type valid in type support for the operator tensor element type
        datatype = TOSA_TO_SPV_TYPE_DECL_NAME_MAP.get(
            op.typesupports[0].tymap.get(argument.tensor_element_type)
        )
        # if not in typesupport table revert to the tensor_element_type
        if datatype == None:
            # index_t not mentioned in support tables instead it follows integer value of 'levels' defind in spec
            if argument.tensor_element_type == 'index_t':
                datatype = 'int32'
            # for rescale if (scale32) mul_t=i32_t else mul_t=i16_t
            elif argument.tensor_element_type == 'mul_t':
                datatype = 'int32'
            else:
                datatype = TOSA_TO_SPV_TYPE_DECL_NAME_MAP.get(argument.tensor_element_type)
                if argument.type == 'bool_t':
                    datatype = 'bool'
                if datatype is None:
                    datatype = 'int32'
        return datatype

    def get_num_values_template(self, argument):
        if argument.shape == "[4]":
            numValues = "4"
            values = "1,1,1,1"
        elif argument.shape == "[3]":
            numValues = "3"
            values = "1,1,1"
        elif argument.shape == "[6]":
            numValues = "6"
            values = "1,1,1,1,1,1"
        elif argument.shape == "[2]":
            numValues = "2"
            values = "1,1"
        elif argument.shape == "-" or argument.shape == "[1]":
            numValues = "1"
            values = "1"
        elif argument.shape == "[NC]":
            numValues = "1"
            values = "1"
        elif argument.shape == "[rank(shape)]":
            numValues = "4"
            values = "1, 1, 1, 1"
        elif argument.shape == "[rank(shape1)]":
            numValues = "4"
            values = "1, 1, 1, 1"
        # PAD attribute: padding
        elif argument.shape == "[rank(shape1),2]":
            numValues = "4"
            values = "1, 1, 1, 1"
        # TABLE attribute: table
        elif argument.shape == '[TABLE_SIZE]':
            numValues = '1'
            values = '1'
        elif argument.shape == '[2*rank(shape1)]':
            numValues = '4'
            values = '1, 1, 1, 1'
        else:
            numValues = '4'
            values = "1,1,1,1"
        return numValues, values

    def generate_tensor_arguments(self, op, argument):
        numValues, values = self.get_num_values_template(argument)
        datatype = self.get_arg_data_type(op, argument)

        name_line = 'std::string {}Name = \"{}\";\n'.format(argument.name, argument.name)
        shape_line = 'std::vector<{}> {}Shape = {{{}}};\n'.format('int32_t', argument.name, values)

        valuesStr = ''

        if self.is_constant_input(argument):
            valuesStr = values
        if self.is_constant_input(argument) and values == '1' and datatype == "int32":
            valuesStr = '{0,0,0,1}'

        tensor_line = 'auto {}Tensor = std::make_unique<TosaSerializationTensor>({}Name, {}Shape, {}, std::vector<uint8_t>{{{}}});\n'.format(argument.name, argument.name,
                                                                                                          argument.name, self.get_dtype(datatype), valuesStr)
        tensor_line += "    tensors.push_back(std::move({}Tensor));\n".format(argument.name)
        if self.is_constant_input(argument):
            tensor_line += '    auto {}Op = std::make_unique<tosa::TosaSerializationOperator>(Op::Op_CONST, Attribute::Attribute_NONE, nullptr, std::vector<std::string>{{}}, std::vector<std::string>{{ {}Name }}, TosaOpLocation{{}});\n'.format(argument.name, argument.name)
            tensor_line += '    ops.push_back(std::move({}Op));\n'.format(argument.name)

        return [name_line, shape_line, tensor_line]

    def print_tensor_arguments(self, lines, file):
        if len(lines) == 0:
            raise ValueError("ParserTests Tensor Arguments Generation Failed!")
        for col_idx in range(3):
            for line_idx in range(len(lines)):
                file.write(lines[line_idx][col_idx])
            file.append("\n")

    def generate_test_arguments(self, op, name, file):

        for argument in op.arguments:
            if argument.shape == "[4]":
                numValues = "4"
                values = "1,1,1,1"
            elif argument.shape == "[3]":
                numValues = "3"
                values = "1,1,1"
            elif argument.shape == "[6]":
                numValues = "6"
                values = "1,1,1,1,1,1"
            elif argument.shape == "[2]":
                numValues = "2"
                values = "1,1"
            elif argument.shape == "-":
                numValues = "1"
                values = "1"
            elif argument.shape == "[NC]":
                numValues = "1"
                values = "1"
            elif argument.shape == "[rank(shape)]":
                numValues = "4"
                values = "1, 1, 1, 1"
            elif argument.shape == "[rank(shape1)]":
                numValues = "4"
                values = "1, 1, 1, 1"
            else:
                numValues = "1,1,1,1"
                values = "1,1,1,1"

            datatype = self.get_arg_data_type(op, argument)

            first_line = ''
            if self.is_dynamic_input(argument):
                first_line = 'auto {} = graph.AddInput(Tensor('.format(argument.name)
                file.write(first_line + 'DataType::{}_t, '.format(datatype))
                file.append('std::vector<unsigned int>{ ')
                file.append(str(numValues))
                file.append(' })')
                file.append(', 0)')
                file.append(';\n')

            elif any(category.name == "output" for category in argument.categories):
                first_line = 'auto {} = Tensor('.format(argument.name)
                file.write(first_line + 'DataType::{}_t, '.format(datatype))
                file.append('std::vector<unsigned int>{ ')
                file.append(str(numValues))
                file.append(' })')
                file.append(';\n')

            elif tosa_spirv.TOSASPIRVSpec.is_operator_argument_ctc(argument):
                    first_line = 'auto {}_attr = Attribute('.format(argument.name)
                    first_line += '{' + values + '}, '
                    first_line += 'DataType::{}_t);\n'.format(datatype)
                    file.write(first_line)
                    file.write("auto {} = graph.AddTensorConstant({}_attr);\n".format(argument.name, argument.name))

            elif tosa_spirv.TOSAType.type_category_for_arg(argument):
                    file.write("auto {} = Attribute(".format(argument.name))
                    file.append('{' +  str(values))
                    file.append('}, ')
                    if op.name == "CLAMP" and argument.name == "min_val" or argument.name == "max_val":
                        datatype = "int8"
                    file.append('DataType::{}_t);\n'.format(datatype))

            file.append("\n")

        for argument in op.arguments:
            if "input" in argument.name and argument.type == "tensor_list_t":
                file.write("std::vector<ResId> inputs;\n")
                file.write("inputs.push_back(input1);\n\n")

    def get_layer_variable_name(self, name):
        return name[0].lower() + name[1:] + "Layer"

    def get_validation_string(self, op):

        inputStr = []
        outputStr = []
        attributeStr = []

        for argument in op.arguments:
            # Number of values for the shape
            numValues, values = self.get_num_values_template(argument)

            datatype = self.get_arg_data_type(op, argument)
            datatype_str = str(datatype) + "_t"

            if self.has_argument_category_name(argument, 'attribute'):
                if tosa_spirv.TOSAType.type_category_for_arg(argument) == tosa_spirv.TOSAType.SCALAR or tosa_spirv.TOSAType.type_category_for_arg(argument) == tosa_spirv.TOSAType.ENUM:
                    shape = "1"
                    if op.name == "RFFT2D" and argument.name == "local_bound":
                        values = "0"
                    else:
                        values = "1"
                    if op.name == "CLAMP" and argument.type == "in_out_t":
                        datatype_str = "int8_t"
                    if self.has_argument_type_name(argument, 'bool_t'):
                        datatype_str = "bool_t"
                else:
                    shape = numValues
                attributeStr.append("{{" + values + "}, DataType::" + datatype_str + ", {" + shape + "}}")
            elif self.has_argument_category_name(argument, 'input'):
                if self.is_shape_arg(argument):
                    # Shape inputs are provided via CONST_SHAPE and lowered as tensor constants.
                    # Expect a 1-D int32 constant with length == numValues.
                    inputStr.append("{{" + values + "}, DataType::int32_t, {" + str(numValues) + "}}")
                elif not self.is_dynamic_input(argument):
                    if values == "1":
                        inputStr.append("{{1}, DataType::" + datatype_str + ", {1}}")
                    else:
                        inputStr.append("{std::initializer_list<uint32_t>{}, DataType::" + datatype_str + ", {" + values + "}}")
                else:
                    inputStr.append("{std::initializer_list<uint32_t>{}, DataType::" + datatype_str + ", {" + values + "}}")
            elif self.has_argument_category_name(argument, 'output'):
                outputStr.append("{DataType::" + datatype_str + ", {" + values + "}}")

        output =  "testutils::CheckModule(spirvModule,\n"
        output += "                       TOSA" + op.name + ",\n"
        output += "                       {" + ",".join(inputStr) + "},\n"
        output += "                       {" + ",".join(outputStr) + "},\n"
        output += "                       {" + ",".join(attributeStr) + "});\n"

        return output

    def generate_CMake_lists_parser_test(self):
        NUM_IND = 15
        operator_map = self.get_group_of_operators()
        operator_group_names = [
            self.operator_group_name_to_pascal_case(name)
            for name in operator_map.keys()
        ]
        file = FileWriter(self.cmake_file_dir, append=True)
        for name in sorted(operator_group_names):
            file.write(" " * NUM_IND + "{}OpsTest.cpp".format(name) + "\n")

    def generate_parser_tests(self):
        tosa_serialization_parser_test_path = (
            self.tosa_serialization_parser_path + "/test/"
        )

        operator_map = self.get_group_of_operators()

        # iterate through all operator groups
        for operator_group in operator_map.keys():
            # create group test file
            file = FileWriter(
                tosa_serialization_parser_test_path
                + "{}OpsTest.cpp".format(
                    self.operator_group_name_to_pascal_case(operator_group)
                )
            )
            file.write(self.preamble())
            file.copy_from(
                self.input_template_dict["parser_tests_path"], self.num_copyright_lines
            )
            # iterate through all operators in the operator groups
            for op in operator_map[operator_group]:
                print(
                    "[TosaSerializationParserTest Generator] Generating {} ...".format(
                        op.name
                    )
                )
                self.generate_parser_test_case(op, file)
            file.close()

    def generate_parser_test_case(self, op, file):
        op_pascal_case_name = self.operator_name_to_pascal_case(op.name)

        file.write("TEST(TOSA_FOR_SPIRV_CODEGEN_PARSER, {})\n".format(op_pascal_case_name))
        file.write("{\n")
        file.start_block()

        input_args = []
        output_args = []
        attribute_args = []

        for argument in op.arguments:
            if any(category.name == "input" for category in argument.categories):
                input_args.append(argument)
            elif any(category.name == "output" for category in argument.categories):
                output_args.append(argument)
            elif (
                any("attribute" in category.name for category in argument.categories)
                and tosa_spirv.TOSAType.type_category_for_arg(argument)
                == tosa_spirv.TOSAType.RANKED_TENSOR
            ):
                input_args.append(argument)
            else:
                attribute_args.append(argument)

        # Create Attribute
        # Attribute values
        attr_list = []
        for argument in attribute_args:
            is_attr, attr_name = self.generate_attribute_arguments(op, argument, file)


            if is_attr:
                if len(attr_name) > 0:
                    attr_list.extend(attr_name)
                else:
                    attr_list.append(argument.name)

        attr_list = self.reorder_acc_type_local_bound(attr_list)

        if len(attr_list) > 0:
            file.write("// Create Attribute\n")
            file.write("Tosa{}Attribute attribute(".format(self.operator_name_to_pascal_case_tosa_attributes(op.name)))

            # print attribute names
            for attr in attr_list:
                if attr == attr_list[-1]:
                    file.append("{});\n".format(self.get_attribute_casting(attr)))
                else:
                    file.append("{}, ".format(self.get_attribute_casting(attr)))

            file.append("\n")

        # Create tensors
        file.write("// Create Tensors\n")
        file.write("std::vector<std::unique_ptr<TosaSerializationTensor>> tensors;\n")
        file.write("std::vector<std::unique_ptr<TosaSerializationOperator>> ops;\n")
        file.write("std::vector<std::unique_ptr<TosaSerializationShape>> shapes;\n")

        # Emit shape constants (CONST_SHAPE) for shape_t inputs
        for argument in input_args:
            if self.is_shape_arg(argument):
                numValues, values = self.get_num_values_template(argument)
                file.write(f'std::string {argument.name}Name = "{argument.name}";\n')
                file.write(f'std::vector<int64_t> {argument.name}Dims = {{ {values} }};\n')
                file.write(f'std::vector<uint8_t> {argument.name}Bytes; '
                           f'TosaSerializationHandler::ConvertI64toU8({argument.name}Dims, {argument.name}Bytes);\n')
                file.write(f'auto {argument.name}Shape = std::make_unique<TosaSerializationShape>('
                           f'{argument.name}Name, static_cast<uint32_t>({numValues}), {argument.name}Bytes);\n')
                file.write(f'shapes.push_back(std::move({argument.name}Shape));\n')
                file.write(f'auto {argument.name}Op = std::make_unique<tosa::TosaSerializationOperator>('
                           f'Op::Op_CONST_SHAPE, Attribute::Attribute_ConstShapeAttribute, nullptr, '
                           f'std::vector<std::string>{{}}, std::vector<std::string>{{ {argument.name}Name }}, '
                           f'TosaOpLocation{{}});\n')
                file.write(f'ops.push_back(std::move({argument.name}Op));\n')
        file.append("\n")

        tensor_lines = []
        for argument in input_args:
            if self.is_shape_arg(argument):
                continue  # shapes handled above
            tensor_lines.append(self.generate_tensor_arguments(op, argument))
        for argument in output_args:
            tensor_lines.append(self.generate_tensor_arguments(op, argument))
        self.print_tensor_arguments(tensor_lines, file)

        # Get input tensors and input/output names
        input_output_args = input_args + output_args
        dynamic_input_args = []
        # Only dynamic inputs that are not coming from constant operators are added as inputs to block
        for arg in input_output_args:
            if self.is_dynamic_input(arg):
                dynamic_input_args.append(arg)

        tensors = ["{}Tensor".format(arg.name) for arg in input_output_args]
        input_names = ["{}Name".format(arg.name) for arg in input_args]
        dynamic_input_names = ["{}Name".format(arg.name) for arg in dynamic_input_args]
        output_names = ["{}Name".format(arg.name) for arg in output_args]

        # Create Operator
        file.write("// Create Operator\n")
        first_line = "auto op = std::make_unique<tosa::TosaSerializationOperator>("
        file.write(first_line + "Op::Op_{},\n".format(op.name))
        if len(attr_list) == 0:
            file.write(" " * len(first_line) + "Attribute::Attribute_NONE,\n")
            file.write(" " * len(first_line) + "nullptr,\n")
        else:
            file.write(' ' * len(first_line) + "Attribute::Attribute_{}Attribute,\n".format(self.operator_name_to_pascal_case_tosa_attributes(op.name)))
            file.write(' ' * len(first_line) + "&attribute,\n")
        file.write(' ' * len(first_line) + "std::vector<std::string>{ ")
        for input_name in input_names:
            if input_name == input_names[-1]:
                file.append("{} }},\n".format(input_name))
            else:
                file.append("{}, ".format(input_name))

        file.write(' ' * len(first_line) + "std::vector<std::string>{ ")
        for output_name in output_names:
            if output_name == output_names[-1]:
                file.append("{} }}".format(output_name))
            else:
                file.append("{}, ".format(output_name))
        file.append(", TosaOpLocation{});\n")

        file.write("ops.push_back(std::move(op));\n")

        file.append("\n")

        # Create a tosa single-op basic block
        file.write("// Create a tosa single-op basic block\n")
        file.write(
            "// The raw pointers of operators and tensors will be deleted by the destructor of the block\n"
        )

        first_line = "TosaSerializationBasicBlock block("
        file.write(first_line + "\"{}\",\n".format(op.name.lower()))
        file.write(' ' * len(first_line) + "\"main\",\n")
        file.write(' ' * len(first_line) + "std::move(ops),\n")
        file.write(' ' * len(first_line) + "std::move(tensors),\n")
        file.write(' ' * len(first_line) + "std::move(shapes),\n")
        file.write(' ' * len(first_line) + "{ ")
        for input_name in dynamic_input_names:
            if input_name == dynamic_input_names[-1]:
                file.append("{} }},\n".format(input_name))
            else:
                file.append("{}, ".format(input_name))

        file.write(" " * len(first_line) + "{ ")
        for output_name in output_names:
            if output_name == output_names[-1]:
                file.append("{} }}".format(output_name))
            else:
                file.append("{}, ".format(output_name))
        file.append(");\n")
        file.append("\n")

        # Create parser
        file.write("TosaSerializationParser parser(&block);\n")

        # Generate SPIRV Module
        file.write('const auto& spirvModule = parser.GenerateSPIRVModule("main");\n')
        file.append("\n")

        validation_string = self.get_validation_string(op)
        file.write(validation_string)

        file.end_block()
        file.write("}\n")
        file.write("\n")
