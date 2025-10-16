#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param([Parameter(Mandatory)][string]$ExternalDir)

$ArgParseDir = Join-Path $ExternalDir "argparse"
New-Item -ItemType Directory -Force -Path $ArgParseDir | Out-Null
git clone https://github.com/p-ranav/argparse.git $ArgParseDir
Push-Location $ArgParseDir
git checkout tags/v3.1
Pop-Location

