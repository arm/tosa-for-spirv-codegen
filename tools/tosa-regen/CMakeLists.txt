#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)

set(TOOL_SOURCE src/main.cpp)
set(TOSA_REGEN_SOURCE
    src/TosaOperator.cpp
    src/TosaRegen.cpp
    src/SpirvParser.cpp)

add_library(tosaregen STATIC ${TOSA_REGEN_SOURCE})
target_include_directories(tosaregen PRIVATE ${TOOLS_PATH}/tosa-regen/include)
target_include_directories(tosaregen PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/tosa ${PROJECT_SOURCE_DIR}/include/spirv)
target_include_directories(tosaregen PRIVATE ${SPIRV_HEADERS_SOURCE_PATH}/include)
target_include_directories(tosaregen PRIVATE ${SPIRV_HEADERS_SOURCE_PATH})
target_include_directories(tosaregen PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Executable tool
add_executable(tosa-regen ${TOOL_SOURCE})
target_include_directories(tosa-regen PRIVATE ${TOOLS_PATH}/tosa-regen/include)
target_include_directories(tosa-regen PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/tosa ${PROJECT_SOURCE_DIR}/include/spirv)
target_include_directories(tosa-regen PRIVATE ${TOOLS_PATH}/utils/include)
target_include_directories(tosa-regen PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(tosa-regen PRIVATE ${SPIRV_HEADERS_SOURCE_PATH}/include)
target_include_directories(tosa-regen PRIVATE ${SPIRV_HEADERS_SOURCE_PATH})

# Find and link the Flatbuffers library, which is needed for the TOSA Serialization Library.
set(FLATBUFFERS_LIBRARY libflatbuffers.a)
set(FLATBUFFERS_PATH "${SERIALIZATION_LIB_BUILD_PATH}/third_party/flatbuffers")

find_library(FLATBUFFERS_LIBRARY_PATH
             NAMES ${FLATBUFFERS_LIBRARY}
             HINTS ${SERIALIZATION_LIB_BUILD_PATH}/_deps/flatbuffers-build
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(FLATBUFFERS_LIBRARY_PATH)
    message(STATUS "Flatbuffers library located at: ${FLATBUFFERS_LIBRARY_PATH}")
    target_link_libraries(tosaregen PRIVATE
            -Wl,--whole-archive ${FLATBUFFERS_LIBRARY_PATH} -Wl,--no-whole-archive)
else()
    message(WARNING "Flatbuffers library (${FLATBUFFERS_LIBRARY}) not found.")
endif()


target_include_directories(tosaregen PRIVATE ${FLATBUFFERS_SOURCE_PATH}/include)
target_include_directories(tosa-regen PRIVATE ${FLATBUFFERS_SOURCE_PATH}/include)

# Find TOSA Serialization Library
set(TOSA_SERIALIZATION_LIBRARY libtosa_serialization_lib.a)
set(TOSA_SERIALIZATION_PATH "${SERIALIZATION_LIB_BUILD_PATH}")

find_library(TOSA_SERIALIZATION_LIBRARY_PATH
             NAMES ${TOSA_SERIALIZATION_LIBRARY}
             HINTS ${TOSA_SERIALIZATION_PATH}
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

if(TOSA_SERIALIZATION_LIBRARY_PATH)
    message(STATUS "TOSA Serialization Library located at: ${TOSA_SERIALIZATION_LIBRARY_PATH}")
    target_link_libraries(tosaregen PRIVATE ${TOSA_SERIALIZATION_LIBRARY_PATH})
else()
    message(WARNING "TOSA Serialization Library (${TOSA_SERIALIZATION_LIBRARY_PATH}) not found.")
endif()

# Include headers for dependencies and sub dependencies of the TOSA Serialization Library
target_include_directories(tosaregen PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
target_include_directories(tosaregen PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)
target_include_directories(tosa-regen PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
target_include_directories(tosa-regen PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)


# Link libraries
target_link_libraries(tosaregen PRIVATE tosa_for_spirv_codegen)
target_link_libraries(tosaregen PRIVATE utils)

target_link_libraries(tosa-regen PRIVATE tosa_for_spirv_codegen)
target_link_libraries(tosa-regen PRIVATE utils)
target_link_libraries(tosa-regen PRIVATE tosaregen)

install(TARGETS tosa-regen RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
