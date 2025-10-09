#!/usr/bin/env bash

#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

SPIRV_TOOLS_DIR=$EXTERNAL_DIR/SPIRV-Tools

SPIRV_TOOLS_REVISION="7f2d9ee926f98fc77a3ed1e1e0f113b8c9c49458" # Pinned SHA of v2025.4

rm -rf $SPIRV_TOOLS_DIR

echo "+++ Cloning SPIRV-Tools"
git clone https://github.com/KhronosGroup/SPIRV-Tools.git $SPIRV_TOOLS_DIR

pushd $SPIRV_TOOLS_DIR > /dev/null
  git checkout $SPIRV_TOOLS_REVISION
popd > /dev/null
