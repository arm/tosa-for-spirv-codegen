#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
enable_testing()

add_executable(tosa2spirv_parser_unittests
               ActivationOpsTest.cpp
               ComparisonOpsTest.cpp
               DataLayoutOpsTest.cpp
               DataNodeOpsTest.cpp
               ElementwiseBinaryOpsTest.cpp
               ElementwiseTernaryOpsTest.cpp
               ElementwiseUnaryOpsTest.cpp
               ImageOpsTest.cpp
               ParserTests.cpp
               ParserUtilsTest.cpp
               ReductionOpsTest.cpp
               ScatterGatherOpsTest.cpp
               TensorOpsTest.cpp
               TypeConversionOpsTest.cpp
)

target_include_directories(tosa2spirv_parser_unittests
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/parsers/src>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/parsers/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/test>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)

target_include_directories(tosa2spirv_parser_unittests
        PRIVATE
        ${SERIALIZATION_LIB_SOURCE_PATH}/include
        ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include
        ${FLATBUFFERS_SOURCE_PATH}/include
        ${SPIRV_HEADERS_SOURCE_PATH}/include
        ${SPIRV_TOOLS_SOURCE_PATH}/include)

target_link_libraries(tosa2spirv_parser_unittests test_utils)

target_link_libraries(tosa2spirv_parser_unittests GTest::gtest_main)

target_link_libraries(tosa2spirv_parser_unittests tosaSerializationParser)

# Don't run unit tests automatically if it is an Android build, this is beacuse if we are building for android on
# a different machine e.g. x86-64 then running the unit tests automatically wil result in failures.
if(NOT ANDROID)
    include(GoogleTest)
    gtest_discover_tests(tosa2spirv_parser_unittests)
else()
    message(STATUS "Skipping gtest_discover_tests for Android cross-build")
endif()
