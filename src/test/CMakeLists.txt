#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

enable_testing()

add_executable(tosa2spirv_unittests
        GraphTests.cpp
        TensorTests.cpp
        layerTests/AbsTests.cpp
        layerTests/AddTests.cpp
        layerTests/ArgMaxTests.cpp
        layerTests/ArithmeticRightShiftTests.cpp
        layerTests/AvgPool2dTests.cpp
        layerTests/BitwiseAndTests.cpp
        layerTests/BitwiseNotTests.cpp
        layerTests/BitwiseOrTests.cpp
        layerTests/BitwiseXorTests.cpp
        layerTests/CastTests.cpp
        layerTests/CeilTests.cpp
        layerTests/ClampTests.cpp
        layerTests/CosTests.cpp
        layerTests/ResizeTests.cpp
        layerTests/SliceTests.cpp
        layerTests/ClzTests.cpp
        layerTests/ConcatTests.cpp
        layerTests/Conv2dTests.cpp
        layerTests/Conv3dTests.cpp
        layerTests/DepthwiseConv2dTests.cpp
        layerTests/EqualTests.cpp
        layerTests/ExpTests.cpp
        layerTests/RescaleTests.cpp
        layerTests/TileTests.cpp
        layerTests/Fft2dTests.cpp
        layerTests/FloorTests.cpp
        layerTests/GatherTests.cpp
        layerTests/GreaterTests.cpp
        layerTests/GreaterEqualTests.cpp
        layerTests/IntDivTests.cpp
        layerTests/LogTests.cpp
        layerTests/LogicalAndTests.cpp
        layerTests/LogicalLeftShiftTests.cpp
        layerTests/LogicalNotTests.cpp
        layerTests/LogicalOrTests.cpp
        layerTests/LogicalRightShiftTests.cpp
        layerTests/LogicalXorTests.cpp
        layerTests/MatmulTests.cpp
        layerTests/MaxPool2dTests.cpp
        layerTests/MaximumTests.cpp
        layerTests/MinimumTests.cpp
        layerTests/MulTests.cpp
        layerTests/NegateTests.cpp
        layerTests/PadTests.cpp
        layerTests/PowTests.cpp
        layerTests/ReciprocalTests.cpp
        layerTests/ReduceAllTests.cpp
        layerTests/ReduceAnyTests.cpp
        layerTests/ReduceMaxTests.cpp
        layerTests/ReduceMinTests.cpp
        layerTests/ReduceProductTests.cpp
        layerTests/ReduceSumTests.cpp
        layerTests/RescaleTests.cpp
        layerTests/ReshapeTests.cpp
        layerTests/ResizeTests.cpp
        layerTests/ReverseTests.cpp
        layerTests/Rfft2dTests.cpp
        layerTests/RsqrtTests.cpp
        layerTests/ScatterTests.cpp
        layerTests/SelectTests.cpp
        layerTests/SigmoidTests.cpp
        layerTests/SinTests.cpp
        layerTests/SliceTests.cpp
        layerTests/SubTests.cpp
        layerTests/TableTests.cpp
        layerTests/TanhTests.cpp
        layerTests/TileTests.cpp
        layerTests/TransposeTests.cpp
        layerTests/TransposeConv2dTests.cpp
        OperandTests.cpp
        InstructionTests.cpp)

target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE${GOOGLETEST_PATH}/include)

target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/tosa2spirv/src/test>)
target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external>)
target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/generator/include>)

target_link_libraries(tosa2spirv_unittests GTest::gtest_main)
target_link_libraries(tosa2spirv_unittests test_utils)

if (EXISTS ${GOOGLETEST_PATH})
    add_subdirectory(${GOOGLETEST_PATH} googletest EXCLUDE_FROM_ALL)
else()
    find_package(GTest REQUIRED)
endif()

# Don't run unit tests automatically if it is an Android build, this is beacuse if we are building for android on
# a different machine e.g. x86-64 then running the unit tests automatically wil result in failures.
if(NOT ANDROID)
    include(GoogleTest)
    gtest_discover_tests(tosa2spirv_unittests)
else()
    message(STATUS "Skipping gtest_discover_tests for Android cross-build")
endif()