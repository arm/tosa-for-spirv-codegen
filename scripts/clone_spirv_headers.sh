#!/usr/bin/env bash

#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

SPIRV_HEADERS_DIR=$EXTERNAL_DIR/SPIRV-Headers

rm -rf $SPIRV_HEADERS_DIR

SPIRV_HEADERS_REVISION="01e0577914a75a2569c846778c2f93aa8e6feddd" # Pinned SHA of vulkan-sdk-1.4.328.0

echo "+++ Cloning SPIRV-Headers"
mkdir -p SPIRV_HEADERS_DIR
git clone https://github.com/KhronosGroup/SPIRV-Headers.git $SPIRV_HEADERS_DIR

pushd $SPIRV_HEADERS_DIR > /dev/null
  git checkout $SPIRV_HEADERS_REVISION
popd > /dev/null
