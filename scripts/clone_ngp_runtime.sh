#!/usr/bin/env bash

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

NGP_RUNTIME_DIR=$EXTERNAL_DIR/ngp-runtime

# Pinned SHA
NGP_RUNTIME_SHA="8797d08cc1a1db76a0d8ef35fc5a7c78dac828f1" # Pinned to known good SHA to prevent breaks

rm -rf $NGP_RUNTIME_DIR
mkdir $EXTERNAL_DIR/ngp-runtime

echo "+++ Cloning ngp-runtime"
git clone ssh://$USERNAME@eu-gerrit-1.euhpc.arm.com:29418/GPU-SW/compute/ngp-runtime.git $NGP_RUNTIME_DIR

pushd $NGP_RUNTIME_DIR > /dev/null
  git checkout $NGP_RUNTIME_SHA
  git submodule update --init
popd > /dev/null
