#!/usr/bin/env bash

#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

source "$(dirname "$0")/scripts_common.sh"

TOSA_SERIALIZATION_LIB_DIR=$EXTERNAL_DIR/serialization_lib

rm -rf $TOSA_SERIALIZATION_LIB_DIR

TOSA_SERIALIZATION_LIB_REVISION="5a85d261cfef40ad8746eabe5f9108ea51811ed8" # v1.0 April release

echo "+++ Cloning TOSA Serialization Library"
git clone "https://review.mlplatform.org/tosa/serialization_lib" $TOSA_SERIALIZATION_LIB_DIR

pushd $TOSA_SERIALIZATION_LIB_DIR > /dev/null
  git checkout $TOSA_SERIALIZATION_LIB_REVISION
  # Cherrypick an android fix
  git fetch ssh://$USER@eu-gerrit-1.euhpc.arm.com:29418/mltech/mirrors/tosa/serialization_lib refs/changes/01/757901/1 && git cherry-pick FETCH_HEAD

  # Initialise Flatbuffers dependency, which is currently v2.0.6
  git submodule update --init --recursive
popd > /dev/null
