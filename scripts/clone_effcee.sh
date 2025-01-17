#!/usr/bin/env bash

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

if [ ! -d ${EXTERNAL_DIR}/effcee ]; then
    # SHA for effcee
    EFFCEE_REVISION="aea1f4d62ca9ee2f44b5393e98e175e200a22e8e"

    # Tags for effcee's dependencies (Google Test already in ${EXTERNAL_DIR})
    RE2_TAG="2024-05-01"
    ABSEIL_CPP_TAG="20240116.2"

    mkdir -p ${EXTERNAL_DIR}/effcee && cd $_

    echo "+++ Cloning effcee"
    git init . -q
    git fetch https://github.com/google/effcee.git $EFFCEE_REVISION --depth=1 -q
    git -c advice.detachedHead=false checkout FETCH_HEAD -q

    cd third_party

    echo "+++ Cloning effcee dependencies"
    git -c advice.detachedHead=false clone -q --depth=1 --branch $RE2_TAG         https://github.com/google/re2.git &
    git -c advice.detachedHead=false clone -q --depth=1 --branch $ABSEIL_CPP_TAG  https://github.com/abseil/abseil-cpp.git &

    wait
fi
