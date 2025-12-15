#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param(
  [Parameter(Mandatory)][string]$ExternalDir,
  [Parameter(Mandatory)][string]$Username
)

$TosaToolsDir = Join-Path $ExternalDir "tosa-tools"
New-Item -ItemType Directory -Force -Path $TosaToolsDir | Out-Null
git clone "https://gitlab.arm.com/tosa/tosa-tools" $TosaToolsDir
Push-Location $TosaToolsDir/serialization
git checkout 988e56145ab66d802ce4a32fa31c0c76e5c801f9
git submodule update --init --recursive
Pop-Location




