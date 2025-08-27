#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

enable_testing()

file(GLOB_RECURSE TEST_SRCS_CORE CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/test/*.cpp)
file(GLOB_RECURSE TEST_SRCS_OPERATOR CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/test/operatorTests/*.cpp)
file(GLOB_RECURSE UTILS CONFIGURE_DEPENDS ${TOOLS_PATH}/utils/src/test/*.cpp)
file(GLOB_RECURSE COMPARISON CONFIGURE_DEPENDS ${TOOLS_PATH}/ModelComparisonTool/src/test/*.cpp)

set(UNITTEST_SRCS ${TEST_SRCS_CORE} ${TEST_SRCS_OPERATOR} ${UTILS} ${COMPARISON})

if (BUILD_TOSA_SERIALIZATION_PARSER)
  file(GLOB_RECURSE TEST_SRCS_TOSA_PARSER ${TOOLS_PATH}/parsers/test/*.cpp)
  list(APPEND UNITTEST_SRCS ${TEST_SRCS_TOSA_PARSER})
endif()

if (BUILD_TOSA_2_SPIRV_GENERATOR AND NOT ANDROID)
  file(GLOB_RECURSE TEST_SRCS_SPIRV_GEN ${TOOLS_PATH}/generator/test/*.cpp)
  list(APPEND UNITTEST_SRCS ${TEST_SRCS_SPIRV_GEN})
endif()

add_executable(tosa_for_spirv_tests ${UNITTEST_SRCS})

target_include_directories(tosa_for_spirv_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  ${TOOLS_PATH}/utils/include
  ${TOOLS_PATH}/utils/src
  ${TOOLS_PATH}/ModelComparisonTool/src)

add_subdirectory(${GOOGLETEST_PATH} ${GOOGLETEST_PATH}/build)

target_link_libraries(tosa_for_spirv_tests PRIVATE
  tosa2spirv
  module_comparator
  utils
  GTest::gtest_main)

find_library(SPIRV_TOOLS_LIBRARY_PATH
             NAMES libSPIRV-Tools.a
             HINTS ${SPIRV_TOOLS_BUILD_PATH}/source
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if (SPIRV_TOOLS_LIBRARY_PATH)
  target_link_libraries(tosa_for_spirv_tests PRIVATE ${SPIRV_TOOLS_LIBRARY_PATH})
else()
  message(WARNING "SPIRV‑Tools not found")
endif()

if (BUILD_TOSA_SERIALIZATION_PARSER)
  target_include_directories(tosa_for_spirv_tests PRIVATE
    ${TOOLS_PATH}/parsers/include
    ${TOOLS_PATH}/parsers/src
    ${TOOLS_PATH}/ModelComparisonTool/include
    ${SERIALIZATION_LIB_SOURCE_PATH}/include
    ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include
    ${FLATBUFFERS_SOURCE_PATH}/include)

  find_library(TOSA_SERIALIZATION_LIBRARY_PATH
               NAMES libtosa_serialization_lib.a
               HINTS ${SERIALIZATION_LIB_BUILD_PATH}
               NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

  if (TOSA_SERIALIZATION_LIBRARY_PATH)
    target_link_libraries(tosa_for_spirv_tests PRIVATE
                          ${TOSA_SERIALIZATION_LIBRARY_PATH} tosaSerializationParser)
  else()
    message(WARNING "TOSA serialization library not found.")
  endif()
endif()

if (BUILD_TOSA_2_SPIRV_GENERATOR)
  target_include_directories(tosa_for_spirv_tests PRIVATE
    ${TOOLS_PATH}/vgfwriter/include
    ${VGF_ENCODER_PATH}/include
    ${NLOHMANN_PATH}/single_include
    ${VGF_ENCODER_PATH}/vgf_dump/include)
  set(VGF_DUMP_LIBRARY libstatic_vgf_dump.a)
  set(VGF_ENCODER_BUILD_PATH "${VGF_ENCODER_PATH}/build")

  find_library(VGF_DUMP_LIBRARY_PATH
               NAMES ${VGF_DUMP_LIBRARY}
               HINTS
                  ${VGF_ENCODER_BUILD_PATH}/vgf_dump
                  ${VGF_ENCODER_BUILD_PATH}
               NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

  find_library(VGF_UTILS_LIBRARY_PATH
          NAMES vgf-utils libvgf-utils.a
          HINTS
          ${VGF_ENCODER_BUILD_PATH}/utils
          ${VGF_ENCODER_BUILD_PATH}
          NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

  if (VGF_DUMP_LIBRARY_PATH)
    message(STATUS "VGF dump library located at: ${VGF_DUMP_LIBRARY_PATH}")
    target_link_libraries(tosa_for_spirv_tests PRIVATE
            ${VGF_DUMP_LIBRARY_PATH})
  else()
    message(STATUS "VGF dump library not found")
  endif()

  if (VGF_UTILS_LIBRARY_PATH)
    message(STATUS "VGF utils library located at: ${VGF_UTILS_LIBRARY_PATH}")
    target_link_libraries(tosa_for_spirv_tests PRIVATE
            ${VGF_UTILS_LIBRARY_PATH})
  else()
    message(WARNING "VGF utils library not found")
  endif()

  target_link_libraries(tosa_for_spirv_tests PRIVATE
          ${VGF_DUMP_LIBRARY_PATH}
          ${VGF_UTILS_LIBRARY_PATH}
          ${VGF_ENCODER_LIBRARY_PATH}
          vgfWriter)
endif()

# Don't run unit tests automatically if it is an Android build.
# If building for Android on a different machine (e.g. x86-64), running unit tests can fail.
if (NOT ANDROID)
  include(GoogleTest)
  gtest_discover_tests(tosa_for_spirv_tests)
else()
  message(STATUS "Skipping gtest_discover_tests for Android cross‑build")
endif()
