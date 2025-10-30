#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param(
  [Parameter(Mandatory)] [string]$RepoRoot,
  [Parameter(Mandatory)] [string]$Username
)

$ExternalDir = Join-Path $RepoRoot "external"
if (-not (Test-Path $ExternalDir)) {
  New-Item -ItemType Directory -Force -Path $ExternalDir | Out-Null
}

$Here = Split-Path -Parent $MyInvocation.MyCommand.Definition

& "$Here\clone_argparse.ps1"                   -ExternalDir $ExternalDir
& "$Here\clone_googletest.ps1"                 -ExternalDir $ExternalDir
& "$Here\clone_json.ps1"                       -ExternalDir $ExternalDir

& "$Here\clone_spirv_headers.ps1"              -ExternalDir $ExternalDir -Username $Username
& "$Here\clone_spirv_tools.ps1"                -ExternalDir $ExternalDir -Username $Username

& "$Here\clone_tosa_serialization_library.ps1" -ExternalDir $ExternalDir -Username $Username
& "$Here\clone_tosa_specification.ps1"         -ExternalDir $ExternalDir
& "$Here\clone_vgf_encoder.ps1"                -ExternalDir $ExternalDir -Username $Username
