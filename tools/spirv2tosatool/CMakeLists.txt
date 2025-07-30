#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(CMAKE_CXX_STANDARD 17)

# Add the executable
set(TOOL_SOURCE main.cpp)
add_executable(spirv2tosaTool ${TOOL_SOURCE})

# Include headers
target_include_directories(spirv2tosaTool PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/tosa ${CMAKE_SOURCE_DIR}/include/spirv)
target_include_directories(spirv2tosaTool PRIVATE ${TOOLS_PATH}/utils/include)
target_include_directories(spirv2tosaTool PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_include_directories(spirv2tosaTool PRIVATE ${SPIRV_HEADERS_SOURCE_PATH}/include)
target_include_directories(spirv2tosaTool PRIVATE ${SPIRV_HEADERS_SOURCE_PATH})

# Link libraries
target_link_libraries(spirv2tosaTool PRIVATE tosa2spirv)
target_link_libraries(spirv2tosaTool PRIVATE utils)

install(TARGETS spirv2tosaTool RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
