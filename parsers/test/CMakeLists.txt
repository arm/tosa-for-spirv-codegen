#
# Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
enable_testing()

add_executable(tosa2spirv_parser_unittests
               ${PROJECT_SOURCE_DIR}/src/test/TestUtils.cpp
               ${PROJECT_SOURCE_DIR}/src/test/TestUtils.hpp
               ${PROJECT_SOURCE_DIR}/src/test/OpTestUtils.cpp
               ${PROJECT_SOURCE_DIR}/src/test/OpTestUtils.hpp
               ActivationOpsTest.cpp
               ComparisonOpsTest.cpp
               DataLayoutOpsTest.cpp
               DataNodeOpsTest.cpp
               ElementwiseBinaryOpsTest.cpp
               ElementwiseTernaryOpsTest.cpp
               ElementwiseUnaryOpsTest.cpp
               ImageOpsTest.cpp
               ParserTests.cpp
               ReductionOpsTest.cpp
               ScatterGatherOpsTest.cpp
               TensorOpsTest.cpp
               TypeConversionOpsTest.cpp)

target_include_directories(tosa2spirv_parser_unittests
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/parsers/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/test>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)

target_include_directories(tosa2spirv_parser_unittests
        PRIVATE
        ${GOOGLETEST_PATH}/googletest/include
        ${SERIALIZATION_LIB_SOURCE_PATH}/include
        ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include
        ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/flatbuffers/include
        ${SPIRV_HEADERS_SOURCE_PATH}/include
        ${SPIRV_TOOLS_SOURCE_PATH}/include)

target_include_directories(tosa2spirv_parser_unittests PRIVATE ${EFFCEE_SOURCE_PATH})
target_include_directories(tosa2spirv_parser_unittests PRIVATE ${RE2_SOURCE_PATH})
target_include_directories(tosa2spirv_parser_unittests PRIVATE ${ABSEIL_SOURCE_PATH})

target_link_libraries(tosa2spirv_parser_unittests GTest::gtest_main)

target_link_libraries(tosa2spirv_parser_unittests
    -Wl,--whole-archive
    ${EFFCEE_BUILD_PATH}/effcee/libeffcee.a
    -Wl,--no-whole-archive)

target_link_libraries(tosa2spirv_parser_unittests ${EFFCEE_BUILD_PATH}/third_party/re2/libre2.a)

target_link_libraries(tosa2spirv_parser_unittests tosa2spirv)

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
    target_link_libraries(tosa2spirv_parser_unittests ${TOSA_SERIALIZATION_LIBRARY_PATH})
endif()

target_link_libraries(tosa2spirv_parser_unittests
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
    target_link_libraries(tosa2spirv_parser_unittests ${SPIRV_TOOLS_LIBRARY_PATH})
endif()

# Find symbols for flatbuffers
set(FLATBUFFERS_LIBRARY libflatbuffers.a)
find_library(FLATBUFFERS_LIBRARY_PATH
             NAMES ${FLATBUFFERS_LIBRARY}
             HINTS ${SERIALIZATION_LIB_BUILD_PATH}/third_party/flatbuffers
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(NOT FLATBUFFERS_LIBRARY_PATH)
    message(WARNING "Flatbuffers library (${FLATBUFFERS_LIBRARY}) not found.")
else()
    message(STATUS "Flatbuffers library located at: ${FLATBUFFERS_LIBRARY_PATH}")
    target_link_libraries(tosa2spirv_parser_unittests
            -Wl,--whole-archive ${FLATBUFFERS_LIBRARY_PATH} -Wl,--no-whole-archive)
endif()

target_link_libraries(tosa2spirv_parser_unittests tosaSerializationParser)

include(GoogleTest)
gtest_discover_tests(tosa2spirv_parser_unittests)
