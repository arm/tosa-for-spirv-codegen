#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param([Parameter(Mandatory)][string]$ExternalDir)

$TosaSpecDir = Join-Path $ExternalDir "tosa_specification"
git clone "https://github.com/arm/tosa-specification.git" $TosaSpecDir
Push-Location $TosaSpecDir
git checkout v1.0
Pop-Location





