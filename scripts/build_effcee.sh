#!/usr/bin/env bash

#
# Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

BUILD_TYPE=$1
CMAKE_PATH=$2
TARGET=$3

EFFCEE_DIR=${EXTERNAL_DIR}/effcee

# Clean builds before to avoid cross-compile issues
if [ "$TARGET" = "ANDROID" ]; then
  echo "Cleaning Effcee and Abseil outputs for Android build"
  rm -rf "${EFFCEE_DIR}/build"
  rm -rf "${EFFCEE_DIR}/third_party/abseil-cpp/out"
fi

pushd $EFFCEE_DIR > /dev/null
  rm -rf build
  mkdir -p build
  CMARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
          -DCMAKE_INSTALL_PREFIX=$(pwd)/install \
          -DEFFCEE_GOOGLETEST_DIR=${EXTERNAL_DIR}/googletest"

  if [ "$TARGET" = "ANDROID" ]; then
    CMARGS="$CMARGS \
      -DCMAKE_SYSTEM_NAME=Android \
      -DCMAKE_TOOLCHAIN_FILE=$NDK_DIR/build/cmake/android.toolchain.cmake \
      -DCMAKE_ANDROID_NDK=$NDK_DIR \
      -DANDROID_ABI=$ANDROID_ABI \
      -DANDROID_PLATFORM=android-$ANDROID_API"
  fi


  $CMAKE_PATH -S . -B build $CMARGS
  AssertZeroExitCode "Cmake for effcee failed"
  $CMAKE_PATH --build build -j$(nproc)
  AssertZeroExitCode "Build of effcee failed"
popd > /dev/null