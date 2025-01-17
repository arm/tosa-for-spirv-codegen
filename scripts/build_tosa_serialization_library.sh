#!/usr/bin/env bash

#
# Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

CMAKE_PATH=$1
TOSA_SERIALIZATION_LIB_DIR=$EXTERNAL_DIR/serialization_lib

pushd $TOSA_SERIALIZATION_LIB_DIR > /dev/null
  rm -rf build
  mkdir -p build

  pushd build > /dev/null
    CMD="$CMAKE_PATH -DBUILD_TESTS=NO \
                     -S .. -B ."
    CXXFLAGS="-fPIC" $CMD
    $CMAKE_PATH --build . -j$(nproc)
  popd > /dev/null
popd > /dev/null