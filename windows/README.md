Building tosa-for-spirv-codegen on Windows

Overview
- This PowerShell script automates setting up all third‑party dependencies and building the tosa-for-spirv-codegen project on Windows using Visual Studio 2022.
- It clones required external repositories, builds and installs SPIR-V components, builds the TOSA serialization library (and FlatBuffers), builds the VGF encoder, configures the main project with CMake, compiles it, and runs the unit tests.

What it does (high‑level)
1) Clones the main repo (this project) into windows/tosa-for-spirv-codegen.
2) Fetches and prepares third‑party sources under windows/tosa-for-spirv-codegen/external
3) Builds and installs SPIRV-Headers and SPIRV-Tools for CMake consumption.
4) Builds serialization_lib and installs FlatBuffers artifacts (flatbuffers-config.cmake and flatc.exe) to an external install prefix.
5) Builds argparse as an installable package.
6) Builds the VGF encoder using the FlatBuffers/argparse/json paths produced in previous steps.
7) Configures tosa-for-spirv-codegen with CMake, pointing it to the above dependencies.
8) Builds targets tosa_for_spirv_codegen and tosa_for_spirv_tests.
9) Runs unit tests via CTest; if test discovery fails, it expects build/Release/tosa_for_spirv_tests.exe to exist.

Prerequisites
- Operating system: Windows 10/11.
- PowerShell: 5.1+ or PowerShell 7+.
- Visual Studio: Visual Studio 2022 (Desktop development with C++ workload).
- CMake: 3.24+ (bundled with VS 2022 is fine).
- Git: with SSH support. OpenSSH client must be available in PATH.
- Credentials:
  - Optionally, GITLAB_USER and GITLAB_TOKEN environment variables for HTTPS clone of the main repo (instead of SSH).

Usage
- Open “x64 Native Tools Command Prompt for VS 2022” or a PowerShell session where the VS 2022 environment is available.
- From the repository windows directory, run:

  .\build-tfsc.ps1 -Username <your_username> [-BuildType Release|Debug]

Parameters
- -Username (string, required): Your username.
- -BuildType (string, optional): CMake build configuration. Default: Release. Typical values: Release, Debug, RelWithDebInfo, MinSizeRel.

Environment variables honored
- GITLAB_USER / GITLAB_TOKEN: If both are set, the script clones tosa-for-spirv-codegen via HTTPS using these credentials. Otherwise it uses SSH with -Username.
- CMAKE_PREFIX_PATH: The script appends to this so that subsequent CMake configure steps can find installed packages.
- INCLUDE: The script prepends several include roots to ensure MSVC finds headers uniformly across all targets.

Outputs and important directories
- Main repo working copy: windows/tosa-for-spirv-codegen
- External sources and installs: windows/tosa-for-spirv-codegen/external
  - spirv-headers-install
  - spirv-tools-install
  - flatbuffers-install
  - argparse-install
- Project build tree: windows/tosa-for-spirv-codegen/build
  - Executables and libraries appear under build/<Config>/, e.g. build/Release/
  - Unit tests binary: build/<Config>/tosa_for_spirv_tests.exe

Notes on test discovery
- It also enforces gtest_force_shared_crt=ON to avoid CRT mismatch issues on Windows.

Network and access considerations
- If your environment requires a proxy, configure Git and PowerShell accordingly.

Troubleshooting
- CMake cannot find SPIRV-Headers/Tools:
  - Verify the spirv-headers-install and spirv-tools-install directories exist under external/.
  - Ensure CMAKE_PREFIX_PATH contains these directories (the script sets it, but profiles may override it).
- MSVC cannot find headers:
  - Confirm INCLUDE contains FlatBuffers, serialization_lib/include, src, src/include, tools/utils/include, googletest/include, and SPIR-V headers.
- flatc.exe not found:
  - The script expects flatc.exe under serialization_lib/third_party/flatbuffers/; ensure the serialization_lib step completed successfully.
- Authentication failures on clone:
  - Verify -Username and that your SSH agent has the right key loaded. Test with: ssh -T <host>.
- Running in the wrong shell:
  - Use the VS 2022 Developer PowerShell/Prompt so cl.exe and MSBuild toolchain are in PATH.

Idempotence and cleanup
- The script deletes and recreates several build directories (e.g., external/*/build and tosa-for-spirv-codegen/build). If you have local changes there, back them up before running.

Example session
1) Open “Developer PowerShell for VS 2022”.
2) cd <repo>\windows
3) .\build-tfsc.ps1 -Username jdoe -BuildType Release

After success you should see:
- tosa-for-spirv-codegen built under windows/tosa-for-spirv-codegen/build/Release
- Unit tests executed via CTest with ‘--output-on-failure’

Maintenance
- Specific commits/tags for third‑party deps are pinned in the script. If you update them, validate builds and tests accordingly.
