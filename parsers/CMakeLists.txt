#
# Copyright © 2023 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)

add_library(tosaSerializationParser
            src/TosaSerializationParser.cpp)

# Find and link the Flatbuffers library, which is needed for the TOSA Serialization Library.
set(FLATBUFFERS_LIBRARY libflatbuffers.a)
find_library(FLATBUFFERS_LIBRARY_PATH
             NAMES ${FLATBUFFERS_LIBRARY}
             HINTS ${SERIALIZATION_LIB_BUILD_PATH}/third_party/flatbuffers
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(NOT FLATBUFFERS_LIBRARY_PATH)
    message(WARNING "Flatbuffers library (${FLATBUFFERS_LIBRARY}) not found.")
else()
    message(STATUS "Flatbuffers library located at: ${FLATBUFFERS_LIBRARY_PATH}")
    target_link_libraries(tosaSerializationParser PUBLIC
            -Wl,--whole-archive ${FLATBUFFERS_LIBRARY_PATH} -Wl,--no-whole-archive)
endif()

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
    target_link_libraries(tosaSerializationParser PRIVATE ${TOSA_SERIALIZATION_LIBRARY_PATH})
endif()

target_include_directories(tosaSerializationParser PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(tosaSerializationParser PRIVATE ${CMAKE_SOURCE_DIR}/parsers/include)
target_include_directories(tosaSerializationParser PRIVATE ${CMAKE_SOURCE_DIR}/parsers/src)

# Include headers for dependencies and sub dependencies of the TOSA Serialization Library
target_include_directories(tosaSerializationParser PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
target_include_directories(tosaSerializationParser PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/flatbuffers/include)
target_include_directories(tosaSerializationParser PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)

# Include SPIR-V headers
target_include_directories(tosa2spirv PUBLIC ${SPIRV_TOOLS_SOURCE_PATH}/include)

target_link_libraries(tosaSerializationParser PRIVATE tosa2spirv)

# Gather the headers to be installed
file(GLOB PUBLIC_HEADERS_PARSER ${CMAKE_SOURCE_DIR}/parsers/include/*.hpp)

# Set the public headers
set_target_properties(tosaSerializationParser
                      PROPERTIES
                      PUBLIC_HEADER "${PUBLIC_HEADERS_PARSER}")

# Install the libraries and headers
install(TARGETS tosaSerializationParser
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})