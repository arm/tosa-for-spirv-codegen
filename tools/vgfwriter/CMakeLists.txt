#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)
set(VGF_ENCODER_PATH ${TFSC_EXTERNAL_PATH}/vgf_encoder CACHE PATH "")
set(VGFWRITER_SOURCE src/VgfWriter.cpp)

# Add utility library
add_library(vgfWriter ${VGFWRITER_SOURCE})

set_target_properties(vgfWriter PROPERTIES VERSION ${TFSC_LIB_VERSION} SOVERSION ${TFSC_MAJOR_VERSION})

# Include headers for tosa_for_spirv_codegen and the vgfwriter.
target_include_directories(vgfWriter PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(vgfWriter PRIVATE ${TOOLS_PATH}/vgfwriter/include)

# Include VGF Encoder headers
target_compile_definitions(vgfWriter PRIVATE VGFLIB_VK_HELPERS)
target_compile_definitions(vgfWriter PRIVATE VGFLIB_FAKE_VULKAN_API)
target_include_directories(vgfWriter PRIVATE ${VGF_ENCODER_PATH}/include)
target_include_directories(vgfWriter PRIVATE ${SPIRV_HEADERS_SOURCE_PATH}/include)
target_include_directories(vgfWriter PRIVATE ${SPIRV_HEADERS_SOURCE_PATH})

# Link generator and libvgf.a to enable use of the VGF encoder class.
set(VGF_ENCODER_LIBRARY libvgf.a)
set(VGF_ENCODER_BUILD_PATH "${VGF_ENCODER_PATH}/build/src")

message(STATUS "VGF_ENCODER_BUILD_PATH: ${VGF_ENCODER_BUILD_PATH}")

find_library(VGF_ENCODER_LIBRARY_PATH
             NAMES ${VGF_ENCODER_LIBRARY}
             HINTS ${VGF_ENCODER_BUILD_PATH}
             NO_DEFAULT_PATH
             NO_CMAKE_FIND_ROOT_PATH)

target_link_libraries(vgfWriter PRIVATE tosa_for_spirv_codegen)

if(VGF_ENCODER_LIBRARY_PATH)
    message(STATUS "VGF library located at: ${VGF_ENCODER_LIBRARY_PATH}")
    target_link_libraries(vgfWriter PRIVATE ${VGF_ENCODER_LIBRARY_PATH})
else ()
    message(STATUS "VGF library not found at: ${VGF_ENCODER_LIBRARY_PATH}")
endif()

# Gather the headers to be installed
file(GLOB PUBLIC_HEADERS_VGFWRITER ${TOOLS_PATH}/vgfwriter/include/*.hpp)

# Set the public headers
set_target_properties(vgfWriter
        PROPERTIES
        PUBLIC_HEADER "${PUBLIC_HEADERS_VGFWRITER}")

# Install the libraries, binaries and headers
install(TARGETS vgfWriter
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tosa_for_spirv_codegen/tools/vgfWriter)
