#!/usr/bin/env bash

#
# Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

BUILD_TYPE=$1
CMAKE_PATH=$2

SPIRV_TOOLS_DIR=$EXTERNAL_DIR/SPIRV-Tools

pushd $SPIRV_TOOLS_DIR > /dev/null
  rm -rf build
  mkdir -p build

  pushd build > /dev/null
    $CMAKE_PATH -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
                -DCMAKE_INSTALL_PREFIX="$(pwd)/install" \
                -DSPIRV-Headers_SOURCE_DIR=$EXTERNAL_DIR/SPIRV-Headers ..
    $CMAKE_PATH --build . -j$(nproc)
  popd > /dev/null
popd > /dev/null