#!/usr/bin/env bash

#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

TOSA_SPEC_DIR=$EXTERNAL_DIR/tosa_specification

rm -rf $TOSA_SPEC_DIR

# Pinned SHA for branch
TOSA_SPEC_REVISION="v1.0"

echo "+++ Cloning Tosa Specification"
git clone https://github.com/arm/tosa-specification.git $TOSA_SPEC_DIR

pushd $TOSA_SPEC_DIR > /dev/null
  git checkout $TOSA_SPEC_REVISION
popd > /dev/null
