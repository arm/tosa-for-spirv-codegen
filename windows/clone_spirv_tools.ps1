#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param(
  [Parameter(Mandatory)][string]$ExternalDir,
  [Parameter(Mandatory)][string]$Username
)

$SpirvToolsDir = (Join-Path $ExternalDir "SPIRV-Tools").Replace('\','/')
New-Item -ItemType Directory -Force -Path $SpirvToolsDir | Out-Null
git clone https://github.com/KhronosGroup/SPIRV-Tools.git $SpirvToolsDir
Push-Location $SpirvToolsDir
git checkout 7f2d9ee926f98fc77a3ed1e1e0f113b8c9c49458
Pop-Location



