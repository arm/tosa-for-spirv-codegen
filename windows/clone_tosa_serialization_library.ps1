#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param(
  [Parameter(Mandatory)][string]$ExternalDir,
  [Parameter(Mandatory)][string]$Username
)

$TosaSerLibDir = Join-Path $ExternalDir "serialization_lib"
New-Item -ItemType Directory -Force -Path $TosaSerLibDir | Out-Null
git clone "https://$Username@review.mlplatform.org/tosa/serialization_lib" $TosaSerLibDir
Push-Location $TosaSerLibDir
git checkout 359145e768e565d7666aa6ebd0abb92784f398cb
git submodule update --init --recursive
Pop-Location




