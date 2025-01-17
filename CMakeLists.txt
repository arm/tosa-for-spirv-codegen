#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
cmake_minimum_required(VERSION 3.7.0)
project(tosa2spirv)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# By default tosa2spirv is a static library.
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(BUILD_TOSA_SERIALIZATION_PARSER "Build the TOSA Serialization Parser.
                                        This library can parse a TOSA Serialization structure and generate SPIRV." ON)
option(BUILD_TOSA_2_SPIRV_GENERATOR "Build the tosa2spirv generator.
                                     This command line tool generates SPIRV binary from a TOSA Flatbuffers file." ON)
# Option for Unittest Coverage testing
option(BUILD_FOR_COVERAGE "Output .gcno and .gcda files used for Unittest Coverage checking." OFF)

set(ARGPARSE_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/argparse CACHE PATH "")
set(SERIALIZATION_LIB_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/serialization_lib CACHE PATH "")
set(SPIRV_TOOLS_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/SPIRV-Tools CACHE PATH "")
set(SPIRV_HEADERS_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/SPIRV-Headers CACHE PATH "")
set(GOOGLETEST_PATH ${CMAKE_SOURCE_DIR}/external/googletest CACHE PATH "")
set(SERIALIZATION_LIB_BUILD_PATH ${CMAKE_SOURCE_DIR}/external/serialization_lib/build CACHE PATH "")
set(SPIRV_TOOLS_BUILD_PATH ${CMAKE_SOURCE_DIR}/external/SPIRV-Tools/build CACHE PATH "")
set(NLOHMANN_PATH ${CMAKE_SOURCE_DIR}/external/json CACHE PATH "")
set(EFFCEE_SOURCE_PATH ${CMAKE_SOURCE_DIR}/external/effcee CACHE PATH "")
set(RE2_SOURCE_PATH ${EFFCEE_SOURCE_PATH}/third_party/re2 CACHE PATH "")
set(ABSEIL_SOURCE_PATH ${EFFCEE_SOURCE_PATH}/third_party/abseil-cpp CACHE PATH "")
set(EFFCEE_BUILD_PATH ${EFFCEE_SOURCE_PATH}/build CACHE PATH "")

# Parser can be enabled without generator but generator requires parser.
if(BUILD_TOSA_2_SPIRV_GENERATOR AND NOT BUILD_TOSA_SERIALIZATION_PARSER)
    message(FATAL_ERROR "TOSA Serialization Parser is required to build the tosa2spirv generator.\n"
                        "Enable BUILD_TOSA_SERIALIZATION_PARSER and try again.")
endif()

if(BUILD_FOR_COVERAGE)
    if(NOT CMAKE_BUILD_TYPE EQUAL "Debug")
        message(WARNING "BUILD_FOR_COVERAGE option enabled; forcing Debug build.")
        set(CMAKE_BUILD_TYPE "Debug")
    endif()
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

add_library(tosa2spirv
            src/OperatorUtils.cpp
            src/Context.cpp
            src/IdGenerator.cpp
            src/Graph.cpp
            src/Instruction.cpp
            src/Module.cpp
            src/Operand.cpp
            src/Tensor.cpp
            src/TypeRegistry.cpp
            src/Writer.cpp
            src/OperatorDefinitions.cpp)

include_directories(tosa2spirv PUBLIC ${SPIRV_HEADERS_SOURCE_PATH}/include)
include_directories(tosa2spirv PUBLIC ${NLOHMANN_PATH})
target_include_directories(tosa2spirv PUBLIC include)
target_include_directories(tosa2spirv PRIVATE src)
target_include_directories(tosa2spirv PRIVATE ${SPIRV_TOOLS_SOURCE_PATH}/include)

if(BUILD_TOSA_SERIALIZATION_PARSER)
    add_subdirectory(parsers)
    add_subdirectory(parsers/test)
endif()

if(BUILD_TOSA_2_SPIRV_GENERATOR)
    add_subdirectory(generator)
    add_subdirectory(generator/test)
endif()

add_subdirectory(src/test)

# Gather the headers to be installed
file(GLOB PUBLIC_HEADERS ${CMAKE_SOURCE_DIR}/include/*.hpp)

# Set the public headers
set_target_properties(tosa2spirv
                      PROPERTIES
                      PUBLIC_HEADER "${PUBLIC_HEADERS}")

# Install the libraries and headers
install(TARGETS tosa2spirv
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})