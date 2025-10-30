#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

# Requires PowerShell 5+ and OpenSSH for SCP
#$IsWindows = $env:OS -like "*Windows*"

param (
    [Parameter(Mandatory)]
    [string]$Username,
    [string]$BuildType = "Release"
)

function AssertZeroExitCode { 
    param ([string]$ErrorMessage) 
    if ($LASTEXITCODE -ne 0) { 
        Write-Error $ErrorMessage 
        exit $LASTEXITCODE 
    } 
}

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$RepoRoot  = Join-Path $ScriptDir "tosa-for-spirv-codegen"

Write-Host "+++ Cloning tosa-for-spirv-codegen repo"
if (!(Test-Path $RepoRoot)) {
    $user  = $env:GITLAB_USER
    $token = $env:GITLAB_TOKEN
    if ($user -and $token) {
        git clone "https://$user`:$token@gitlab.arm.com/artificial-intelligence/neural-graphics/tosa-for-spirv-codegen.git" $RepoRoot
    } else {
        git clone "ssh://$Username@gitlab.arm.com/artificial-intelligence/neural-graphics/tosa-for-spirv-codegen.git" $RepoRoot
    }
    AssertZeroExitCode "Cloning tosa-for-spirv-codegen repo failed"
} else {
    Write-Host "Repo already exists at $RepoRoot"
}

# Sanity check to fail early if clone path is wrong
if (-not (Test-Path (Join-Path $RepoRoot 'CMakeLists.txt'))) {
    throw "CMakeLists.txt not found at $RepoRoot. Clone likely went to a different directory."
}

Write-Host "+++ Updating tosa-for-spirv-codegen"
git -C $RepoRoot checkout main
AssertZeroExitCode "git checkout main failed"
git -C $RepoRoot pull --ff-only origin main
git -C $RepoRoot --no-pager log -1
AssertZeroExitCode "git pull failed"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$RepoRoot = Join-Path $ScriptDir "tosa-for-spirv-codegen"

