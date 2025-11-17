#!/usr/bin/env bash
#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

usage() {
  echo "Usage: $CMD [-t (type) <Debug or Release>
                     -c (cmake path) <Path to cmake executable>
                     -a (build for Android)"
  exit 1
}

# Set default options
BUILD_TYPE="Release"
CMAKE_PATH="cmake" # use system cmake by default

while getopts "ht:c:a" opt; do
  case "$opt" in
    h|\?) usage;;
    t) BUILD_TYPE="$OPTARG";;
    c) CMAKE_PATH="$OPTARG";;
    a) IS_ANDROID=true ;;
  esac
done
shift $((OPTIND - 1))

# If building for android
if [ "$IS_ANDROID" = true ]; then
  if [ -z "$ANDROID_ABI" ]; then
    echo "ANDROID_ABI not set, defaulting to arm64-v8a."
    export ANDROID_ABI=arm64-v8a
  else
    echo "Using ANDROID_ABI=$ANDROID_ABI"
  fi

  if [ -z "$NDK_DIR" ]; then
    echo "NDK_DIR is not set. Please set it for Android builds."
    exit 1
  elif [ ! -d "$NDK_DIR" ]; then
    echo "NDK_DIR must point to a valid directory. NDK_DIR currently points to $NDK_DIR"
    exit 1
  fi

  if [ -z "$ANDROID_API" ]; then
    export ANDROID_API=21
    echo "ANDROID_API is was not set, defaulting to version 21."
  fi
  TARGET="ANDROID"
else
  TARGET="NATIVE"
fi


function BuildSpirvTools {
  scripts/build_spirv_tools.sh $BUILD_TYPE $CMAKE_PATH $TARGET || exit 1
}

function BuildTOSASerializationLibrary {
  scripts/build_tosa_serialization_library.sh $CMAKE_PATH $TARGET || exit 1
}

function BuildVgfEncoder {
  scripts/build_vgf_encoder.sh $BUILD_TYPE $CMAKE_PATH $TARGET || exit 1
}

# Function to clean previously built third party libraries to avoid cross compiling issues
function CleanPreviouslyBuiltDependencies {
    export EXTERNAL_DIR="$(pwd)/external"
    rm -rf ${EXTERNAL_DIR}/serialization_lib/build
    rm -rf ${EXTERNAL_DIR}/vgf_encoder/build
}

echo "*** Building tfsc dependencies."

CleanPreviouslyBuiltDependencies
BuildSpirvTools
BuildTOSASerializationLibrary
BuildVgfEncoder

echo "*** All dependencies built successfully."
