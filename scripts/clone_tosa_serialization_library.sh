#!/usr/bin/env bash

#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

TOSA_TOOLS_DIR=$EXTERNAL_DIR/tosa-tools

rm -rf $TOSA_TOOLS_DIR

TOSA_SERIALIZATION_LIB_REVISION="988e56145ab66d802ce4a32fa31c0c76e5c801f9" # Released with TOSA Software v2025.11.0

echo "+++ Cloning TOSA Serialization Library"
git clone "https://gitlab.arm.com/tosa/tosa-tools.git" $TOSA_TOOLS_DIR

pushd $TOSA_TOOLS_DIR/serialization > /dev/null
  git checkout $TOSA_SERIALIZATION_LIB_REVISION

  # Initialise Flatbuffers dependency, which is currently v2.0.6
  git submodule update --init --recursive
popd > /dev/null
