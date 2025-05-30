#
# Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)

# Add the generator executable
set(GENERATOR_SOURCE main.cpp)
add_executable(tosa2spirvGenerator ${GENERATOR_SOURCE})

# Include headers for tosa2spirv, the parser and the generatorUtils.
target_include_directories(tosa2spirvGenerator PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(tosa2spirvGenerator PRIVATE ${CMAKE_SOURCE_DIR}/parsers/include)
target_include_directories(tosa2spirvGenerator PRIVATE ${CMAKE_SOURCE_DIR}/vgfwriter/include)

# Include argparse headers
target_include_directories(tosa2spirvGenerator PRIVATE ${ARGPARSE_SOURCE_PATH}/include/argparse)

# Include headers for dependencies and sub dependencies of the TOSA Serialization Library
target_include_directories(tosa2spirvGenerator PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/include)
target_include_directories(tosa2spirvGenerator PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/flatbuffers/include)
target_include_directories(tosa2spirvGenerator PRIVATE ${SERIALIZATION_LIB_SOURCE_PATH}/third_party/half/include)

# Include header for nlohman json header only library
target_include_directories(tosa2spirvGenerator PUBLIC ${NLOHMANN_PATH}/include)

target_link_libraries(tosa2spirvGenerator PRIVATE vgfWriter)
target_link_libraries(tosa2spirvGenerator PRIVATE tosaSerializationParser)
target_link_libraries(tosa2spirvGenerator PRIVATE tosa2spirv)

# Install the generator executable
install(TARGETS tosa2spirvGenerator RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
