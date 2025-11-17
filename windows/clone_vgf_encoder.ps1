#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param(
  [Parameter(Mandatory)][string]$ExternalDir,
  [Parameter(Mandatory)][string]$Username
)

$VgfEncoderDir = Join-Path $ExternalDir "vgf_encoder"
New-Item -ItemType Directory -Force -Path $VgfEncoderDir | Out-Null
git clone https://github.com/arm/ai-ml-sdk-vgf-library.git $VgfEncoderDir
Push-Location $VgfEncoderDir
git checkout 55d73eaa8c88f74cf5732a3a8902450f208006af
Pop-Location


