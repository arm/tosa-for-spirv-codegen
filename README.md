# tosa_for_spirv_codegen

## Introduction

The tosa_for_spirv_codegen project provides a lightweight set of online tools and API's for a user
to easily generate valid SPIR-V™ binary from TOSA based data structures, without using MLIR or other
compiler frameworks.

**[TOSA Specification](https://github.com/arm/tosa-specification)**

**[SPIR-V Specification](https://registry.khronos.org/SPIR-V/specs/unified1/SPIRV.html)**


## How to get started

By default tosa_for_spirv_codegen will build for the native platform. It also supports
building with the Android™ NDK. It assumes that CMake™ with a minimum
version of 3.17.2 is available in the path.

This project requires several dependencies. To download them, run the script in the root directory:

``` bash
./download_dependencies.sh
```

#### Native Build

Build needed dependencies:
``` bash
./build_dependencies.sh -t Release
```

Create build folder:
``` bash
mkdir -p build
cd build
cmake ..
make
```
To enable position-independent code:
``` bash
cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
```

#### Android Build

To build for Android, you must set the following environment variables. The dependencies script does not download the NDK.

- NDK_VERSION=[NDK version]
- DEFAULT_NDK=android-ndk-${NDK_VERSION}
- NDK_DIR=[path to the NDK]
- ANDROID_API=[API Version]

Optionally:
- ANDROID_ABI=[ABI Target] (default is arm64-v8a)

Build the dependencies:
``` bash
./build_dependencies.sh -t Release -a
```

Run the build command:
``` bash
cmake -S .. -B ./android \
      -DCMAKE_TOOLCHAIN_FILE="${NDK_DIR}/build/cmake/android.toolchain.cmake" \
      -DANDROID_ABI="${ANDROID_ABI}"
      -DANDROID_PLATFORM="android-${ANDROID_API}"
```

#### Build outputs

The build process generates the following outputs:

| Output Path                                                 | Description                                             | Disable/Modify With                         |
|-------------------------------------------------------------|---------------------------------------------------------|---------------------------------------------|
| `tools/generator/tfsc-generator`                            | Command-line executable for SPIR-V generation           | `-DBUILD_TOSA_2_SPIRV_GENERATOR=OFF`        |
| `libtosa_for_spirv_codegen.a` or `.so`                      | Static or shared library                                | `-DBUILD_SHARED_LIBS=YES` to enable shared  |
| `tools/tosaregen/libtosaregen.a`                            | Static library for the TosaRegen module                 | `-DBUILD_TOSA_REGEN=OFF`                    |
| `tools/tosaregen/tosaregen`                                 | Tool for generating operator definitions and test files | `-DBUILD_TOSA_REGEN=OFF`                    |
| `<build folder>/tools/parsers/libtosaSerializationParser.a` | Library to convert TOSA serialization data to SPIR-V    | `-DBUILD_TOSA_SERIALIZATION_PARSER=OFF`     |

CMake only needs to be rerun if the environment changes (for example, new dependencies or source files). Otherwise, use `make` to rebuild.

## Generator Usage

The *tosa_for_spirv_codegen* Generator is a command line executable that makes use of the
[VgfWriter](tools/vgfwriter/include/VgfWriter.hpp) library ('tools/vgfwriter/include/VgfWriter.hpp')
which is a wrapper of the [ML SDK VGF Library](https://arm.github.io/ai-ml-sdk-for-vulkan/vgf-lib/docs/in/index.html).

The VgfWriter library (libvgfWriter.a) is currently enabled by default, but can be disabled by adding
`-DBUILD_VGF_WRITER=OFF` to the CMake command.

The executable can be used to generate SPIR-V in two ways.

### Using a TOSA FlatBuffers File

By using a TOSA FlatBuffers File containing the serialized subgraph you can output a .vgf file
*Example:*
``` bash
./<build folder>/tools/generator/tfsc-generator \
  --tosa-file=examples/simple_maxpool2d.tosa \
  --output-file=output_file.vgf
```

### Using a JSON-formatted FlatBuffers Descriptor

By using a JSON-formatted FlatBuffers descriptor which describes the subgraph you can output a .vgf file.

The FlatBuffers schema file from the TOSA Serialization library must also be
specified using `--schema-file=external/serialization_lib/schema/tosa.fbs`

*Example:*
``` bash
./<build folder>/tools/generator/tfsc-generator \
  --json-file=examples/simple_maxpool2d.json \
  --schema-file=external/serialization_lib/schema/tosa.fbs \
  --output-file=output_file.vgf
```

On a successful execution, the output SPIR-V will be written in binary
format into the output file specified with the `--output-file` option.

This output can be converted into human-readable SPIR-V using the vgf_dump executable in combination
with the SPIR-V Tools disassembler.
``` bash
./external/vgf_encoder/build/vgf_dump/vgf_dump -i output_file.vgf --dump-spirv 0 -o output_file.spv
./external/SPIRV-Tools/build/tools/spirv-dis output_file.spv > disassembledSpv.txt
```

## Examples

Sample TOSA and JSON files are located in the examples directory. They contain a simple MaxPool2d layer.

## Use the TosaSerializationParser API

You can also generate SPIR-V directly from a TosaSerialization data structure using `TosaSerializationParser.hpp`.

``` c++
TosaSerializationParser parser(&block);
auto binarySpirv = parser.GenerateSPIRV();
std::vector<ConstantData>& constants = parser.GetExternalConstants();
```

Unit tests for this are available in `ParserTests.cpp`.

To disable the parser: `-DBUILD_TOSA_SERIALIZATION_PARSER=OFF`

Run unit tests:

``` c++
./tosa_for_spirv_tests
./tosa_for_spirv_tests --gtest_filter=TOSA_FOR_SPIRV_CODEGEN_PARSER*
```

## Use the Graph API

You can also use the graph API to manually create a SPIR-V graph:

*Example:*
``` c++
// Create a module and add a graph.
auto module = CreateModule(m_Version);
auto graph{module, graphName};

// Create an input to the graph, this will be the input to the maxpool2d operator
// DescriptorSet will be 0. Binding can be specified or left to default
auto input = graph.AddInput(Tensor{DataType::int8_t, {1,1}}, 0);

// Maxpool attributes
auto kernel = Attribute{{1, 1}, DataType::int8_t};
auto stride = Attribute{{1, 1}, DataType::int8_t};
auto pad = Attribute{{1, 1}, DataType::int8_t};
auto nanMode = Attribute{{1}, DataType::uint32_t, {1}};

// Create an output Tensor
auto outputTensor = Tensor{DataType::int8_t, std::vector<unsigned int>{1,1}};

// Create the operator
auto maxPool2d = graph.AddMaxPool2dOperator(input, kernel, stride, pad, nanMode, outputTensor);

// Set the graphs output to be the output of the maxPool2d operator added above
graph.AddOutput(maxPool2d, 1);

// Once all IO and operators are set FinalizeGraph() fixes the type of the graph
// After being called the graph becomes immutable
graph.FinalizeGraph();

// Use Module to write the full binary.
std::vector<uint32_t> binary = WriteToBinary(module);
```

Use `graph.AddTensorConstant` or `graph.AddGraphConstant` for constant inputs. For examples, see `src/test/operatorTests/*`.

Run graph API tests:
``` bash
# To run all tests, including graph unit tests
./tosa_for_spirv_tests

# To run specifically graph unit tests
./tosa_for_spirv_tests --gtest_filter=TOSA_FOR_SPIRV_CODEGEN_OPERATOR.*
```

## Documentation

This project supports Doxygen and Sphinx documentation.

### Doxygen
To generate Doxygen documentation:

``` bash
doxygen ./docs/Doxyfile
<browser_name> ./docs/html/index.xhtml
```

### Sphinx
Enable the Sphinx build:

``` bash
cmake -DML_SDK_BUILD_DOCS=ON ..
```
Install dependencies:

``` bash
apt install sphinx
pip install breathe sphinx_rtd_theme myst_parser
```

Generate and open the output:

``` bash
make
<browser_name> build/docs/sphinx-output/index.html
```

## Code Formatting (clang-format 11)

This project uses **clang-format 11** to enforce consistent code style.

### Formatting Code

To format your code:

```bash
clang-format-11 -i path/to/file.cpp
```

## TosaRegen Usage
You use the TosaRegen library through functions in the header files in the `tools/tosaregen/include` directory.


| Header file        | Purpose                                                                                                                           |
|--------------------|-----------------------------------------------------------------------------------------------------------------------------------|
| `TosaOperator.hpp` | Defines a graph-independent TOSA operator representation. This does not depend on SPIR-V or the `tosa_for_spirv_codegen library`. |
| `SpirvParser.hpp`  | Converts `tfsc::spirv::Instruction` instances into structures compatible with `TosaOperator.hpp`.                                 |
| `TosaRegen.hpp`    | Provides the main API. Generates operator definitions or TOSA Serialization Library structures from a `tfsc::spirv::Module`.      |

Each function includes Doxygen-style comments in the code and documentation. You can find test cases in the `tools/tosaregen/src/test` directory. These test files serve as examples for using the library.

You can use the TosaRegen executable to process SPIR-V disassembly text files. It generates either operator definitions or TOSA Serialization Library flatbuffer files.

You can create SPIR-V disassembly files from VGF files or SPIR-V binaries using tools in the `./external` directory.

To run the tool, use the following command:
```bash
./<build_folder>/tools/tosaregen/tosaregen <path_to_spirv_text_file> <action> [options]
```

* Available `action`:

| action  | Description                                                           |
|---------|-----------------------------------------------------------------------|
| `ops`   | Generates operator definitions and writes them to standard output     |
| `tests` | Creates operator test files, grouped by operator type                 |
| `tosa`  | Outputs a `.tosa` file representing the SPIR-V content in TOSA format |

* Available `options`:

| options                               | Description                                                                  |
|---------------------------------------|------------------------------------------------------------------------------|
| `--unique`                            | Used with `ops` or `tests`. Outputs one item per unique operator type        |
| `--dir <output_test_files_directory>` | Used with `tests`. Specifies where to save generated test files              |
| `--overwrite`                         | Used with `tests`. Overwrites existing test files instead of appending       |
| `--output <output_file_name>`         | Used with `tosa`. Sets the output file name for the `.tosa` flatbuffer file  |

To view more information, run the tool without any arguments or use the `--help` option.

## Dependencies
External dependencies used by tosa-for-spirv-codegen:

| Tool                                                                     | License           | Description                                                  | Version                                               |
|--------------------------------------------------------------------------|-------------------|--------------------------------------------------------------|-------------------------------------------------------|
| [GoogleTest](https://github.com/google/googletest)                       | BSD 3-Clause      | C++ testing framework                                        | v1.13.0                                               |
| [SPIRV-Headers](https://github.com/KhronosGroup/SPIRV-Headers)           | Khronos Group Inc | Machine-readable files for the SPIR-V Registry               | 01e0577914a75a2569c846778c2f93aa8e6feddd              |
| [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools)               | Khronos Group Inc | API and commands for processing SPIR-V modules               | 7f2d9ee926f98fc77a3ed1e1e0f113b8c9c49458              |
| [TOSA Serialization Library]((https://gitlab.arm.com/tosa/tosa-tools))   | Apache-2.0        | Provides methods to read and write serialized TOSA graphs    | 988e56145ab66d802ce4a32fa31c0c76e5c801f9 (v2025.11.0) |
| [JSON for Modern C++]((https://github.com/nlohmann/json/))               | MIT               | Provides methods to read and write JSON                      | 3.11.2                                                |
| [argparse](https://github.com/p-ranav/argparse)                          | MIT               | Provides methods to parse command-line arguments             | v3.0                                                  |
| [ai-ml-sdk-vgf-lib](https://github.com/arm/ai-ml-sdk-vgf-library)        | Apache-2.0        | Provides methods to encode SPIR-V TOSA graph into vgf format | 55d73eaa8c88f74cf5732a3a8902450f208006af              |
| [CMake](https://github.com/Kitware/CMake)                                | BSD 3-Clause      | C++ build tool                                               | v3.25.0                                               |

## Build Flags
tosa_for_spirv_codegen uses the following security related build flags in their code:

| Build flags	       |
|---------------------|
| -Wall	           |
| -Wextra             |
| -Wold-style-cast    |
| -Wno-missing-braces |
| -Wconversion        |
| -Wsign-conversion   |
| -Werror             |

## License

The *tosa_for_spirv_codegen* project is licensed under Apache-2.0.

Copyright (c) 2023-2025 Arm Limited.

## Attributions

SPIR-V is a trademark of the Khronos Group Inc.
Android is a trademark of Google Inc.
CMake is a trademark of Kitware Inc.

