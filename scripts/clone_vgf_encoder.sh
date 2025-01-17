#!/usr/bin/env bash

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

VGF_ENCODER_DIR=$EXTERNAL_DIR/vgf_encoder

rm -rf $VGF_ENCODER_DIR

# Pinned SHA for branch
VGF_ENCODER_REVISION="aeccd4a5c8f23b28a7b330e320b4e5c0408a5fad"

echo "+++ Cloning VGF Encoder"
mkdir -p VGF_ENCODER_DIR
git clone ssh://$USERNAME@eu-gerrit-2.euhpc.arm.com:29418/ml/ngp/ml-sdk-vgf-lib $VGF_ENCODER_DIR

pushd $VGF_ENCODER_DIR > /dev/null
  git checkout $VGF_ENCODER_REVISION
popd > /dev/null
