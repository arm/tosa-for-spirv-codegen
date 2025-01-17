#!/usr/bin/env bash

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

BUILD_TYPE=$1
CMAKE_PATH=$2

EFFCEE_DIR=${EXTERNAL_DIR}/effcee

pushd $EFFCEE_DIR > /dev/null
    $CMAKE_PATH -S . -B build \
                -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
                -DCMAKE_INSTALL_PREFIX="$(pwd)/install" \
                -DEFFCEE_GOOGLETEST_DIR=${EXTERNAL_DIR}/googletest
    $CMAKE_PATH --build build -j$(nproc)
popd > /dev/null