#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)

# Add the generator executable
set(GENERATOR_SOURCE main.cpp)
add_executable(tfsc-generator ${GENERATOR_SOURCE})

# Include headers for tosa_for_spirv_codegen, the parser and the generatorUtils.
target_include_directories(tfsc-generator PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(tfsc-generator PRIVATE ${TOOLS_PATH}/parsers/include)
target_include_directories(tfsc-generator PRIVATE ${TOOLS_PATH}/vgfwriter/include)

# Include argparse headers
target_include_directories(tfsc-generator PRIVATE ${ARGPARSE_SOURCE_PATH}/include/argparse)

# Include headers for dependencies and sub dependencies of the TOSA Serialization Library
target_include_directories(tfsc-generator PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
target_include_directories(tfsc-generator PRIVATE ${FLATBUFFERS_SOURCE_PATH}/include)
target_include_directories(tfsc-generator PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)

# Include header for nlohman json header only library
target_include_directories(tfsc-generator PUBLIC ${NLOHMANN_PATH}/include)

target_link_libraries(tfsc-generator PRIVATE vgfWriter)
target_link_libraries(tfsc-generator PRIVATE tosaSerializationParser)
target_link_libraries(tfsc-generator PRIVATE tosa_for_spirv_codegen)

# Install the generator executable
install(TARGETS tfsc-generator RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
