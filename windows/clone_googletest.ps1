#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

param([Parameter(Mandatory)][string]$ExternalDir)

$GoogleTestDir = Join-Path $ExternalDir "googletest"
if (Test-Path $GoogleTestDir) { Remove-Item -Recurse -Force $GoogleTestDir }
git clone https://github.com/google/googletest.git $GoogleTestDir
git -C $GoogleTestDir checkout b796f7d44681514f58a683a3a71ff17c94edb0c1


