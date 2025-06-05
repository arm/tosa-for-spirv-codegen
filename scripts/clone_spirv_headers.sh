#!/usr/bin/env bash

#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

SPIRV_HEADERS_DIR=$EXTERNAL_DIR/SPIRV-Headers

rm -rf $SPIRV_HEADERS_DIR

# Pinned SHA for main-with-ARM-patches branch
SPIRV_HEADERS_REVISION="16eb1fe71edaf9f2b71d08ce3440b9b40fa42c1a" # Add SPV_EXT_float8

echo "+++ Cloning SPIRV-Headers"
mkdir -p SPIRV_HEADERS_DIR
git clone ssh://$USERNAME@eu-gerrit-1.euhpc.arm.com:29418/GPU-SW/khronos/SPIRV-Headers.git $SPIRV_HEADERS_DIR

pushd $SPIRV_HEADERS_DIR > /dev/null
  git checkout $SPIRV_HEADERS_REVISION
popd > /dev/null
