#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param([Parameter(Mandatory)][string]$ExternalDir)

$JsonDir = Join-Path $ExternalDir "json"
New-Item -ItemType Directory -Force -Path $JsonDir | Out-Null
git clone https://github.com/nlohmann/json.git $JsonDir
Push-Location $JsonDir
git checkout tags/v3.11.3
Pop-Location


