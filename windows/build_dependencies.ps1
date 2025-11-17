#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param(
  [Parameter(Mandatory)][string]$RepoRoot,
  [string]$BuildType = "Release"
)

$Here = Split-Path -Parent $MyInvocation.MyCommand.Definition

& "$Here\build_spirv_tools.ps1"              -RepoRoot $RepoRoot -BuildType $BuildType
& "$Here\build_tosa_serialization_library.ps1" -RepoRoot $RepoRoot -BuildType $BuildType
& "$Here\build_vgf_encoder.ps1"              -RepoRoot $RepoRoot -BuildType $BuildType

