#!/usr/bin/env bash

#
# Copyright © 2023,2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

BUILD_TYPE=$1
CMAKE_PATH=$2
TARGET=$3

SPIRV_TOOLS_DIR=$EXTERNAL_DIR/SPIRV-Tools

pushd $SPIRV_TOOLS_DIR > /dev/null
  rm -rf build
  mkdir -p build

  pushd build > /dev/null
    CMARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DCMAKE_INSTALL_PREFIX=$(pwd)/install \
            -DSPIRV-Headers_SOURCE_DIR=$EXTERNAL_DIR/SPIRV-Headers"

    if [ "$TARGET" = "ANDROID" ]; then
        CMARGS="$CMARGS \
            -DCMAKE_SYSTEM_NAME=Android \
            -DCMAKE_TOOLCHAIN_FILE=$NDK_DIR/build/cmake/android.toolchain.cmake \
            -DCMAKE_ANDROID_NDK=$NDK_DIR \
            -DANDROID_ABI=$ANDROID_ABI \
            -DANDROID_PLATFORM=android-$ANDROID_API"
    fi

    $CMAKE_PATH $CMARGS ..
    AssertZeroExitCode "Cmake for SPIRV-Tools failed"
    $CMAKE_PATH --build . -j$(nproc)
    AssertZeroExitCode "Build of SPIRV-Tools failed"
  popd > /dev/null
popd > /dev/null