#!/usr/bin/env python3

#
# Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

import sys
from unicodedata import category

# Add tosa.py directory to paths python searches for modules
sys.path.append("./external/tosa_specification/tools")
# Add tosa_spirv.py directory to paths python searches for modules
sys.path.append("./external/ngp-runtime/scripts")

import tosa
from tosa import TOSAOperatorArgumentCategory


class Operators:
    def __init__(self):
        self.spec = tosa.TOSASpec("./external/tosa_specification/tosa.xml")
        self.tosa_version = "{}.{}.{}".format(
            self.spec.version_major, self.spec.version_minor, self.spec.version_patch
        )

        self.operator_list = []
        self.operator_name_list = []
        self.operator_exception = [
            "CUSTOM",
            "CONST",
            "IDENTITY",
            "DIM",
            "VARIABLE",
            "VARIABLE_WRITE",
            "VARIABLE_READ",
        ]
        self.unsupported_groups = ["custom", "control-flow", "variable"]

        for group in self.spec.operatorgroups:
            if group.name in self.unsupported_groups:
                continue
            for op in group.operators:
                if op.name in self.operator_exception:
                    continue
                op = apply_operator_fixes(op)
                self.operator_list.append(op)
                self.operator_name_list.append(operator_name_to_pascal_case(op.name))


def get_argument(op, name):
    for arg in op.arguments:
        if arg.name == name:
            return arg
    raise Exception("Argument: " + name + " not found in operator: " + op.name)


def apply_operator_fixes(op):

    if op.name == "MUL":
        shape = get_argument(op, "shift")
        shape.categories[0].name = "attribute"

    if op.name == 'RESCALE':
        multiplier = get_argument(op, 'multiplier')
        multiplier.categories[0].name = 'input'

        shift = get_argument(op, 'shift')
        shift.categories[0].name = 'input'

    elif op.name == "RESHAPE":
        shape = get_argument(op, "shape")
        shape.categories[0].name = "attribute"

    elif op.name == "RESIZE":
        scale = get_argument(op, "scale")
        scale.categories[0].name = "attribute"

        offset = get_argument(op, "offset")
        offset.categories[0].name = "attribute"

        border = get_argument(op, "border")
        border.categories[0].name = "attribute"

    elif op.name == "RESHAPE":
        shape = get_argument(op, "shape")
        shape.categories[0].name = "attribute"

    elif op.name == "TILE":
        arg = get_argument(op, "multiples")
        arg.categories[0].name = "attribute"

    elif op.name == "PAD":
        arg = get_argument(op, "padding")
        arg.categories.append(TOSAOperatorArgumentCategory("attribute"))
        for cat in get_argument(op, "padding").categories:
            print(cat.name)

    return op


def name_to_pascal_case(name, separator):
    return "".join(w.capitalize() for w in name.split(separator))


def operator_name_to_pascal_case(name):
    return name_to_pascal_case(name, "_")


def operator_name_to_camel_case(name):
    camel_case = "".join(w.capitalize() for w in name.split("_"))
    camel_case = "".join([camel_case[:1].lower(), (camel_case[1:])])
    return camel_case


def has_argument_category_name(arg, string):
    has_name = False
    for category in arg.categories:
        if category.name == string:
            has_name = True
    return has_name

