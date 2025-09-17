#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)

set(MODULE_COMPARATOR_SOURCE
    src/ModuleComparator.cpp
    src/AdjacencyMatrix.cpp
    src/ProblemTree.cpp)

add_library(module_comparator STATIC ${MODULE_COMPARATOR_SOURCE})
target_include_directories(module_comparator PRIVATE ${TOOLS_PATH}/model-comparison/include)
target_include_directories(module_comparator PRIVATE ${TOOLS_PATH}/utils/include)
target_include_directories(module_comparator PRIVATE ${CMAKE_SOURCE_DIR}/include/spirv)
target_include_directories(module_comparator PUBLIC ${SPIRV_HEADERS_SOURCE_PATH}/include)
target_include_directories(module_comparator PUBLIC ${SPIRV_HEADERS_SOURCE_PATH})

target_link_libraries(module_comparator  PRIVATE utils)
if (NOT BUILD_MODEL_COMPARISON_TOOL)
    return()
endif()

add_executable(model-comparison src/ModelParsers.cpp src/main.cpp)
# Include headers for tosa_for_spirv_codegen, the parser and the generatorUtils.
target_include_directories(model-comparison PRIVATE ${TOOLS_PATH}/model-comparison/include)
target_include_directories(model-comparison PRIVATE ${TOOLS_PATH}/model-comparison/src)
target_include_directories(model-comparison PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(model-comparison PRIVATE ${TOOLS_PATH}/parsers/include)
target_include_directories(model-comparison PRIVATE ${TOOLS_PATH}/utils/include)
target_include_directories(model-comparison PRIVATE ${TOOLS_PATH}/utils/include)

# Include argparse headers
target_include_directories(model-comparison PRIVATE ${ARGPARSE_SOURCE_PATH}/include/argparse)

# Include headers for dependencies and sub dependencies of the TOSA Serialization Library
target_include_directories(model-comparison PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
    
target_include_directories(model-comparison PRIVATE ${FLATBUFFERS_SOURCE_PATH}/include)
target_include_directories(model-comparison PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)

# Include header for nlohman json header only library
target_include_directories(model-comparison  PUBLIC ${NLOHMANN_PATH}/include)

target_link_libraries(model-comparison PRIVATE module_comparator)
target_link_libraries(model-comparison PRIVATE vgfWriter)
target_link_libraries(model-comparison PRIVATE tosaSerializationParser)
target_link_libraries(model-comparison PRIVATE tosa_for_spirv_codegen)
target_link_libraries(model-comparison PRIVATE utils)

target_compile_definitions(model-comparison PRIVATE VGFLIB_VK_HELPERS)
target_compile_definitions(model-comparison PRIVATE VGFLIB_FAKE_VULKAN_API)
target_include_directories(model-comparison PRIVATE ${VGF_ENCODER_PATH}/include)
target_include_directories(model-comparison PRIVATE ${SPIRV_HEADERS_SOURCE_PATH}/include)
target_include_directories(model-comparison PRIVATE ${SPIRV_HEADERS_SOURCE_PATH})

# Link generator and libvgf.a to enable use of the VGF encoder class.
set(VGF_ENCODER_LIBRARY libvgf.a)
find_library(VGF_ENCODER_LIBRARY_PATH
        NAMES ${VGF_ENCODER_LIBRARY}
        HINTS ${VGF_ENCODER_PATH}/build/src
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH)

target_link_libraries(model-comparison  PRIVATE tosa_for_spirv_codegen)

if(NOT VGF_ENCODER_LIBRARY_PATH)
    message(WARNING "VGF library (${VGF_ENCODER_LIBRARY}) not found.")
else()
    message(STATUS "VGF library located at: ${VGF_ENCODER_LIBRARY_PATH}")
    target_link_libraries(model-comparison PRIVATE ${VGF_ENCODER_LIBRARY_PATH})
endif()

# Install the model-comparison executable
install(TARGETS model-comparison RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
