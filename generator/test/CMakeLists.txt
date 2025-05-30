#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

enable_testing()

add_executable(tosa2spirv_generator_unittests
               GeneratorTests.cpp)

target_include_directories(tosa2spirv_generator_unittests
                           PRIVATE
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/parsers/include>
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vgfwriter/include>
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external>)

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
set(VGF_ENCODER_BUILD_PATH "${VGF_ENCODER_PATH}/build/vgf_dump")

find_library(VGF_DUMP_LIBRARY_PATH
             NAMES ${VGF_DUMP_LIBRARY}
             HINTS ${VGF_ENCODER_BUILD_PATH}
             NO_DEFAULT_PATH
             NO_CMAKE_FIND_ROOT_PATH)

if(VGF_DUMP_LIBRARY_PATH)
    message(STATUS "VGF dump library located at: ${VGF_DUMP_LIBRARY_PATH}")
    target_link_libraries(tosa2spirv_generator_unittests ${VGF_DUMP_LIBRARY_PATH})
else ()
    message(STATUS "VGF dump library not found")
endif()

target_link_libraries(tosa2spirv_generator_unittests GTest::gtest_main)
target_link_libraries(tosa2spirv_generator_unittests ${VGF_ENCODER_LIBRARY_PATH})

if (BUILD_EXAMPLE)
    message(STATUS "Building Example File UnitTests")
    add_definitions(-DBUILD_EXAMPLE)
endif()

target_link_libraries(tosa2spirv_generator_unittests vgfWriter)
target_link_libraries(tosa2spirv_generator_unittests test_utils)
target_link_libraries(tosa2spirv_generator_unittests tosa2spirv)
target_link_libraries(tosa2spirv_generator_unittests tosaSerializationParser)

# Don't run unit tests automatically if it is an Android build, this is beacuse if we are building for android on
# a different machine e.g. x86-64 then running the unit tests automatically wil result in failures.
if(NOT ANDROID)
    include(GoogleTest)
    gtest_discover_tests(tosa2spirv_generator_unittests)
else()
    message(STATUS "Skipping gtest_discover_tests for Android cross-build")
endif()