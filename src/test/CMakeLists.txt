#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

enable_testing()

add_executable(tosa2spirv_unittests
        GraphTests.cpp
        TensorTests.cpp
        operatorTests/AbsOperatorTest.cpp
        operatorTests/AddOperatorTest.cpp
        operatorTests/ArgMaxOperatorTest.cpp
        operatorTests/ArithmeticRightShiftOperatorTest.cpp
        operatorTests/AvgPool2dOperatorTest.cpp
        operatorTests/BitwiseAndOperatorTest.cpp
        operatorTests/BitwiseNotOperatorTest.cpp
        operatorTests/BitwiseOrOperatorTest.cpp
        operatorTests/BitwiseXorOperatorTest.cpp
        operatorTests/CastOperatorTest.cpp
        operatorTests/CeilOperatorTest.cpp
        operatorTests/ClampOperatorTest.cpp
        operatorTests/ClzOperatorTest.cpp
        operatorTests/ConcatOperatorTest.cpp
        operatorTests/Conv2dOperatorTest.cpp
        operatorTests/Conv3dOperatorTest.cpp
        operatorTests/CosOperatorTest.cpp
        operatorTests/DepthwiseConv2dOperatorTest.cpp
        operatorTests/EqualOperatorTest.cpp
        operatorTests/ErfOperatorTest.cpp
        operatorTests/ExpOperatorTest.cpp
        operatorTests/Fft2dOperatorTest.cpp
        operatorTests/FloorOperatorTest.cpp
        operatorTests/GatherOperatorTest.cpp
        operatorTests/GreaterEqualOperatorTest.cpp
        operatorTests/GreaterOperatorTest.cpp
        operatorTests/IntDivOperatorTest.cpp
        operatorTests/LogicalAndOperatorTest.cpp
        operatorTests/LogicalLeftShiftOperatorTest.cpp
        operatorTests/LogicalNotOperatorTest.cpp
        operatorTests/LogicalOrOperatorTest.cpp
        operatorTests/LogicalRightShiftOperatorTest.cpp
        operatorTests/LogicalXorOperatorTest.cpp
        operatorTests/LogOperatorTest.cpp
        operatorTests/MatmulOperatorTest.cpp
        operatorTests/MaximumOperatorTest.cpp
        operatorTests/MaxPool2dOperatorTest.cpp
        operatorTests/MinimumOperatorTest.cpp
        operatorTests/MulOperatorTest.cpp
        operatorTests/NegateOperatorTest.cpp
        operatorTests/PadOperatorTest.cpp
        operatorTests/PowOperatorTest.cpp
        operatorTests/ReciprocalOperatorTest.cpp
        operatorTests/ReduceAllOperatorTest.cpp
        operatorTests/ReduceAnyOperatorTest.cpp
        operatorTests/ReduceMaxOperatorTest.cpp
        operatorTests/ReduceMinOperatorTest.cpp
        operatorTests/ReduceProductOperatorTest.cpp
        operatorTests/ReduceSumOperatorTest.cpp
        operatorTests/RescaleOperatorTest.cpp
        operatorTests/ReshapeOperatorTest.cpp
        operatorTests/ResizeOperatorTest.cpp
        operatorTests/ReverseOperatorTest.cpp
        operatorTests/Rfft2dOperatorTest.cpp
        operatorTests/RsqrtOperatorTest.cpp
        operatorTests/ScatterOperatorTest.cpp
        operatorTests/SelectOperatorTest.cpp
        operatorTests/SigmoidOperatorTest.cpp
        operatorTests/SinOperatorTest.cpp
        operatorTests/SliceOperatorTest.cpp
        operatorTests/SubOperatorTest.cpp
        operatorTests/TableOperatorTest.cpp
        operatorTests/TanhOperatorTest.cpp
        operatorTests/TileOperatorTest.cpp
        operatorTests/TransposeConv2dOperatorTest.cpp
        operatorTests/TransposeOperatorTest.cpp
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
