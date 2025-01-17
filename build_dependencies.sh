#!/usr/bin/env bash

#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

usage() {
  echo "Usage: $CMD [-t (type) <Debug or Release>
                     -c (cmake path) <Path to cmake executable>]"
  exit 1
}

# Set default options
BUILD_TYPE="Release"
CMAKE_PATH="cmake" # use system cmake by default

while getopts "ht:c:" opt; do
  case "$opt" in
    h|\?) usage;;
    t) BUILD_TYPE="$OPTARG";;
    c) CMAKE_PATH="$OPTARG";;
  esac
done
shift $((OPTIND - 1))

function BuildSpirvTools {
  scripts/build_spirv_tools.sh $BUILD_TYPE $CMAKE_PATH || exit 1
}

function BuildTOSASerializationLibrary {
  scripts/build_tosa_serialization_library.sh $CMAKE_PATH || exit 1
}

function BuildVgfEncoder {
  scripts/build_vgf_encoder.sh $BUILD_TYPE $CMAKE_PATH || exit 1
}

function BuildEffcee {
    scripts/build_effcee.sh $BUILD_TYPE $CMAKE_PATH || exit 1
}

echo "*** Building tosa2spirv dependencies."

BuildSpirvTools
BuildTOSASerializationLibrary
BuildVgfEncoder
BuildEffcee

echo "*** All dependencies built successfully."