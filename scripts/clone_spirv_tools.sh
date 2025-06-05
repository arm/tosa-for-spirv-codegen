#!/usr/bin/env bash

#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

SPIRV_TOOLS_DIR=$EXTERNAL_DIR/SPIRV-Tools

# Pinned SHA for main-with-ARM-patches branch
SPIRV_TOOLS_REVISION="9012a8de4fdba0039ccedac1922c31f7ac83a2c0" # Fix asm/disasm support for the MLGraph DebugInfo instruction set

rm -rf $SPIRV_TOOLS_DIR

echo "+++ Cloning SPIRV-Tools"
git clone ssh://$USERNAME@eu-gerrit-1.euhpc.arm.com:29418/GPU-SW/khronos/SPIRV-Tools.git $SPIRV_TOOLS_DIR

pushd $SPIRV_TOOLS_DIR > /dev/null
  git checkout $SPIRV_TOOLS_REVISION
popd > /dev/null
