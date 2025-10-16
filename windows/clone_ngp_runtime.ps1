#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param(
  [Parameter(Mandatory)][string]$ExternalDir,
  [Parameter(Mandatory)][string]$Username
)

$NgpRuntimeDir = Join-Path $ExternalDir "ngp-runtime"
New-Item -ItemType Directory -Force -Path $NgpRuntimeDir | Out-Null

# Do not init submodules
git clone --recurse-submodules=0 "ssh://$Username@eu-gerrit-1.euhpc.arm.com:29418/GPU-SW/compute/ngp-runtime.git" $NgpRuntimeDir

Push-Location $NgpRuntimeDir
git checkout 8797d08cc1a1db76a0d8ef35fc5a7c78dac828f1

# Patch .gitmodules to replace ARM+user with your $Username before any submodule action
$regex = "ARM\+\w+"
Get-ChildItem -Path . -Filter .gitmodules -Recurse | ForEach-Object {
  (Get-Content $_.FullName) -replace $regex, $Username | Set-Content $_.FullName
}

git submodule deinit -f --all
git submodule sync --recursive
git submodule update --init --recursive
Pop-Location


