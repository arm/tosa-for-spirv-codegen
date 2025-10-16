#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param(
  [Parameter(Mandatory)][string]$ExternalDir,
  [Parameter(Mandatory)][string]$Username
)

$SpirvHeadersDir = (Join-Path $ExternalDir "SPIRV-Headers").Replace('\','/')
New-Item -ItemType Directory -Force -Path $SpirvHeadersDir | Out-Null
git clone https://github.com/KhronosGroup/SPIRV-Headers.git $SpirvHeadersDir
Push-Location $SpirvHeadersDir
git checkout 01e0577914a75a2569c846778c2f93aa8e6feddd
Pop-Location


