#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.7.0)
project(tosa_for_spirv_codegen)

include(GNUInstallDirs)  # Ensure standard install directory variables are defined

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure custom CMake modules can be found
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# By default tosa_for_spirv_codegen is a static library.
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(BUILD_TOSA_SERIALIZATION_PARSER
       "Build the TOSA Serialization Parser. This library can parse a TOSA Serialization structure and \
        generate SPIRV." ON)
option(BUILD_TOSA_FOR_SPIRV_CODEGEN_GENERATOR
       "Build the tosa_for_spirv_codegen generator. This command line tool generates SPIRV binary from a TOSA \
        Flatbuffers file." ON)
option(BUILD_TOSA_REGEN "Build the tosa regen module." ON)
option(BUILD_VGF_WRITER "Build the VGF Writer library.
                         A wrapper of the VGF Encoder allowing users to create .vgf files with tosa_for_spirv_codegen output." ON)
# Option for Unittest Coverage testing
option(BUILD_FOR_COVERAGE "Output .gcno and .gcda files used for Unittest Coverage checking." OFF)
# By default build UnitTests
option(BUILD_UNITTESTS "Build UnitTests for tosa_for_spirv_codegen" ON)

# Root of the tosa_for_spirv_codegen project
set(TFSC_ROOT "${CMAKE_CURRENT_LIST_DIR}")

# External dependencies root
set(TFSC_EXTERNAL_PATH "${TFSC_ROOT}/external")

# Paths to specific external libraries
set(ARGPARSE_SOURCE_PATH           "${TFSC_EXTERNAL_PATH}/argparse"                 CACHE PATH "")
set(SERIALIZATION_LIB_SOURCE_PATH "${TFSC_EXTERNAL_PATH}/serialization_lib"        CACHE PATH "")
set(FLATBUFFERS_SOURCE_PATH       "${TFSC_EXTERNAL_PATH}/flatbuffers"              CACHE PATH "")
set(SPIRV_TOOLS_SOURCE_PATH       "${TFSC_EXTERNAL_PATH}/SPIRV-Tools"              CACHE PATH "")
set(SPIRV_HEADERS_SOURCE_PATH     "${TFSC_EXTERNAL_PATH}/SPIRV-Headers/include"    CACHE PATH "")
set(GOOGLETEST_PATH               "${TFSC_EXTERNAL_PATH}/googletest"               CACHE PATH "")
set(NLOHMANN_PATH                 "${TFSC_EXTERNAL_PATH}/json"                     CACHE PATH "")

# Build paths for certain dependencies
set(SERIALIZATION_LIB_BUILD_PATH "${SERIALIZATION_LIB_SOURCE_PATH}/build"          CACHE PATH "")
set(SPIRV_TOOLS_BUILD_PATH       "${SPIRV_TOOLS_SOURCE_PATH}/build"                CACHE PATH "")

# Tools path (relative to the repo root)
set(TOOLS_PATH "${TFSC_ROOT}/tools" CACHE PATH "")

set(TOOLS_PATH ${CMAKE_SOURCE_DIR}/tools CACHE PATH "")

# Parser can be enabled without generator but generator requires parser.
if(BUILD_TOSA_FOR_SPIRV_CODEGEN_GENERATOR AND NOT BUILD_TOSA_SERIALIZATION_PARSER)
    message(FATAL_ERROR
        "TOSA Serialization Parser is required to build the tosa_for_spirv_codegen generator.")
endif()

if(BUILD_TOSA_FOR_SPIRV_CODEGEN_GENERATOR AND NOT BUILD_VGF_WRITER)
    message(FATAL_ERROR
        "VGFWriter is required to build the tosa_for_spirv_codegen generator.\nEnable BUILD_VGF_WRITER and try again.")
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

if(COMPILER_IS_GNU_LIKE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Werror -Wold-style-cast -Wno-missing-braces -Wconversion -Wsign-conversion")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-psabi")
    endif()
endif()

# Core library
add_library(tosa_for_spirv_codegen
    src/Graph.cpp
    src/Instruction.cpp
    src/Module.cpp
    src/TosaForSpirvCodegen.cpp
    src/OperatorDefinitions.cpp
    src/Operand.cpp
    src/SPIRVDefinitions.cpp)

target_include_directories(tosa_for_spirv_codegen PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/spirv
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tosa)

file(READ ${CMAKE_CURRENT_LIST_DIR}/include/Version.hpp versionFile)

# Parse the TFSC version components
string(REGEX MATCH "#define TFSC_MAJOR_VERSION ([0-9]*)" _ ${versionFile})
set(TFSC_MAJOR_VERSION ${CMAKE_MATCH_1})
string(REGEX MATCH "#define TFSC_MINOR_VERSION ([0-9]*)" _ ${versionFile})
set(TFSC_MINOR_VERSION ${CMAKE_MATCH_1})

# Define LIB version
set(TFSC_LIB_VERSION "${TFSC_MAJOR_VERSION}.${TFSC_MINOR_VERSION}")

set_target_properties(tosa_for_spirv_codegen PROPERTIES VERSION ${TFSC_LIB_VERSION} SOVERSION ${TFSC_MAJOR_VERSION})

target_include_directories(tosa_for_spirv_codegen PRIVATE
    src
    ${SPIRV_HEADERS_SOURCE_PATH}/include
    ${SPIRV_HEADERS_SOURCE_PATH})

# Optional components
if(BUILD_TOSA_SERIALIZATION_PARSER)
    add_subdirectory(${TOOLS_PATH}/parsers)
endif()

if(BUILD_VGF_WRITER)
    add_subdirectory(${TOOLS_PATH}/vgfwriter)
endif()

if(BUILD_TOSA_FOR_SPIRV_CODEGEN_GENERATOR)
    add_subdirectory(${TOOLS_PATH}/generator)
endif()
if(BUILD_TOSA_REGEN)
    add_subdirectory(${TOOLS_PATH}/tosa-regen)
endif()

if(BUILD_UNITTESTS)
    add_subdirectory(${TOOLS_PATH}/utils)
endif()

if(BUILD_UNITTESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/unittests.cmake)
endif()

# install include directories into their respective folders
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/TosaForSpirvCodegen.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tosa_for_spirv_codegen)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/tosa
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tosa_for_spirv_codegen)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/spirv
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tosa_for_spirv_codegen)

install(TARGETS tosa_for_spirv_codegen)
if(ML_SDK_BUILD_DOCS)
    include(docs/docs.cmake)
endif()

install(TARGETS tosa_for_spirv_codegen)
