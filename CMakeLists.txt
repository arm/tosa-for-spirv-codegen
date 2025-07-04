#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.7.0)
project(tosa2spirv)

include(GNUInstallDirs)  # Ensure standard install directory variables are defined

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(BUILD_TOSA_SERIALIZATION_PARSER
       "Build the TOSA Serialization Parser. This library can parse a TOSA Serialization structure and \
        generate SPIRV." ON)
option(BUILD_TOSA_2_SPIRV_GENERATOR
       "Build the tosa2spirv generator. This command line tool generates SPIRV binary from a TOSA \
        Flatbuffers file." OFF)
option(BUILD_VGF_WRITER
       "Build the VGF Writer library. A wrapper of the VGF Encoder allowing users to create .vgf files \
        with tosa2spirv output." ON)
option(BUILD_MODEL_COMPARISON_TOOL "Build Model comparison tool" ON)
option(BUILD_SPIRV_2_TOSA_TOOL "Build the spirv2tosa utility tool." ON)
option(BUILD_VGF_WRITER "Build the VGF Writer library.
                         A wrapper of the VGF Encoder allowing users to create .vgf files with tosa2spirv output." ON)
# Option for Unittest Coverage testing
option(BUILD_FOR_COVERAGE "Output .gcno and .gcda files used for Unittest Coverage checking." OFF)
option(BUILD_UNITTESTS "Build UnitTests for tosa2spirv" ON)

# External paths
set(ARGPARSE_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/argparse CACHE PATH "")
set(SERIALIZATION_LIB_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/serialization_lib CACHE PATH "")
set(FLATBUFFERS_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/flatbuffers CACHE PATH "")
set(SPIRV_TOOLS_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/SPIRV-Tools CACHE PATH "")
set(SPIRV_HEADERS_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/SPIRV-Headers/include CACHE PATH "")
set(GOOGLETEST_PATH ${CMAKE_SOURCE_DIR}/external/googletest CACHE PATH "")
set(SERIALIZATION_LIB_BUILD_PATH ${CMAKE_SOURCE_DIR}/external/serialization_lib/build CACHE PATH "")
set(SPIRV_TOOLS_BUILD_PATH ${CMAKE_SOURCE_DIR}/external/SPIRV-Tools/build CACHE PATH "")
set(NLOHMANN_PATH ${CMAKE_SOURCE_DIR}/external/json CACHE PATH "")
set(EFFCEE_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/effcee CACHE PATH "")
set(RE2_SOURCE_PATH ${EFFCEE_SOURCE_PATH}/third_party/re2 CACHE PATH "")
set(ABSEIL_SOURCE_PATH ${EFFCEE_SOURCE_PATH}/third_party/abseil-cpp CACHE PATH "")
set(EFFCEE_BUILD_PATH ${EFFCEE_SOURCE_PATH}/build CACHE PATH "")

# Option validation
if(BUILD_TOSA_2_SPIRV_GENERATOR AND NOT BUILD_TOSA_SERIALIZATION_PARSER)
    message(FATAL_ERROR
        "TOSA Serialization Parser is required to build the tosa2spirv generator.")
endif()

if(BUILD_TOSA_2_SPIRV_GENERATOR AND NOT BUILD_VGF_WRITER)
    message(FATAL_ERROR
        "VGFWriter is required to build the tosa2spirv generator.\nEnable BUILD_VGF_WRITER and try again.")
endif()

# Coverage setup
if(BUILD_FOR_COVERAGE)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "BUILD_FOR_COVERAGE option enabled; forcing Debug build.")
        set(CMAKE_BUILD_TYPE "Debug")
    endif()
    if(NOT BUILD_UNITTESTS)
        message(WARNING "BUILD_FOR_COVERAGE option enabled; forcing build of Unittests.")
        set(BUILD_UNITTESTS ON)
    endif()
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

# Core library
add_library(tosa2spirv
    src/Graph.cpp
    src/Instruction.cpp
    src/Module.cpp
    src/tosa2spirv.cpp
    src/OperatorDefinitions.cpp
    src/Operand.cpp
    src/SPIRVDefinitions.cpp)

target_include_directories(tosa2spirv PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/spirv
    ${CMAKE_SOURCE_DIR}/include/tosa)

target_include_directories(tosa2spirv PRIVATE
    src
    ${SPIRV_HEADERS_SOURCE_PATH}/include
    ${SPIRV_HEADERS_SOURCE_PATH})

# Optional components
if(BUILD_TOSA_SERIALIZATION_PARSER)
    add_subdirectory(parsers)
endif()

if(BUILD_VGF_WRITER)
    add_subdirectory(vgfwriter)
endif()

if(BUILD_TOSA_2_SPIRV_GENERATOR)
    add_subdirectory(generator)
endif()
if(BUILD_SPIRV_2_TOSA_TOOL)
    add_subdirectory(spirv2tosatool)
endif()

if(BUILD_UNITTESTS OR BUILD_MODEL_COMPARISON_TOOL)
    add_subdirectory(testutils)
    add_subdirectory(ModelComparisonTool)  # comparison functionality needed for testing
endif()

if(BUILD_UNITTESTS)
    include(${CMAKE_SOURCE_DIR}/cmake/unittests.cmake)
endif()

# Install interface
install(FILES ${CMAKE_SOURCE_DIR}/include/tosa2spirv.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/tosa
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/spirv
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS tosa2spirv)