$ExternalDir = Join-Path $RepoRoot "external"
$ArgParseDir = Join-Path $ExternalDir "argparse"
$GoogleTestDir = Join-Path $ExternalDir "googletest"
$JsonDir = Join-Path $ExternalDir "json"
$SpirvHeadersDir = (Join-Path $ExternalDir "SPIRV-Headers").Replace('\','/') #CMake on Windows supports forward slashes, and using them avoids the escape problem entirely.
$SpirvToolsDir = (Join-Path $ExternalDir "SPIRV-Tools").Replace('\','/')
$SpirvToolsBuildDir = Join-Path $SpirvToolsDir 'build'
$SpirvToolsInstall  = Join-Path $ExternalDir 'spirv-tools-install'
$TosaSerLibDir = Join-Path $ExternalDir "serialization_lib"
$TosaSpecDir = Join-Path $ExternalDir "tosa_specification" 
$VgfEncoderDir = Join-Path $ExternalDir "vgf_encoder"

if (-not (Test-Path $ExternalDir)) {
    New-Item -ItemType Directory -Force -Path $ExternalDir | Out-Null
}

# Download dependencies
$DownloadDepsScript = Join-Path $ScriptDir "download_dependencies.ps1"
if (-not (Test-Path $DownloadDepsScript)) {
    throw "download_dependencies.ps1 not found at $DownloadDepsScript"
}

Write-Host "+++ Downloading all dependency repositories"
& $DownloadDepsScript -RepoRoot $RepoRoot -Username $Username
AssertZeroExitCode "Downloading dependencies failed"

# Build dependencies
$BuildDepsScript = Join-Path $ScriptDir "build_dependencies.ps1"
if (-not (Test-Path $BuildDepsScript)) {
    throw "build_dependencies.ps1 not found at $BuildDepsScript"
}

Write-Host "+++ Building all dependency projects"
& $BuildDepsScript -RepoRoot $RepoRoot -BuildType $BuildType
AssertZeroExitCode "Building dependencies failed"

$SpirvToolsBuild    = Join-Path $SpirvToolsDir "build"
$SpirvToolsInstall  = Join-Path $ExternalDir   "spirv-tools-install"
$foundSpvCfg = Get-ChildItem -Path $SpirvToolsInstall -Recurse -Filter 'SPIRV-ToolsConfig.cmake' -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $foundSpvCfg) { throw "SPIRV-ToolsConfig.cmake not found under $SpirvToolsInstall" }
$SpirvToolsConfigDir = $foundSpvCfg.Directory.FullName -replace '\\','/'

$SpirvHeadersInstall  = Join-Path $ExternalDir 'spirv-headers-install'
$SpirvHeadersConfigDir = (Join-Path $SpirvHeadersInstall 'lib\cmake\SPIRV-Headers') -replace '\\','/'
$SpirvHeadersInc       = (Join-Path $SpirvHeadersInstall 'include') -replace '\\','/'
$env:INCLUDE = "$SpirvHeadersInc;$env:INCLUDE"

$TosaSerBuildDir       = Join-Path $TosaSerLibDir "build"
$FlatbuffersInstall    = Join-Path $ExternalDir 'flatbuffers-install'
$FlatbuffersConfigDir  = (Join-Path $FlatbuffersInstall 'lib\cmake\flatbuffers') -replace '\\','/'
$FlatcExe              = (Join-Path $TosaSerLibDir 'third_party\flatbuffers\flatc.exe') -replace '\\','/'
$env:flatbuffers_DIR   = $FlatbuffersConfigDir
$env:flatbuffers_ROOT  = $FlatbuffersInstall

$ArgparseInstall = Join-Path $ExternalDir "argparse-install"
$VgfBuildDir     = Join-Path $VgfEncoderDir "build"

# Build tosa-for-spirv-codegen
Write-Host "Starting to build tfsc..."
$TfscBuildDir = Join-Path $RepoRoot "build"
# Clean build directory
if (Test-Path $TfscBuildDir) {
    Remove-Item -Recurse -Force $TfscBuildDir
}

New-Item -ItemType Directory -Path $TfscBuildDir | Out-Null

Push-Location $TfscBuildDir
$CMakeArgs = @(
  "-DBUILD_EXAMPLE=1",
  "-DBUILD_SHARED_LIBS=NO",
  "-DSPIRV-Tools_DIR=$SpirvToolsConfigDir",
  "-DSPIRV-Headers_DIR=$SpirvHeadersConfigDir",
  "-DCMAKE_POLICY_DEFAULT_CMP0074=NEW",
  "-DSPIRV_TOOLS_BUILD_PATH=$SpirvToolsBuild"
)

$testDirs = @(
    "$TfscBuildDir\src\test",
    "$TfscBuildDir\parsers\test",
    "$TfscBuildDir\generator\test",
    "$TfscBuildDir\testutils\src\test"
)

# After VGF build completes:
$VgfLibDir  = Join-Path $VgfBuildDir "src\$BuildType"
$TosaSerLib = Join-Path $TosaSerBuildDir "$BuildType\tosa_serialization_lib.lib"
$FbIncDir   = (Join-Path $FlatbuffersInstall "include") -replace '\\','/'
$FbLibPath = Join-Path $TosaSerBuildDir "_deps\flatbuffers-build\$BuildType\flatbuffers.lib"
$FbLibPath = $FbLibPath -replace '\\','/'
$TosaSerInc = (Join-Path $TosaSerLibDir "include") -replace '\\','/'

# Append to the args used for the tosa-for-spirv-codegen configure:
$CMakeArgs += @(
  "-DTOSA_SERIALIZATION_LIBRARY_PATH=$TosaSerLib",
  "-DFLATC_PATH=$FlatcExe",
  "-DFLATBUFFERS_LIBRARY_PATH=$FbLibPath",
  "-DFLATBUFFERS_INCLUDE_DIR=$FbIncDir",
  "-Dflatbuffers_DIR=$FlatbuffersConfigDir",
  "-DFLATBUFFERS_PATH=$TosaSerLibDir\third_party\flatbuffers",
  "-DVGF_ENCODER_BUILD_PATH=$VgfBuildDir\src",
  "-DVGF_ENCODER_LIBRARY_PATH=$VgfLibDir\vgf.lib",
  "-DVGF_DUMP_LIBRARY_PATH=$VgfLibDir\vgf_dump.lib",
  "-DVGF_UTILS_LIBRARY_PATH=$VgfLibDir\vgf_utils.lib"
)

# Project and third-party include roots needed by the tests
$SrcInc1  = (Join-Path $RepoRoot 'src') -replace '\\','/'
$SrcInc2  = (Join-Path $RepoRoot 'src\include') -replace '\\','/'
$UtilsInc = (Join-Path $RepoRoot 'tools\utils\include') -replace '\\','/'
$GTestInc = (Join-Path $GoogleTestDir 'googletest\include') -replace '\\','/'
$SpvInc   = $SpirvHeadersInc  # from the SPIRV-Headers install step

$commonIncludeFlags = "/I`"$FbIncDir`" /I`"$TosaSerInc`" /I`"$SrcInc1`" /I`"$SrcInc2`" /I`"$UtilsInc`" /I`"$GTestInc`" /I`"$SpvInc`""

$CMakeArgs += @(
  "-DCMAKE_CXX_FLAGS=$commonIncludeFlags /EHsc /Zc:preprocessor /D_CRT_SECURE_NO_WARNINGS",
  "-DCMAKE_C_FLAGS=$commonIncludeFlags /Zc:preprocessor /D_CRT_SECURE_NO_WARNINGS"
)

# Make FlatBuffers + serialization headers visible to ALL targets in the project
$env:INCLUDE = "$FbIncDir;$TosaSerInc;$SrcInc1;$SrcInc2;$UtilsInc;$GTestInc;$SpvInc;$env:INCLUDE"

# Also make the packages discoverable via CMake package search
$env:CMAKE_PREFIX_PATH = (@($env:CMAKE_PREFIX_PATH, $FlatbuffersInstall, $ArgparseInstall, $SpirvToolsInstall) | Where-Object { $_ -and $_.Trim() -ne "" }) -join ';'

$graphTests = Join-Path $RepoRoot 'src\test\GraphTests.cpp'
if (Test-Path $graphTests) {
  $t = Get-Content $graphTests -Raw
  if ($t -notmatch '(?m)^\s*#\s*include\s*<array>') {
    Set-Content $graphTests ("#include <array>`r`n" + $t)
  }
}

# Fixes GoogleTest CRT mismatch
$CMakeArgs += @(
  "-Dgtest_force_shared_crt=ON"
)

Write-Host "Configure Cmake for Tfsc..."
& cmake -S "$RepoRoot" -B . -G "Visual Studio 17 2022" -A x64 @CMakeArgs
AssertZeroExitCode "CMake for tosa-for-spirv-codegen failed"

Write-Host "Building Tfsc"
& cmake --build . --config $BuildType --parallel --target tosa_for_spirv_codegen --target tosa_for_spirv_tests
AssertZeroExitCode "Build of tosa-for-spirv-codegen failed"

# Run unit tests
Write-Host "Running unit tests..."
Push-Location $TfscBuildDir
& ctest -C $BuildType --output-on-failure -j 8
AssertZeroExitCode "Unit tests failed"

# Fallback: if CTest did not register tests for some reason, run the binary directly
$TestExe = Join-Path $TfscBuildDir "$BuildType\tosa_for_spirv_tests.exe"
if (-not (Test-Path $TestExe)) {
  throw "tosa_for_spirv_tests.exe not found at $TestExe"
}
Pop-Location

$neededDlls = @(
    "libstdc++-6.dll",
    "libgcc_s_seh-1.dll",
    "libwinpthread-1.dll"
)

$sourceDirs = @(
    "$TfscBuildDir\bin",
    "$TfscBuildDir",
    "$TfscBuildDir\parsers",
    "$TfscBuildDir\vgfwriter"
)
