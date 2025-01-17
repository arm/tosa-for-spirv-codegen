#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

enable_testing()

add_executable(tosa2spirv_unittests
               TestUtils.cpp
               TestUtils.hpp
               OpTestUtils.cpp
               OpTestUtils.hpp
               GraphTests.cpp
               IdGeneratorTests.cpp
               TensorTests.cpp
               layerTests/AbsTests.cpp
               layerTests/AddTests.cpp
               layerTests/ArgmaxTests.cpp
               layerTests/ArithmeticRightShiftTests.cpp
               layerTests/AvgPool2dTests.cpp
               layerTests/BitwiseAndTests.cpp
               layerTests/BitwiseNotTests.cpp
               layerTests/BitwiseOrTests.cpp
               layerTests/BitwiseXorTests.cpp
               layerTests/CastTests.cpp
               layerTests/CeilTests.cpp
               layerTests/ClampTests.cpp
               layerTests/ResizeTests.cpp
               layerTests/SliceTests.cpp
               layerTests/ClzTests.cpp
               layerTests/ConcatTests.cpp
               layerTests/Conv2dTests.cpp
               layerTests/Conv3dTests.cpp
               layerTests/DepthwiseConv2dTests.cpp
               layerTests/EffceeTests.cpp
               layerTests/EqualTests.cpp
               layerTests/ExpTests.cpp
               layerTests/RescaleTests.cpp
               layerTests/TileTests.cpp
               layerTests/Fft2dTests.cpp
               layerTests/FloorTests.cpp
               layerTests/FullyConnectedTests.cpp
               layerTests/GatherTests.cpp
               layerTests/GreaterTests.cpp
               layerTests/GreaterEqualTests.cpp
               layerTests/IntdivTests.cpp
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
               layerTests/SliceTests.cpp
               layerTests/SubTests.cpp
               layerTests/TableTests.cpp
               layerTests/TanhTests.cpp
               layerTests/TileTests.cpp
               layerTests/TransposeTests.cpp
               layerTests/TransposeConv2dTests.cpp)

target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE${GOOGLETEST_PATH}/include)

target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)
target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/test>)
target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external>)
target_include_directories(tosa2spirv_unittests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/generator/include>)

target_include_directories(tosa2spirv_unittests PRIVATE ${EFFCEE_SOURCE_PATH})
target_include_directories(tosa2spirv_unittests PRIVATE ${RE2_SOURCE_PATH})
target_include_directories(tosa2spirv_unittests PRIVATE ${ABSEIL_SOURCE_PATH})
target_include_directories(tosa2spirv_unittests PRIVATE ${SPIRV_TOOLS_SOURCE_PATH}/include)

target_link_libraries(
        tosa2spirv_unittests
        GTest::gtest_main
)

target_link_libraries(tosa2spirv_unittests
    -Wl,--whole-archive
    ${EFFCEE_BUILD_PATH}/effcee/libeffcee.a
    -Wl,--no-whole-archive)

target_link_libraries(tosa2spirv_unittests ${EFFCEE_BUILD_PATH}/third_party/re2/libre2.a)

add_subdirectory(
    ${ABSEIL_SOURCE_PATH}
    ${ABSEIL_SOURCE_PATH}/out)

target_link_libraries(tosa2spirv_unittests tosa2spirv)

target_link_libraries(tosa2spirv_unittests
    absl::strings
    absl::base
    absl::flags
    absl::flags_parse
    absl::status
    absl::str_format
    absl::hash)

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
    target_link_libraries(tosa2spirv_unittests ${SPIRV_TOOLS_LIBRARY_PATH})
endif()

if (EXISTS ${GOOGLETEST_PATH})
    add_subdirectory(${GOOGLETEST_PATH} googletest EXCLUDE_FROM_ALL)
else()
    find_package(GTest REQUIRED)
endif()

include(GoogleTest)
gtest_discover_tests(tosa2spirv_unittests)
