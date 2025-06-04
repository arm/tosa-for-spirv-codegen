#!/usr/bin/env bash

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

NGP_RUNTIME_DIR=$EXTERNAL_DIR/ngp-runtime

# Pinned SHA
NGP_RUNTIME_SHA="9a944168717744958219da4a7f35904d5c3df507" # Tosa 1.0 March ops delivery

rm -rf $NGP_RUNTIME_DIR
mkdir $EXTERNAL_DIR/ngp-runtime

echo "+++ Cloning ngp-runtime"
git clone ssh://$USERNAME@eu-gerrit-1.euhpc.arm.com:29418/GPU-SW/compute/ngp-runtime.git $NGP_RUNTIME_DIR

pushd $NGP_RUNTIME_DIR > /dev/null
  git checkout $NGP_RUNTIME_SHA
  git submodule update --init
popd > /dev/null
