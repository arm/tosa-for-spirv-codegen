#!/usr/bin/env bash

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

JSON_DIR=$EXTERNAL_DIR/json

rm -rf $JSON_DIR

# Pinned SHA for main-with-ARM-patches branch
JSON_REVISION="upstream/release/3.11.3"

echo "+++ Cloning JSON"
mkdir -p $JSON_DIR
git clone https://eu-gerrit-1.euhpc.arm.com/mirrors/opensource/github/nlohmann/json $JSON_DIR

pushd $JSON_DIR > /dev/null
  git checkout $JSON_REVISION
popd > /dev/null
