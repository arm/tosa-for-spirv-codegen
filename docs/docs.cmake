#
# Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

include(cmake/doxygen.cmake)
include(cmake/sphinx.cmake)

if(NOT DOXYGEN_FOUND OR NOT SPHINX_FOUND)
    return()
endif()

file(MAKE_DIRECTORY ${SPHINX_GEN_DIR})

# copy MD files for inclusion into the published docs
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CONTRIBUTING.md ${SPHINX_GEN_DIR}/CONTRIBUTING.md COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README.md ${SPHINX_GEN_DIR}/README.md COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/windows/README.md ${SPHINX_GEN_DIR}/windows/README.md COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SECURITY.md ${SPHINX_GEN_DIR}/SECURITY.md COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/LICENSES/Apache-2.0.txt ${SPHINX_GEN_DIR}/LICENSES/Apache-2.0.txt COPYONLY)

# set source inputs list
file(GLOB_RECURSE DOC_SRC_FILES CONFIGURE_DEPENDS RELATIVE ${SPHINX_SRC_DIR_IN} ${SPHINX_SRC_DIR_IN}/*)
set(DOC_SRC_FILES_FULL_PATHS "")
foreach(SRC_IN IN LISTS DOC_SRC_FILES)
    set(DOC_SOURCE_FILE_IN "${SPHINX_SRC_DIR_IN}/${SRC_IN}")
    set(DOC_SOURCE_FILE "${SPHINX_SRC_DIR}/${SRC_IN}")
    configure_file(${DOC_SOURCE_FILE_IN} ${DOC_SOURCE_FILE} COPYONLY)
    list(APPEND DOC_SRC_FILES_FULL_PATHS ${DOC_SOURCE_FILE})
endforeach()

list(APPEND DOC_SRC_FILES_FULL_PATHS
        ${SPHINX_GEN_DIR}/CONTRIBUTING.md
        ${SPHINX_GEN_DIR}/README.md
        ${SPHINX_GEN_DIR}/SECURITY.md)

add_custom_command(
    OUTPUT ${SPHINX_INDEX_HTML}
    DEPENDS ${DOC_SRC_FILES_FULL_PATHS}
    COMMAND ${SPHINX_EXECUTABLE} -b html -Dbreathe_projects.MLSDK=${DOXYGEN_XML_GEN} ${SPHINX_SRC_DIR} ${SPHINX_BLD_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating tosa_for_spirv_codegen build script help documentation"
    VERBATIM
)

# main target to build the docs
add_custom_target(tosa_for_spirv_codegen_doc ALL DEPENDS tosa_for_spirv_codegen_doxy_doc tosa_for_spirv_codegen_sphx_doc SOURCES "${SPHINX_SRC_DIR}/index.rst")