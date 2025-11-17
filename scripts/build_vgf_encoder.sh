#!/usr/bin/env bash
#
# Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

BUILD_TYPE=$1
CMAKE_PATH=$2
TARGET=$3

VGF_ENCODER_DIR=$EXTERNAL_DIR/vgf_encoder
BUILD_DIR=$VGF_ENCODER_DIR/build

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR" > /dev/null

# Make sure we are getting an x86-64 build of flatc
FLATC_PATH="$EXTERNAL_DIR/flatbuffers/build/flatc"
FLATC_NATIVE_BUILD_DIR="$EXTERNAL_DIR/flatbuffers/build"

# Build native flatbuffers
if [ -n "$FLATC_PATH" ]; then
  mkdir -p "$FLATC_NATIVE_BUILD_DIR"
  pushd "$FLATC_NATIVE_BUILD_DIR" > /dev/null

  $CMAKE_PATH -DBUILD_TESTS=OFF .. || {
    echo "Failed to configure native flatbuffers build."
    exit 1
  }

  $CMAKE_PATH --build . -j$(nproc) || {
    echo "Failed to build native flatbuffers (flatc)."
    exit 1
  }
  popd
fi

CMARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DARGPARSE_PATH=$EXTERNAL_DIR/argparse \
        -DJSON_PATH=$EXTERNAL_DIR/json \
        -DFLATBUFFERS_PATH=$EXTERNAL_DIR/flatbuffers \
        -DML_SDK_VGF_LIB_BUILD_TESTS=0 \
        -DML_SDK_VGF_LIB_BUILD_TOOLS=1 \
        -DFLATC_PATH=$FLATC_PATH"

if [ "$TARGET" = "ANDROID" ]; then
    CMARGS="$CMARGS \
            -DCMAKE_SYSTEM_NAME=Android \
            -DCMAKE_TOOLCHAIN_FILE=$NDK_DIR/build/cmake/android.toolchain.cmake \
            -DCMAKE_ANDROID_NDK=$NDK_DIR \
            -DANDROID_ABI=$ANDROID_ABI \
            -DANDROID_PLATFORM=android-$ANDROID_API \
            -DFLATC_PATH=$FLATC_PATH"
fi

CXXFLAGS="-fPIC" $CMAKE_PATH $CMARGS -S .. -B .
AssertZeroExitCode "CMake for VGF encoder failed"

$CMAKE_PATH --build . -j$(nproc)
AssertZeroExitCode "Build of VGF encoder failed"

