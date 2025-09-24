#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)

add_library(tosaSerializationParser
            src/TosaSerializationParser.cpp)

set_target_properties(tosaSerializationParser PROPERTIES VERSION ${TFSC_LIB_VERSION} SOVERSION ${TFSC_MAJOR_VERSION})

# Find and link the Flatbuffers library, which is needed for the TOSA Serialization Library.
set(FLATBUFFERS_LIBRARY libflatbuffers.a)
set(FLATBUFFERS_PATH "${SERIALIZATION_LIB_BUILD_PATH}/third_party/flatbuffers")

find_library(FLATBUFFERS_LIBRARY_PATH
             NAMES ${FLATBUFFERS_LIBRARY}
             HINTS ${SERIALIZATION_LIB_BUILD_PATH}/_deps/flatbuffers-build
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(FLATBUFFERS_LIBRARY_PATH)
    message(STATUS "Flatbuffers library located at: ${FLATBUFFERS_LIBRARY_PATH}")
    target_link_libraries(tosaSerializationParser PUBLIC
            -Wl,--whole-archive ${FLATBUFFERS_LIBRARY_PATH} -Wl,--no-whole-archive)
else()
    message(WARNING "Flatbuffers library (${FLATBUFFERS_LIBRARY}) not found.")
endif()

target_include_directories(tosaSerializationParser PUBLIC ${FLATBUFFERS_SOURCE_PATH}/include)

# Find TOSA Serialization Library
set(TOSA_SERIALIZATION_LIBRARY libtosa_serialization_lib.a)
set(TOSA_SERIALIZATION_PATH "${SERIALIZATION_LIB_BUILD_PATH}")

find_library(TOSA_SERIALIZATION_LIBRARY_PATH
             NAMES ${TOSA_SERIALIZATION_LIBRARY}
             HINTS ${TOSA_SERIALIZATION_PATH}
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(TOSA_SERIALIZATION_LIBRARY_PATH)
    message(STATUS "TOSA Serialization Library located at: ${TOSA_SERIALIZATION_LIBRARY_PATH}")
    target_link_libraries(tosaSerializationParser PUBLIC ${TOSA_SERIALIZATION_LIBRARY_PATH})
else()
    message(WARNING "TOSA Serialization Library (${TOSA_SERIALIZATION_LIBRARY_PATH}) not found.")
endif()

target_include_directories(tosaSerializationParser PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(tosaSerializationParser PRIVATE ${TOOLS_PATH}/parsers/include)
target_include_directories(tosaSerializationParser PRIVATE ${TOOLS_PATH}/parsers/src)

# Include headers for dependencies and sub dependencies of the TOSA Serialization Library
target_include_directories(tosaSerializationParser PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
target_include_directories(tosaSerializationParser PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)

# Include SPIR-V headers
target_include_directories(tosa_for_spirv_codegen PUBLIC ${SPIRV_TOOLS_SOURCE_PATH}/include)
target_link_libraries(tosaSerializationParser PUBLIC tosa_for_spirv_codegen)

# Gather the headers to be installed
file(GLOB PUBLIC_HEADERS_PARSER ${TOOLS_PATH}/parsers/include/*.hpp)

# Set the public headers
set_target_properties(tosaSerializationParser
        PROPERTIES
        PUBLIC_HEADER "${PUBLIC_HEADERS_PARSER}")

# Install the libraries and headers
install(TARGETS tosaSerializationParser
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tosa_for_spirv_codegen/tools/parsers)
