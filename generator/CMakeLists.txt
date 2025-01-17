#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)
set(VGF_ENCODER_PATH ${CMAKE_SOURCE_DIR}/external/vgf_encoder CACHE PATH "")
set(UTILS_SOURCE src/GeneratorUtils.cpp
        src/ResourceInfoVisitor.cpp)

# Add utility library
add_library(generatorUtils ${UTILS_SOURCE})

# Include headers for tosa2spirv, the parser and the generatorUtils.
target_include_directories(generatorUtils PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(generatorUtils PRIVATE ${CMAKE_SOURCE_DIR}/parsers/include)
target_include_directories(generatorUtils PRIVATE ${CMAKE_SOURCE_DIR}/generator/include)

# Include headers for dependencies and sub dependencies of the TOSA Serialization Library
target_include_directories(generatorUtils PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
target_include_directories(generatorUtils PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/flatbuffers/include)
target_include_directories(generatorUtils PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)

# Include VGF Encoder headers
target_include_directories(generatorUtils PRIVATE ${VGF_ENCODER_PATH}/include)
target_compile_definitions(generatorUtils PRIVATE VGFLIB_FAKE_VULKAN_API)

# Include argparse headers
target_include_directories(generatorUtils PRIVATE ${ARGPARSE_SOURCE_PATH}/include/argparse)

# Include header for nlohman json header only library
target_include_directories(generatorUtils PUBLIC ${NLOHMANN_PATH}/include)

# Link the generatorUtils with the parser
target_link_libraries(generatorUtils PRIVATE tosaSerializationParser)

# Link generator and libvgf.a to enable use of the VGF encoder class.
set(VGF_ENCODER_LIBRARY libvgf.a)
find_library(VGF_ENCODER_LIBRARY_PATH
             NAMES ${VGF_ENCODER_LIBRARY}
             HINTS ${VGF_ENCODER_PATH}/build/src
             NO_DEFAULT_PATH
             NO_CMAKE_FIND_ROOT_PATH)

if(NOT VGF_ENCODER_LIBRARY_PATH)
    message(WARNING "VGF library (${VGF_ENCODER_LIBRARY}) not found.")
else()
    message(STATUS "VGF library located at: ${VGF_ENCODER_LIBRARY_PATH}")
    target_link_libraries(generatorUtils PRIVATE ${VGF_ENCODER_LIBRARY_PATH})
endif()

# Add the generator executable
set(GENERATOR_SOURCE main.cpp)
add_executable(tosa2spirvGenerator ${GENERATOR_SOURCE})

# Include headers for tosa2spirv, the parser and the generatorUtils.
target_include_directories(tosa2spirvGenerator PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(tosa2spirvGenerator PRIVATE ${CMAKE_SOURCE_DIR}/parsers/include)
target_include_directories(tosa2spirvGenerator PRIVATE ${CMAKE_SOURCE_DIR}/generator/include)

# Include argparse headers
target_include_directories(tosa2spirvGenerator PRIVATE ${ARGPARSE_SOURCE_PATH}/include/argparse)

# Include headers for dependencies and sub dependencies of the TOSA Serialization Library
target_include_directories(tosa2spirvGenerator PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
target_include_directories(tosa2spirvGenerator PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/flatbuffers/include)
target_include_directories(tosa2spirvGenerator PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)

# Include VGF Encoder headers
target_include_directories(tosa2spirvGenerator PRIVATE ${VGF_ENCODER_PATH}/include)

# Include header for nlohman json header only library
target_include_directories(tosa2spirvGenerator PUBLIC ${NLOHMANN_PATH}/include)

target_link_libraries(tosa2spirvGenerator PRIVATE generatorUtils)

if(BUILD_SHARED_LIBS)
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
        target_link_libraries(tosa2spirvGenerator PRIVATE ${TOSA_SERIALIZATION_LIBRARY_PATH})
    endif()
endif()

target_link_libraries(tosa2spirvGenerator PRIVATE tosaSerializationParser)
target_link_libraries(tosa2spirvGenerator PRIVATE tosa2spirv)

# Gather the headers to be installed
file(GLOB PUBLIC_HEADERS_GENERATOR ${CMAKE_SOURCE_DIR}/generator/include/*.hpp)

# Set the public headers
set_target_properties(tosa2spirvGenerator
                      PROPERTIES
                      PUBLIC_HEADER "${PUBLIC_HEADERS_GENERATOR}")

# Install the libraries, binaries and headers
install(TARGETS tosa2spirvGenerator generatorUtils
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
