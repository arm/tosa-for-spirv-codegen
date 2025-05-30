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
unsupportedgroups = ["custom", "control-flow", "variable"]
TOSA_TO_SPV_TYPE_DECL_NAME_MAP = {
    "bool_t": "bool",
    "i4_t": "int4",
    "i8_t": "int8",
    "i16_t": "int16",
    "i32_t": "int32",
    "i48_t": "int64",  # Not a typo. We map 48-bit TOSA to 64-bit SPIR-V
    "fp16_t": "float16",
    "fp32_t": "float32",
    "bf16_t": "bfloat16",
    "index_t": "int_index_t",
    "shape_t": "int_shape_t",
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
        layer_test_path,
        xml_path,
        cmake_file_path,
        force_generate,
    ):

        self.input_template_dict = input_template_dict
        # set number of copyright lines which we can ignore from these input files
        self.num_copyright_lines = 5
        self.layer_test_path = layer_test_path
        self.spec = tosa.TOSASpec(xml_path)
        self.tosa_serialization_parser_path = tosa_serialization_parser_path
        self.cmake_file_dir = cmake_file_path
        self.force_generate = force_generate
        self.operator_exception = [
            "CUSTOM",
            "CONST",
            "IDENTITY",
            "DIM",
            "VARIABLE",
            "VARIABLE_WRITE",
            "VARIABLE_READ",
        ]

        self.load_tosa_version()

        self.dictOpAttributes = self.get_tosa_attribute_dict()

        self._operator_list = []
        self._operator_name_list = []
        self.get_operators()

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
        return self.name_to_pascal_case(name, "_")

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

    def get_tosa_attribute_dict(self):
        def def_value():
            return "[UNIMPLEMENTED ATTRIBUTE: Add to dictOpAttributes dictionary in code_generator.py]"

        dictOpAttributes = defaultdict(def_value)

        dictOpAttributes["ARGMAX"] = "AxisAttribute"
        dictOpAttributes["AVG_POOL2D"] = "PoolAttribute"
        dictOpAttributes["CONV2D"] = "ConvAttribute"
        dictOpAttributes["CONV3D"] = "ConvAttribute"
        dictOpAttributes["DEPTHWISE_CONV2D"] = "ConvAttribute"
        dictOpAttributes["FULLY_CONNECTED"] = "FullyConnectedAttribute"
        dictOpAttributes["MATMUL"] = "MatMulAttribute"
        dictOpAttributes["MAX_POOL2D"] = "PoolAttribute"
        dictOpAttributes["TRANSPOSE_CONV2D"] = "TransposeConvAttribute"
        dictOpAttributes["CLAMP"] = "ClampAttribute"
        dictOpAttributes["RESERVED"] = "Attribute_NONE"
        dictOpAttributes["SIGMOID"] = "Attribute_NONE"
        dictOpAttributes["TANH"] = "Attribute_NONE"
        dictOpAttributes["ADD"] = "Attribute_NONE"
        dictOpAttributes["ARITHMETIC_RIGHT_SHIFT"] = "ArithmeticRightShiftAttribute"
        dictOpAttributes["BITWISE_AND"] = "Attribute_NONE"
        dictOpAttributes["BITWISE_OR"] = "Attribute_NONE"
        dictOpAttributes["BITWISE_XOR"] = "Attribute_NONE"
        dictOpAttributes["INTDIV"] = "Attribute_NONE"
        dictOpAttributes["LOGICAL_AND"] = "Attribute_NONE"
        dictOpAttributes["LOGICAL_LEFT_SHIFT"] = "Attribute_NONE"
        dictOpAttributes["LOGICAL_RIGHT_SHIFT"] = "Attribute_NONE"
        dictOpAttributes["LOGICAL_OR"] = "Attribute_NONE"
        dictOpAttributes["LOGICAL_XOR"] = "Attribute_NONE"
        dictOpAttributes["MAXIMUM"] = "Attribute_NONE"
        dictOpAttributes["MINIMUM"] = "Attribute_NONE"
        dictOpAttributes["MUL"] = "MulAttribute"
        dictOpAttributes["POW"] = "PowAttribute"
        dictOpAttributes["SUB"] = "SubAttribute"
        dictOpAttributes["TABLE"] = "TableAttribute"
        dictOpAttributes["ABS"] = "Attribute_NONE"
        dictOpAttributes["BITWISE_NOT"] = "Attribute_NONE"
        dictOpAttributes["CEIL"] = "Attribute_NONE"
        dictOpAttributes["CLZ"] = "Attribute_NONE"
        dictOpAttributes["EXP"] = "Attribute_NONE"
        dictOpAttributes["FLOOR"] = "Attribute_NONE"
        dictOpAttributes["LOG"] = "Attribute_NONE"
        dictOpAttributes["LOGICAL_NOT"] = "Attribute_NONE"
        dictOpAttributes["NEGATE"] = "NegateAttribute"
        dictOpAttributes["RECIPROCAL"] = "Attribute_NONE"
        dictOpAttributes["RSQRT"] = "Attribute_NONE"
        dictOpAttributes["SELECT"] = "Attribute_NONE"
        dictOpAttributes["EQUAL"] = "Attribute_NONE"
        dictOpAttributes["GREATER"] = "Attribute_NONE"
        dictOpAttributes["GREATER_EQUAL"] = "Attribute_NONE"
        dictOpAttributes["REDUCE_ANY"] = "AxisAttribute"
        dictOpAttributes["REDUCE_ALL"] = "AxisAttribute"
        dictOpAttributes["REDUCE_MAX"] = "AxisAttribute"
        dictOpAttributes["REDUCE_MIN"] = "AxisAttribute"
        dictOpAttributes["REDUCE_PRODUCT"] = "AxisAttribute"
        dictOpAttributes["REDUCE_SUM"] = "AxisAttribute"
        dictOpAttributes["CONCAT"] = "AxisAttribute"
        dictOpAttributes["PAD"] = "PadAttribute"
        dictOpAttributes["RESHAPE"] = "ReshapeAttribute"
        dictOpAttributes["REVERSE"] = "AxisAttribute"
        dictOpAttributes["SLICE"] = "SliceAttribute"
        dictOpAttributes["TILE"] = "TileAttribute"
        dictOpAttributes["TRANSPOSE"] = "TransposeAttribute"
        dictOpAttributes["GATHER"] = "Attribute_NONE"
        dictOpAttributes["SCATTER"] = "Attribute_NONE"
        dictOpAttributes["RESIZE"] = "ResizeAttribute"
        dictOpAttributes["CAST"] = "Attribute_NONE"
        dictOpAttributes["RESCALE"] = "RescaleAttribute"
        dictOpAttributes["CONST"] = "Attribute_NONE"
        dictOpAttributes["IDENTITY"] = "Attribute_NONE"
        dictOpAttributes["CUSTOM"] = "CustomAttribute"
        dictOpAttributes["COND_IF"] = "CondIfAttribute"
        dictOpAttributes["WHILE_LOOP"] = "WhileLoopAttribute"
        dictOpAttributes["FFT2D"] = "FFTAttribute"
        dictOpAttributes["RFFT2D"] = "RFFTAttribute"
        dictOpAttributes["ERF"] = "Attribute_NONE"
        dictOpAttributes["DIM"] = "AxisAttribute"

        return dictOpAttributes

    # convert operator name to underscore seperated e.g. MAX_POOL2D
    def operator_name_to_underscore_seperated(self, name):
        underscore_seperated = "_".join(w.capitalize() for w in name.split("_"))
        return underscore_seperated

    # apply attribute exception fixes
    def apply_attribute_exception_fixes(self, opname, attrname):
        if opname == "CLAMP" and attrname == "max_val":
            return "attribute.max_int()"
        if opname == "CLAMP" and attrname == "min_val":
            return "attribute.min_int()"
        if opname == "TRANSPOSE_CONV2D" and attrname == "out_shape":
            return "attribute.output_shape()"
        if opname == "PAD" and attrname == "pad_const":
            return "attribute.pad_const_int()"
        if opname == "AVG_POOL2D" and attrname == "acc_size":
            return "static_cast<int>(ConvertAvgPoolAccType(attribute))"
        if opname == "RESIZE" and attrname == "mode":
            return "static_cast<int>(attribute.mode())"
        if opname == "TABLE" and attrname == "table":
            return "attribute.table()"
        else:
            return "attribute." + attrname + "()"

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

    def get_layer_test_path(self, name):
        return self.layer_test_path + "/{}Tests.cpp".format(name)

    def check_layer_test(self, name):
        return os.path.exists(self.get_layer_test_path(name))

    def has_argument_category_name(self, arg, string):
        hasName = False
        for category in arg.categories:
            if category.name == string:
                hasName = True
        return hasName

    def is_resize_attribute(self, op, arg):
        if op.name == "RESIZE":
            if arg.name == "scale" or arg.name == "offset" or arg.name == "border":
                return True
        return False

    def is_reshape_attribute(self, op, arg):
        if op.name == "RESHAPE":
            if arg.name == "shape":
                return True
        return False

    def is_transpose_attribute(self, op, arg):
        if op.name == "TRANSPOSE":
            if arg.name == "perms":
                return True
        return False

    def is_slice_attribute(self, op, arg):
        if op.name == "SLICE":
            if arg.name == "start" or arg.name == "size":
                return True
        return False

    def is_tile_attribute(self, op, arg):
        if op.name == "TILE":
            if arg.name == "multiples":
                return True
        return False

    def is_rescale_graph_constant(self, op, arg):
        if op.name == "RESCALE":
            if arg.name == "multiplier" or arg.name == "shift":
                return True
        return False

    def has_multiple_inputs(self, op):
        inputCount = 0
        for arg in op.arguments:
            for cat in arg.categories:
                if cat.name == "input":
                    inputCount += 1
        if inputCount > 1:
            return True
        else:
            return False

    def has_attributes(self, op):
        attrCount = 0
        print(op.name)
        for arg in op.arguments:
            for cat in arg.categories:
                print(cat.name)
                if cat.name == "attribute":
                    print(op.name + " true")
                    attrCount += 1
        if attrCount > 0:
            return True
        else:
            return False

    def has_array_attributes(self, op):
        for arg in op.arguments:
            if (
                tosa_spirv.TOSAType.type_category_for_arg(arg)
                == tosa_spirv.TOSAType.ARRAY
            ):
                return True
            # account for ops with arrays that are incorrectly catagorized as not being arrays
            elif op.name in ["TRANSPOSE"]:
                return True
        return False

    def get_arg_type(self, op, arg):
        datatype = TOSA_TO_SPV_TYPE_DECL_NAME_MAP.get(
            op.typesupports[0].tymap.get(arg.tensor_element_type)
        )
        if datatype == None:
            if arg.tensor_element_type == "acc_size_t":
                datatype = "int32"
                # index_t not mentioned in support tables instead it follows integer value of 'levels' defind in spec
            elif arg.tensor_element_type == "index_t":
                datatype = "int32"
            elif arg.tensor_element_type == "shape_t":
                datatype = "int32"
            elif arg.tensor_element_type == "i32_t":
                datatype = "int32"
            elif arg.tensor_element_type == "resize_mode_t":
                datatype = "int32"
            elif arg.tensor_element_type == "mul_t":
                datatype = "int32"
            elif arg.tensor_element_type == "bool_t":
                datatype = "bool"
            else:
                datatype = "int8"
        return datatype

    def gather_arg_types(self, op):
        list = []
        for arg in op.arguments:
            type = self.get_arg_type(op, arg)
            if type not in list:
                list.append(type)
        return list

    def has_argument_of_type(self, op, string):
        for arg in op.arguments:
            if self.get_arg_type(op, arg) == string:
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
            print(
                str(op.name)
                + " "
                + str(argument.name)
                + " "
                + string
                + " "
                + self.get_arg_type(op, arg)
            )
            if arg.name != argument.name:
                if not self.has_argument_category_name(argument, "output"):
                    if self.get_arg_type(op, argument) == string:
                        print("true: " + argument.name + " = " + string)
                        return True
        return False

    def has_output_of_type(self, op, string):
        for arg in op.arguments:
            if self.has_argument_category_name(arg, "output"):
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

    def fix_op(self, op):
        op_fix_dict = {
            "RESCALE": {
                "multiplier": {"shape": "[1]", "tensor-element-type": "i32_t"},
                "shift": {"shape": "[1]", "tensor-element-type": "i32_t"},
            },
            "RESIZE": {
                "scale": {"tensor-element-type": "i16_t"},
                "offset": {"tensor-element-type": "i16_t"},
                "border": {"tensor-element-type": "i16_t"},
            },
            "AVG_POOL2D": {
                "input_zp": {"tensor-element-type": "i32_t"},
                "output_zp": {"tensor-element-type": "i32_t"},
            },
            "TABLE": {"table": {"tensor-element-type": "i16_t"}},
        }
        if op.name in op_fix_dict.keys():
            for argument in op.arguments:
                if argument.name in op_fix_dict[op.name].keys():
                    for item in op_fix_dict[op.name][argument.name].keys():
                        if item == "shape":
                            argument.shape = op_fix_dict[op.name][argument.name][item]
                        elif item == "tensor-element-type":
                            argument.tensor_element_type = op_fix_dict[op.name][
                                argument.name
                            ][item]
        return op

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

    def generate_attribute_arguments(self, op, argument, file):
        numValues, values = self.get_num_values_template(argument)
        datatype = self.get_arg_data_type(op, argument)

        attr_name = []

        is_attr = False
        if int(numValues) > 1 or (
            op.name == "RESCALE"
            and (argument.name == "multiplier" or argument.name == "shift")
        ):
            # Add vector to store values for attributes
            if (
                any(category.name == "attribute" for category in argument.categories)
                and tosa_spirv.TOSAType.type_category_for_arg(argument)
                != tosa_spirv.TOSAType.RANKED_TENSOR
                or self.check_fake_input(op, argument)
            ):
                is_attr = True
                if datatype == "bool":
                    file.write(
                        "bool {} = {};\n".format(
                            argument.name, "true" if values == "1" else "false"
                        )
                    )
                else:
                    file.write(
                        "std::vector<{}> {} = {{{}}};\n".format(
                            self.get_cpp_data_type(datatype), argument.name, values
                        )
                    )
            # work around attribute incorrectly marked as input
            elif (
                self.is_reshape_attribute(op, argument)
                or self.is_tile_attribute(op, argument)
                or self.is_transpose_attribute(op, argument)
            ):
                is_attr = True
                file.write(
                    "std::vector<int32_t> {} = {{{}}};\n".format(argument.name, values)
                )
        else:
            # single value attribute
            # fix resize mode attribute
            if argument.tensor_element_type == "resize_mode_t":
                is_attr = True
                file.write(
                    "ResizeMode {} = ResizeMode_BILINEAR;\n".format(argument.name)
                )

            # fix PAD pad_const attribute, and CLAMP min_val/max_val attribute
            elif (
                op.name == "PAD" and argument.name == "pad_const"
            ) or op.name == "CLAMP":
                is_attr = True
                file.write("int32_t {}_int = {};\n".format(argument.name, values))
                file.write("float {}_fp = {};\n".format(argument.name, values))
                attr_name = [
                    "{}_int".format(argument.name),
                    "{}_fp".format(argument.name),
                ]

            # general attribute
            elif (
                any(category.name == "attribute" for category in argument.categories)
                and tosa_spirv.TOSAType.type_category_for_arg(argument)
                != tosa_spirv.TOSAType.RANKED_TENSOR
                or self.check_fake_input(op, argument)
            ):
                is_attr = True
                if datatype == "bool":
                    file.write(
                        "bool {} = {};\n".format(
                            argument.name, "true" if values == "1" else "false"
                        )
                    )
                else:
                    # Fix for unused attribute in (TOSA2SPIRV_PARSER, AvgPool2d)
                    if argument.name == "acc_size":
                        pass
                    else:
                        file.write(
                            "{} {} = {};\n".format(
                                self.get_cpp_data_type(datatype), argument.name, values
                            )
                        )

            # work around attribute incorrectly marked as input
            elif self.is_reshape_attribute(op, argument) or self.is_transpose_attribute(
                op, argument
            ):
                is_attr = True
                file.write("int32_t {} = {};\n".format(argument.name, values))

        return is_attr, attr_name

    def get_arg_data_type(self, op, argument):
        # find a data type valid in type support for the operator tensor element type
        datatype = TOSA_TO_SPV_TYPE_DECL_NAME_MAP.get(
            op.typesupports[0].tymap.get(argument.tensor_element_type)
        )
        # if not in typesupport table revert to the tensor_element_type
        if datatype == None:
            # hack for inconsistency in avg_pool2d fixed in tosa 1.0
            if argument.tensor_element_type == "acc_size_t":
                datatype = "int32"
                # index_t not mentioned in support tables instead it follows integer value of 'levels' defined in spec
            elif argument.tensor_element_type == "index_t":
                datatype = "int32"
            elif argument.tensor_element_type == "shape_t":
                datatype = "int32"
            # for rescale if (scale32) mul_t=i32_t else mul_t=i16_t
            elif argument.tensor_element_type == "mul_t":
                datatype = "int32"
            # Enums are int32_t
            elif argument.tensor_element_type == "resize_mode_t":
                datatype = "int32"
            else:
                datatype = TOSA_TO_SPV_TYPE_DECL_NAME_MAP.get(
                    argument.tensor_element_type
                )
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
        elif argument.shape == "[TABLE_SIZE]":
            numValues = "4"
            values = "1, 1, 1, 1"
        else:
            numValues = "1,1,1,1"
            values = "1,1,1,1"
        return numValues, values

    def generate_tensor_arguments(self, op, argument):
        numValues, values = self.get_num_values_template(argument)
        datatype = self.get_arg_data_type(op, argument)

        name_line = 'std::string {}Name = "{}";\n'.format(argument.name, argument.name)
        shape_line = "std::vector<{}> {}Shape = {{{}}};\n".format(
            "int32_t", argument.name, values
        )
        tensor_line = "auto* {}Tensor = new TosaSerializationTensor({}Name, {}Shape, {}, {{}});\n".format(
            argument.name, argument.name, argument.name, self.get_dtype(datatype)
        )
        return [name_line, shape_line, tensor_line]

    def print_tensor_arguments(self, lines, file):
        if len(lines) == 0:
            print("ParserTests Tensor Arguments Generation Failed!")
            return
        for col_idx in range(3):
            for line_idx in range(len(lines)):
                file.write(lines[line_idx][col_idx])
            file.append("\n")

    # main generator for unit tests
    def generate_unit_tests(self):
        # check unit test path
        if not os.path.exists(self.layer_test_path):
            os.mkdir(self.layer_test_path)

        layer_counter = 0
        skip_layer_counter = 0

        # generate layer classes
        names, ops = self.get_operators()
        for idx in range(len(names)):
            if not ops[idx].name in self.operator_exception:
                if not self.force_generate:
                    if not self.check_layer_test(names[idx]):
                        self.generate_layer_test(ops[idx], names[idx])
                    else:
                        print(
                            "[Code Generator] Found existing {}.".format(
                                self.get_layer_test_path(names[idx])
                            )
                        )
                        skip_layer_counter += 1
                else:
                    if not names[idx] in self.operator_exception:
                        self.generate_layer_test(ops[idx], names[idx])
                    else:
                        print(
                            "[Code Generator] Skipping {}".format(
                                self.get_layer_test_path(names[idx])
                            )
                        )
                        skip_layer_counter += 1
                layer_counter += 1
        print(
            "[Code Generator] Generated unit tests for {} layers, skipped {} layers".format(
                layer_counter, skip_layer_counter
            )
        )

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

            # find a data type valid in type support for the operator tensor element type
            datatype = TOSA_TO_SPV_TYPE_DECL_NAME_MAP.get(
                op.typesupports[0].tymap.get(argument.tensor_element_type)
            )
            # if not it typesupport table revert to the tensor_element_type
            if datatype == None:
                # hack for inconsistency in avg_pool2d fixed in tosa 1.0
                if argument.tensor_element_type == "acc_size_t":
                    datatype = "int32"
                    # index_t not mentioned in support tables instead it
                    # follows integer value of 'levels' defind in spec
                elif argument.tensor_element_type == "index_t":
                    datatype = "int32"
                elif argument.tensor_element_type == "shape_t":
                    datatype = "int32"
                # for rescale if (scale32) mul_t=i32_t else mul_t=i16_t
                elif argument.tensor_element_type == "mul_t":
                    datatype = "int32"
                # Enums are int32_t
                elif argument.tensor_element_type == "resize_mode_t":
                    datatype = "int32"
                else:
                    datatype = TOSA_TO_SPV_TYPE_DECL_NAME_MAP.get(
                        argument.tensor_element_type
                    )

            inputArg = False
            first_line = ""
            if (
                any(category.name == "input" for category in argument.categories)
                and not any(
                    category.name == "attribute" for category in argument.categories
                )
                and argument.shape != "[rank(shape)]"
                and argument.shape != "-"
                and argument.shape != "[2]"
                and argument.shape != "[4]"
                and argument.shape != "[NC]"
                and argument.shape != "[TABLE_SIZE]"
                and argument.shape != "[rank(shape1)]"
            ):
                first_line = "auto {} = graph.AddInput(Tensor(".format(argument.name)
                inputArg = True
            elif any(category.name == "output" for category in argument.categories):
                first_line = "auto {} = Tensor(".format(argument.name)

            attributeArg = False
            if any(
                category.name == "attribute" for category in argument.categories
            ) and tosa_spirv.TOSAType.type_category_for_arg(argument):
                attributeArg = True
            if argument.name == "padding":
                attributeArg = True

            # work around resize incorrectly marking scale offset and border as inputs instead of attributes
            elif self.is_resize_attribute(op, argument):
                attributeArg = True
            # work around resize incorrectly marked as input
            elif (
                self.is_reshape_attribute(op, argument)
                or self.is_tile_attribute(op, argument)
                or self.is_transpose_attribute(op, argument)
            ):
                attributeArg = True

            if attributeArg:
                if (
                    tosa_spirv.TOSAType.type_category_for_arg(argument)
                    != tosa_spirv.TOSAType.RANKED_TENSOR
                    and argument.name != "padding"
                ):
                    first_line = "auto {} = Attribute(".format(argument.name)
                    first_line += "{" + values + "}, "
                    first_line += "DataType::{}_t);".format(datatype)
                    file.write(first_line)
                else:
                    file.write(
                        "auto {}Tensor = Tensor(DataType::{}_t, ".format(
                            argument.name, datatype
                        )
                    )
                    file.append("{" + values + "});\n")
                    file.write(
                        "const auto {}Constant = graph.AddGraphConstant({}Tensor);\n".format(
                            argument.name, argument.name, argument.name
                        )
                    )
                    file.write(
                        "auto {} = Attribute({}Constant);\n".format(
                            argument.name, argument.name
                        )
                    )

            else:
                file.write(first_line + "DataType::{}_t, ".format(datatype))
                file.append("std::vector<unsigned int>{ ")
                file.append(str(numValues))
                file.append(" })")
                if inputArg:
                    file.append(", 0)")
                file.append(";\n")

            if any(category.name == "input" for category in argument.categories) or any(
                category.name == "output" for category in argument.categories
            ):
                if (
                    self.is_reshape_attribute(op, argument)
                    or self.is_tile_attribute(op, argument)
                    or self.is_transpose_attribute(op, argument)
                ):
                    continue

            file.append("\n")

        for argument in op.arguments:
            if "input" in argument.name and argument.type == "tensor_list_t":
                file.write("std::vector<ResId> inputs;\n")
                file.write("inputs.push_back(input1);\n\n")

    def get_layer_variable_name(self, name):
        return name[0].lower() + name[1:] + "Layer"

    def get_effcee_string(self, op, is_graph=False):

        categoryEffceeCheckDict = {
            tosa_spirv.TOSAType.RANKED_TENSOR: "testutils::Check{INPUTOUTPUT}Tensor({{{VALUES}}}, "
            'DataType::{DATATYPE}, "{OP_NAME}", outputStr);',
            tosa_spirv.TOSAType.SHAPED_TENSOR: "testutils::Check{INPUTOUTPUT}Tensor({{{VALUES}}}, "
            'DataType::{DATATYPE}, "{OP_NAME}", outputStr);',
            tosa_spirv.TOSAType.SCALAR: "testutils::Check{BOOL}Constant("
            'DataType::{DATATYPE}, "{OP_NAME}", outputStr, '
            "{VALUE}, {ARG_INDEX});",
            tosa_spirv.TOSAType.ARRAY: "testutils::CheckConstCompositeTensor({{{VALUES}}}, "
            '"{OP_NAME}", outputStr, {ARG_INDEX});',
            tosa_spirv.TOSAType.TENSOR_LIST: "UNKNOWNTENSOR_LIST",
            tosa_spirv.TOSAType.TENSOR_LIST_UNIFORM_ETYPE: "testutils::Check{INPUTOUTPUT}Tensor({{{VALUES}}}, "
            'DataType::{DATATYPE}, "{OP_NAME}", outputStr);',
            tosa_spirv.TOSAType.ENUM: "testutils::Check{BOOL}Constant("
            'DataType::{DATATYPE}, "{OP_NAME}", outputStr, '
            "{VALUE}, {ARG_INDEX});",
            tosa_spirv.TOSAType.GRAPH: "UNKNOWNGRAPH",
            tosa_spirv.TOSAType.UNKNOWN: "UNKNOWN",
        }

        idx = 0
        inputIdx = 0
        effceestring = ""

        if op.name == "RESIZE":
            inputIdx += 1

        for argument in op.arguments:
            # Decide on number of values to give for shape
            if argument.shape == "[4]":
                values = "1, 1, 1, 1"
            elif argument.shape == "[3]":
                values = "1,1,1"
            elif argument.shape == "[6]":
                values = "1,1,1,1,1,1"
            elif argument.shape == "[2]":
                values = "1, 1"
            elif argument.shape == "-":
                values = "1"
            elif argument.shape == "[rank(shape)]":
                values = "1, 1, 1, 1"
            # PAD attribute: padding
            elif argument.shape == "[rank(shape1),2]" and not is_graph:
                values = "4"
            # TABLE attribute: table
            elif argument.shape == "[TABLE_SIZE]":
                values = "4"
            else:
                values = "1, 1, 1, 1"

            effcee_check_str = categoryEffceeCheckDict[
                tosa_spirv.TOSAType.type_category_for_arg(argument)
            ]
            isInput = self.has_argument_category_name(argument, "input")

            datatype = self.get_arg_type(op, argument)

            if is_graph:
                if op.name == "AVG_POOL2D" and (
                    argument.name == "input_zp" or argument.name == "output_zp"
                ):
                    datatype = "int8"
                elif op.name == "PAD" and argument.name == "padding":
                    datatype = "bool"

            # hack to account rescale having graph constants
            if self.is_rescale_graph_constant(op, argument):

                effcee_check_str = "\n"
                effceestring += effcee_check_str
                idx += 1
            # hack to account reshape having Arrays
            elif (
                self.is_reshape_attribute(op, argument)
                or self.is_tile_attribute(op, argument)
                or self.is_transpose_attribute(op, argument)
                or self.is_slice_attribute(op, argument)
            ):
                if self.has_argument_category_name(argument, "input"):
                    index = 1
                else:
                    index = 0
                effcee_check_str = (
                    '    testutils::CheckConstCompositeTensor({{{VALUES}}}, "{OP_NAME}", outputStr, {ARG_INDEX});\n')

                effceestring += effcee_check_str.format(
                    VALUES=values, OP_NAME=op.name, ARG_INDEX=index
                )
                idx += 1
            # Handle scalars and enums
            elif (
                tosa_spirv.TOSAType.type_category_for_arg(argument)
                == tosa_spirv.TOSAType.SCALAR
                or tosa_spirv.TOSAType.type_category_for_arg(argument)
                == tosa_spirv.TOSAType.ENUM
            ):
                if (
                    self.has_argument_tensor_element_type_name(argument, "bool_t")
                    or datatype == "bool"
                ):
                    datatype = "bool"
                    effceestring += (
                        "    "
                        + effcee_check_str.format(
                            BOOL="Bool",
                            OP_NAME=op.name,
                            VALUE="true",
                            ARG_INDEX=idx,
                            DATATYPE="bool_t",
                        )
                        + "\n"
                    )
                else:
                    datatype_str = str(datatype) + "_t"
                    effceestring += (
                        "    "
                        + effcee_check_str.format(
                            BOOL="",
                            OP_NAME=op.name,
                            VALUE="1",
                            ARG_INDEX=idx,
                            DATATYPE=datatype_str,
                        )
                        + "\n"
                    )
                idx += 1
            # Handle Inputs
            # hack to account for mode attribute before inputs for resize
            elif isInput or self.is_resize_attribute(op, argument):
                # resize marked as multiple inputs but actually they are attributes
                functionName = "Input"
                if inputIdx > 0:
                    effceestring += "    "

                datatype_str = str(datatype) + "_t"
                if (op.name == "TABLE" and inputIdx == 1) or (
                    op.name == "PAD" and inputIdx > 0
                ):
                    continue
                effceestring += (
                    effcee_check_str.format(
                        OP_NAME=op.name,
                        INPUTOUTPUT=functionName,
                        VALUES=values,
                        ARG_INDEX=inputIdx,
                        DATATYPE=datatype_str,
                    )
                    + "\n"
                )
                inputIdx += 1
            # Handle outputs and non scalar/bool
            else:
                functionName = "Output"
                datatype_str = str(datatype) + "_t"
                effceestring += (
                    "    "
                    + effcee_check_str.format(
                        OP_NAME=op.name,
                        INPUTOUTPUT=functionName,
                        VALUES=values,
                        ARG_INDEX=idx,
                        DATATYPE=datatype_str,
                    )
                    + "\n"
                )
                # hack for rescale unusually having output come first in arguments
                if (
                    "output" in argument.name
                    and op.name == "RESCALE"
                    or op.name == "FFT2D"
                    or op.name == "RFFT2D"
                ):
                    continue
                else:
                    idx += 1

        return effceestring

    def generate_layer_test(self, op, name):

        print(
            "[Test Generator] Generating {} ...".format(self.get_layer_test_path(name))
        )

        file = FileWriter(self.get_layer_test_path(name))

        file.write(self.preamble_free_edit())

        # include files
        file.write("#include <tosa2spirv.hpp>\n")
        file.write("#include <Graph.hpp>\n")

        file.write("#include <AssemblyUtils.hpp>\n\n")
        file.write("#include <OpTestUtils.hpp>\n")

        file.write("#include <gtest/gtest.h>\n")
        file.write("#include <iostream>\n\n")

        # namespaces
        file.write("using namespace tosa2spirv::tosa;\n")

        # test case
        file.write("// Automatically generated by code_generator.py\n")
        file.write("TEST(TOSA2SPIRV_LAYERS, {})\n".format(name))
        file.write("{\n")

        # test body
        file.start_block()

        file.write("auto module = CreateModule(tosa2spirv::TOSAVersion{});\n")
        file.write("auto graph = Graph(module);\n")
        file.append("\n")

        # argument assignment
        self.generate_test_arguments(op, name, file)

        file.append("\n")

        first_line = "const auto res = graph.Add{}Operator(".format(
            name, self.get_layer_variable_name(name)
        )
        num_indentation = len(first_line)

        file.write(first_line)

        for argument in op.arguments:
            if argument is op.arguments[0]:
                if argument.type == "tensor_list_t":
                    file.append("{},\n".format("inputs"))
                else:
                    file.append("{},\n".format(argument.name))
            elif argument is op.arguments[len(op.arguments) - 1]:
                file.write(" " * num_indentation + "{});\n".format(argument.name))
            else:
                file.write(" " * num_indentation + "{},\n".format(argument.name))

        if (op.name == "FFT2D") or (op.name == "RFFT2D"):
            file.write("graph.AddOutput(res[0], 0);\n")
            file.write("graph.AddOutput(res[1], 0);\n")
        else:
            file.write("graph.AddOutput(res, 0);\n")
        file.write("graph.FinalizeGraph();\n")
        file.append("\n")

        file.write("auto binary = tosa2spirv::WriteToBinary(module);\n")
        file.write(
            "std::string outputStr(testutils::DisassembleSPIRV(binary, true));\n\n"
        )

        effceestring = self.get_effcee_string(op)

        file.write(effceestring + "\n")
        file.write("// Write binary a second time to ensure IDs remain consistent.\n")
        file.write("binary = tosa2spirv::WriteToBinary(module);\n")
        file.write("outputStr = testutils::DisassembleSPIRV(binary, true);\n\n")
        file.write(effceestring)

        file.end_block()
        file.write("}")

        file.close()

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

    def check_fake_input(self, op, argument):
        fake_input_dict = {
            "RESCALE": {"multiplier", "shift"},
            "RESIZE": {"scale", "offset", "border"},
            "PAD": {"padding"},
            "TABLE": {"table"},
        }
        if (
            op.name in fake_input_dict.keys()
            and argument.name in fake_input_dict[op.name]
        ):
            return True
        return False

    # Check input which is incorrectly marked as attribute
    def check_fake_attribute(self, op, argument):
        fake_attr_dict = {}
        if (
            op.name in fake_attr_dict.keys()
            and argument.name in fake_attr_dict[op.name]
        ):
            return True
        return False

    def generate_parser_test_case(self, op, file):
        op_pascal_case_name = self.operator_name_to_pascal_case(op.name)

        file.write("TEST(TOSA2SPIRV_PARSER, {})\n".format(op_pascal_case_name))
        file.write("{\n")
        file.start_block()

        op = self.fix_op(op)

        input_args = []
        output_args = []
        attribute_args = []

        for argument in op.arguments:
            # check the fake input which should be attributes
            if self.check_fake_input(op, argument):
                attribute_args.append(argument)
            elif self.check_fake_attribute(op, argument):
                input_args.append(argument)
            elif (
                any(category.name == "input" for category in argument.categories)
                and argument.shape != "[rank(shape)]"
                and argument.shape != "-"
                and argument.shape != "[2]"
                and argument.shape != "[4]"
                and argument.shape != "[NC]"
                and argument.shape != "[TABLE_SIZE]"
                and argument.shape != "[rank(shape1)]"
            ):
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
            # attr_name will return attribute name for special cases, such as pad_const_int and pad_const_fp for PAD
            is_attr, attr_name = self.generate_attribute_arguments(op, argument, file)
            if is_attr:
                if len(attr_name) > 0:
                    attr_list.extend(attr_name)
                else:
                    attr_list.append(argument.name)

        if len(attr_list) > 0:
            file.write("// Create Attribute\n")
            file.write("Tosa{} attribute(".format(self.dictOpAttributes[op.name]))
            # fix Clamp min_val, max_val
            if op.name == "CLAMP":
                attr_list[1], attr_list[2] = attr_list[2], attr_list[1]
            # fix POOL attribute sequence
            elif op.name == "AVG_POOL2D":
                attr_list[0], attr_list[1], attr_list[2] = (
                    attr_list[2],
                    attr_list[0],
                    attr_list[1],
                )
                attr_list[3], attr_list[4] = attr_list[4], attr_list[5]
                attr_list[5] = "DType::DType_FP16"
            elif op.name == "MAX_POOL2D":
                attr_list[0], attr_list[1], attr_list[2] = (
                    attr_list[2],
                    attr_list[0],
                    attr_list[1],
                )
                attr_list.extend(["0", "0", "DType::DType_INT32"])
            # print attribute names
            for attr in attr_list:
                if attr == attr_list[-1]:
                    file.append("{});\n".format(attr))
                else:
                    file.append("{}, ".format(attr))

            file.append("\n")

        # Create tensors
        file.write("// Create Tensors\n")
        tensor_lines = []
        # Input tensors
        for argument in input_args:
            tensor_line = self.generate_tensor_arguments(op, argument)
            tensor_lines.append(tensor_line)
        # Output tensors
        for argument in output_args:
            tensor_line = self.generate_tensor_arguments(op, argument)
            tensor_lines.append(tensor_line)
        # Write lines to the file
        self.print_tensor_arguments(tensor_lines, file)

        # Get input tensors and input/output names
        input_output_args = input_args + output_args
        tensors = ["{}Tensor".format(arg.name) for arg in input_output_args]
        input_names = ["{}Name".format(arg.name) for arg in input_args]
        output_names = ["{}Name".format(arg.name) for arg in output_args]

        # Create Operator
        file.write("// Create Operator\n")
        first_line = "auto op = new tosa::TosaSerializationOperator("
        file.write(first_line + "Op::Op_{},\n".format(op.name))
        if len(attr_list) == 0:
            file.write(" " * len(first_line) + "Attribute::Attribute_NONE,\n")
            file.write(" " * len(first_line) + "nullptr,\n")
        else:
            file.write(
                " " * len(first_line)
                + "Attribute::Attribute_{},\n".format(self.dictOpAttributes[op.name])
            )
            file.write(" " * len(first_line) + "&attribute,\n")
        file.write(" " * len(first_line) + "{ ")
        for input_name in input_names:
            if input_name == input_names[-1]:
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

        # Create a tosa single-op basic block
        file.write("// Create a tosa single-op basic block\n")
        file.write(
            "// The raw pointers of operators and tensors will be deleted by the destructor of the block\n"
        )

        first_line = "TosaSerializationBasicBlock block("
        file.write(first_line + '"{}",\n'.format(op.name.lower()))
        file.write(" " * len(first_line) + '"main",\n')
        file.write(" " * len(first_line) + "{ op },\n")
        file.write(" " * len(first_line) + "{ ")
        for tensor in tensors:
            if tensor == tensors[-1]:
                file.append("{} }},\n".format(tensor))
            else:
                file.append("{}, ".format(tensor))
        file.write(" " * len(first_line) + "{ ")
        for input_name in input_names:
            if input_name == input_names[-1]:
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

        # Generate and Disassemble SPIRV
        file.write('auto binarySpirv = parser.GenerateSPIRV("main");\n')
        file.write(
            "const std::string outputStr(testutils::DisassembleSPIRV(binarySpirv, true));\n"
        )
        file.append("\n")

        # Generate effcee check string
        effceestring = self.get_effcee_string(op, is_graph=True)
        file.write(effceestring)

        file.end_block()
        file.write("}\n")
        file.write("\n")
