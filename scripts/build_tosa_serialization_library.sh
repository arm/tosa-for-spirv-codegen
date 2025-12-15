#!/usr/bin/env bash
#
# Copyright © 2023, 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

CMAKE_PATH=$1
TARGET=$2
TOSA_SERIALIZATION_LIB_DIR=$EXTERNAL_DIR/tosa-tools/serialization
BUILD_DIR=$TOSA_SERIALIZATION_LIB_DIR/build

pushd "$TOSA_SERIALIZATION_LIB_DIR" > /dev/null

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR" > /dev/null

CMARGS="-DBUILD_TESTS=NO"

if [ "$TARGET" = "ANDROID" ]; then
  CMARGS="$CMARGS \
          -DCMAKE_SYSTEM_NAME=Android \
          -DCMAKE_TOOLCHAIN_FILE=$NDK_DIR/build/cmake/android.toolchain.cmake \
          -DCMAKE_ANDROID_NDK=$NDK_DIR \
          -DANDROID_ABI=$ANDROID_ABI \
          -DANDROID_PLATFORM=android-$ANDROID_API"
fi

CXXFLAGS="-fPIC" "$CMAKE_PATH" $CMARGS -S .. -B .
AssertZeroExitCode "CMake for TOSA serialization library ($TARGET) failed"

$CMAKE_PATH --build . -j$(nproc)
AssertZeroExitCode "Build of TOSA serialization library ($TARGET) failed"

popd > /dev/null
