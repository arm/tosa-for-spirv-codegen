## Overview
The build-tfsc.ps1 script automates the entire Windows build process. It performs the following steps:

- Clones this repository into windows/tosa-for-spirv-codegen
- Fetches third-party sources and sets them up under external/
- Builds and installs:
- SPIR-V Headers and Tools 
  - FlatBuffers and the TOSA serialization library
  - argparse and the VGF encoder
- Configures the main project using CMake
- Compiles targets:
 - tosa_for_spirv_codegen
 - tosa_for_spirv_tests
- Runs unit tests using CTest

## Prerequisites

| Tool             | Version/Requirement                    |
|------------------|----------------------------------------|
| Operating System | Windows® 10 or 11                      |
| Powershell       | Version 5.1 or later, or PowerShell 7+ |
| Visual Studio    | Visual Studio 2022 with C++ workload   |
| CMake            | Version 3.24 or later                  | 


## Usage

```{tip}
Run the script from the <em>Visual Studio Developer PowerShell</em> or <em>x64 Native Tools Command Prompt for VS 2022</em> to ensure that <code>cl.exe</code> and <code>MSBuild</code> are available in your <code>PATH</code>.
This prevents most environment-related build errors.
```

From the windows/ directory, run:

```powershell
  .\build-tfsc.ps1 -Username <your_username> [-BuildType Release|Debug]
```

## Parameters


| Parameter    | Description                                                                                                             |
|--------------|-------------------------------------------------------------------------------------------------------------------------|
| `-Username`  | (Required) Your Git username                                                                                            |
| `-BuildType` | (Optional) CMake build type. Default: `Release`<br/>Accepted values: `Release`, `Debug`, `RelWithDebInfo`, `MinSizeRel` |

## Troubleshooting

| Issue                                     | Suggested Actions                                                                                                                                                                                                                    |
|-------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| CMake cannot find SPIR-V headers or tools | * Ensure `spirv-headers-install/` and `spirv-tools-install/` exist under `external/` <br> * Check that `CMAKE_PREFIX_PATH` includes these paths (the script sets them automatically, but custom profiles might override them)        |
| MSVC cannot find headers                  | * Confirm that the `INCLUDE` environment variable contains: <br> `- FlatBuffers/` <br> `- serialization_lib/include/` <br> `- src/, src/include/` <br> `- tools/utils/include/` <br> `- googletest/include/` <br> `- SPIR-V headers` |
| flatc.exe not found                       | * Check that `flatc.exe` exists under `serialization_lib/third_party/flatbuffers/` <br> * Rerun the script if the serialization library step failed                                                                                  |
| Authentication errors when cloning        | * Ensure the `-Username` value is correct <br> * Verify that your SSH agent is running and has the right key <br> * Test SSH connection with: <br> ```ssh -T <host>```                                                               |