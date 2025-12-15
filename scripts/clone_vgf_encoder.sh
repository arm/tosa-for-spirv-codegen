#!/usr/bin/env bash

#
# Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

VGF_ENCODER_DIR=$EXTERNAL_DIR/vgf_encoder
FLATBUFFERS_DIR=$EXTERNAL_DIR/flatbuffers

rm -rf $VGF_ENCODER_DIR
rm -rf $FLATBUFFERS_DIR

# Pinned SHA for branch
VGF_ENCODER_REVISION="55d73eaa8c88f74cf5732a3a8902450f208006af"  # Pinned SHA for 0.7.x branch
FLATBUFFERS_REVISION="v25.2.10" # To align with flatbuffer version in the latest serialization lib

echo "+++ Cloning VGF Encoder"
mkdir -p $VGF_ENCODER_DIR
git clone https://github.com/arm/ai-ml-sdk-vgf-library.git $VGF_ENCODER_DIR
git -C $VGF_ENCODER_DIR checkout $VGF_ENCODER_REVISION

mkdir -p $FLATBUFFERS_DIR
git clone https://github.com/google/flatbuffers.git $FLATBUFFERS_DIR
git -C $FLATBUFFERS_DIR checkout $FLATBUFFERS_REVISION
