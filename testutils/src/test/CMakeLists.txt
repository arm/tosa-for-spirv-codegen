#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

enable_testing()

add_executable(testutils_unittests
               ModuleComparisonTest.cpp
               EnumMapTest.cpp
               AssemblyUtilsTest.cpp
               OpGeneratorTest.cpp)

target_include_directories(testutils_unittests PRIVATE
        $<BUILD_INTERFACE${GOOGLETEST_PATH}/include)

target_include_directories(testutils_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/tosa2spirv/src/test>)
target_include_directories(testutils_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external>)
target_include_directories(testutils_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/generator/include>)

target_link_libraries(testutils_unittests test_utils)

if (NOT ANDROID)
    include(GoogleTest)
    gtest_discover_tests(testutils_unittests)
else()
    message(STATUS "Skipping gtest_discover_tests for Android cross-build")
endif()
