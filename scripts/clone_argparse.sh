#!/usr/bin/env bash

#
# Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

USERNAME=$1

source "$(dirname "$0")/scripts_common.sh"

ARGPARSE_DIR=$EXTERNAL_DIR/argparse

rm -rf $ARGPARSE_DIR

# Pinned SHA for main-with-ARM-patches branch
ARGPARSE_REVISION="tags/v3.1"

echo "+++ Cloning Argparse"
mkdir -p $ARGPARSE_DIR
git clone https://github.com/p-ranav/argparse.git $ARGPARSE_DIR

pushd $ARGPARSE_DIR > /dev/null
  git checkout $ARGPARSE_REVISION
popd > /dev/null
