#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

enable_testing()

add_executable(tosa2spirv_generator_unittests
               ${PROJECT_SOURCE_DIR}/src/test/TestUtils.cpp
               ${PROJECT_SOURCE_DIR}/src/test/TestUtils.hpp
               GeneratorTests.cpp)

target_include_directories(tosa2spirv_generator_unittests
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/parsers/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/generator/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/test>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)

target_include_directories(tosa2spirv_generator_unittests
        PRIVATE
        ${GOOGLETEST_PATH}/googletest/include
        ${SERIALIZATION_LIB_SOURCE_PATH}/include
        ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include
        ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/flatbuffers/include
        ${SPIRV_HEADERS_SOURCE_PATH}/include
        ${SPIRV_TOOLS_SOURCE_PATH}/include
        ${VGF_ENCODER_PATH}/include
        ${NLOHMANN_PATH}/single_include
        ${VGF_ENCODER_PATH}/vgf_dump/include)

# Link unittests and libstatic_vgf_dump.a to enable use of the VGF encoder class.
set(VGF_DUMP_LIBRARY libstatic_vgf_dump.a)
find_library(VGF_DUMP_LIBRARY_PATH
             NAMES ${VGF_DUMP_LIBRARY}
             HINTS ${VGF_ENCODER_PATH}/build/vgf_dump
             NO_DEFAULT_PATH
             NO_CMAKE_FIND_ROOT_PATH)

if(NOT VGF_DUMP_LIBRARY_PATH)
    message(WARNING "VGF dump library (${VGF_DUMP_LIBRARY}) not found.")
else()
    message(STATUS "VGF dump library located at: ${VGF_DUMP_LIBRARY_PATH}")
    target_link_libraries(tosa2spirv_generator_unittests ${VGF_DUMP_LIBRARY_PATH})
endif()

target_link_libraries(tosa2spirv_generator_unittests GTest::gtest_main)
target_link_libraries(tosa2spirv_generator_unittests ${VGF_ENCODER_LIBRARY_PATH})

if (BUILD_EXAMPLE)
    message(STATUS "Building Example File UnitTests")
    add_definitions(-DBUILD_EXAMPLE)
endif()

# Find symbols for spvtools::SpirvTools::IsValid() etc.
set(SPIRV_TOOLS_LIBRARY libSPIRV-Tools.a)
find_library(SPIRV_TOOLS_LIBRARY_PATH
             NAMES ${SPIRV_TOOLS_LIBRARY}
             HINTS ${SPIRV_TOOLS_BUILD_PATH}/source
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(NOT SPIRV_TOOLS_LIBRARY_PATH)
    message(WARNING "SPIRV-Tools library (${SPIRV_TOOLS_LIBRARY}) not found.")
else()
    message(STATUS "SPIRV-Tools library located at: ${SPIRV_TOOLS_LIBRARY_PATH}")
    target_link_libraries(tosa2spirv_generator_unittests ${SPIRV_TOOLS_LIBRARY_PATH})
endif()

# link to generatorUtils so that we can test its functions
target_link_libraries(tosa2spirv_generator_unittests generatorUtils)

if(BUILD_SHARED_LIBS)
    # Find TOSA Serialization Library
    set(TOSA_SERIALIZATION_LIBRARY libtosa_serialization_lib.a)
    find_library(TOSA_SERIALIZATION_LIBRARY_PATH
                 NAMES ${TOSA_SERIALIZATION_LIBRARY}
                 HINTS ${SERIALIZATION_LIB_BUILD_PATH}
                 NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

    if(NOT TOSA_SERIALIZATION_LIBRARY_PATH)
        message(WARNING "TOSA Serialization Library (${TOSA_SERIALIZATION_LIBRARY}) not found.")
    else()
        message(STATUS "TOSA Serialization Library located at: ${TOSA_SERIALIZATION_LIBRARY_PATH}")
        target_link_libraries(tosa2spirv_generator_unittests ${TOSA_SERIALIZATION_LIBRARY_PATH})
    endif()
endif()

target_link_libraries(tosa2spirv_generator_unittests tosa2spirv)
target_link_libraries(tosa2spirv_generator_unittests tosaSerializationParser)

include(GoogleTest)
gtest_discover_tests(tosa2spirv_generator_unittests)
