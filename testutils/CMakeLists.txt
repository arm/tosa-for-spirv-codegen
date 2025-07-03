#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF)
set(TEST_UTILS_PATH ${CMAKE_SOURCE_DIR}/test_utils/ CACHE PATH "")

set(TEST_UTILS_SOURCE src/ModuleComparator.cpp
                      src/OpTestUtils.cpp
                      src/AssemblyUtils.cpp
                      src/ValidationUtils.cpp
                      src/EnumMaps.cpp
                      src/OpGenerator.cpp)

add_library(test_utils ${TEST_UTILS_SOURCE})

target_include_directories(test_utils PRIVATE SPIRV_TOOLS_SOURCE_PATH)
target_include_directories(test_utils PUBLIC include)
target_include_directories(test_utils PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(test_utils PUBLIC ${SPIRV_HEADERS_SOURCE_PATH}/include)
target_include_directories(test_utils PUBLIC ${SPIRV_HEADERS_SOURCE_PATH})

target_link_libraries(test_utils PUBLIC tosa2spirv)
target_link_libraries(test_utils PUBLIC GTest::gtest_main)

# Find symbols for spvtools::SpirvTools::IsValid() etc.
set(SPIRV_TOOLS_LIBRARY libSPIRV-Tools.a)
find_library(SPIRV_TOOLS_LIBRARY_PATH
             NAMES ${SPIRV_TOOLS_LIBRARY}
             HINTS ${SPIRV_TOOLS_BUILD_PATH}/source
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

# Find TOSA Serialization Library
set(TOSA_SERIALIZATION_LIBRARY libtosa_serialization_lib.a)
set(TOSA_SERIALIZATION_PATH "${SERIALIZATION_LIB_BUILD_PATH}")

find_library(TOSA_SERIALIZATION_LIBRARY_PATH
             NAMES ${TOSA_SERIALIZATION_LIBRARY}
             HINTS ${TOSA_SERIALIZATION_PATH}
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(NOT TOSA_SERIALIZATION_LIBRARY_PATH)
    message(WARNING "TOSA Serialization Library (${TOSA_SERIALIZATION_LIBRARY}) not found.")
else()
    message(STATUS "TOSA Serialization Library located at: ${TOSA_SERIALIZATION_LIBRARY_PATH}")
    target_link_libraries(test_utils PUBLIC ${TOSA_SERIALIZATION_LIBRARY_PATH})
endif()


set(FLATBUFFERS_LIBRARY libflatbuffers.a)
set(FLATBUFFERS_PATH "${SERIALIZATION_LIB_BUILD_PATH}/third_party/flatbuffers")

find_library(FLATBUFFERS_LIBRARY_PATH
        NAMES ${FLATBUFFERS_LIBRARY}
        HINTS ${SERIALIZATION_LIB_BUILD_PATH}/third_party/flatbuffers/
        NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(NOT FLATBUFFERS_LIBRARY_PATH)
    message(WARNING "Flatbuffers library (${FLATBUFFERS_LIBRARY}) not found.")
else()
    message(STATUS "Flatbuffers library located at: ${FLATBUFFERS_LIBRARY_PATH}")
    target_link_libraries(test_utils PUBLIC
            -Wl,--whole-archive ${FLATBUFFERS_LIBRARY_PATH} -Wl,--no-whole-archive)
endif()

target_include_directories(test_utils PUBLIC ${EFFCEE_SOURCE_PATH})
target_include_directories(test_utils PUBLIC ${RE2_SOURCE_PATH})
target_include_directories(test_utils PUBLIC ${ABSEIL_SOURCE_PATH})
add_subdirectory(${ABSEIL_SOURCE_PATH} ${ABSEIL_SOURCE_PATH}/out)

target_link_libraries(test_utils PUBLIC
                      -Wl,--whole-archive
                      ${EFFCEE_BUILD_PATH}/effcee/libeffcee.a
                      -Wl,--no-whole-archive)

target_link_libraries(test_utils PUBLIC ${EFFCEE_BUILD_PATH}/third_party/re2/libre2.a)

target_link_libraries(test_utils PUBLIC
                      absl::strings
                      absl::base
                      absl::flags
                      absl::flags_parse
                      absl::status
                      absl::str_format
                      absl::hash)

if(NOT SPIRV_TOOLS_LIBRARY_PATH)
    message(WARNING "SPIRV-Tools library (${SPIRV_TOOLS_LIBRARY}) not found.")
else()
    message(STATUS "SPIRV-Tools library located at: ${SPIRV_TOOLS_LIBRARY_PATH}")
    target_link_libraries(test_utils PUBLIC ${SPIRV_TOOLS_LIBRARY_PATH})
endif()

target_include_directories(test_utils PUBLIC ${EFFCEE_SOURCE_PATH})
add_subdirectory(src/test)
