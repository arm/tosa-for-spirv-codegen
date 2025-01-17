#!/usr/bin/env bash

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

BUILD_TYPE=$1
CMAKE_PATH=$2

VGF_ENCODER_DIR=$EXTERNAL_DIR/vgf_encoder

pushd $VGF_ENCODER_DIR > /dev/null
  rm -rf build
  mkdir -p build

  pushd build > /dev/null
    CMD="$CMAKE_PATH -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
                     -DFLATC_PATH=$EXTERNAL_DIR/serialization_lib/build/third_party/flatbuffers/flatc \
                     -DARGPARSE_PATH=$EXTERNAL_DIR/argparse \
                     -DJSON_PATH=$EXTERNAL_DIR/json \
                     -DFLATBUFFERS_PATH=$EXTERNAL_DIR/serialization_lib/third_party/flatbuffers \
                     -DML_SDK_VGF_LIB_BUILD_TESTS=0 \
                     -DML_SDK_VGF_LIB_BUILD_TOOLS=1 \
                     -S .. -B ."
    CXXFLAGS="-fPIC" $CMD
    $CMAKE_PATH --build . -j$(nproc)
  popd > /dev/null
popd > /dev/null