#!/usr/bin/env bash

#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

TOSA_SERIALIZATION_LIB_DIR=$EXTERNAL_DIR/serialization_lib

rm -rf $TOSA_SERIALIZATION_LIB_DIR

TOSA_SERIALIZATION_LIB_REVISION="359145e768e565d7666aa6ebd0abb92784f398cb" # Released with TOSA Software v2025.07.0

echo "+++ Cloning TOSA Serialization Library"
git clone "https://gitlab.arm.com/tosa/tosa-serialization.git" $TOSA_SERIALIZATION_LIB_DIR

pushd $TOSA_SERIALIZATION_LIB_DIR > /dev/null
  git checkout $TOSA_SERIALIZATION_LIB_REVISION

  # Initialise Flatbuffers dependency, which is currently v2.0.6
  git submodule update --init --recursive
popd > /dev/null
