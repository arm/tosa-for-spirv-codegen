#!/usr/bin/env bash

#
# Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

# Pinned SHA's for v1.13.0
GOOGLE_TEST_REVISION="b796f7d44681514f58a683a3a71ff17c94edb0c1"

cd $EXTERNAL_DIR

if [ ! -d googletest ]; then
    echo "+++ Cloning googletest"
    git clone https://github.com/google/googletest.git && cd googletest && git checkout $GOOGLE_TEST_REVISION
fi