//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

/// Macro utils
#define STRINGIFY_VALUE(s) STRINGIFY_MACRO(s)
#define STRINGIFY_MACRO(s) #s

// tosa_for_spirv_codegen version components
#define TFSC_MAJOR_VERSION 1
#define TFSC_MINOR_VERSION 0
#define TFSC_PATCH_VERSION 0

/// TFSC_VERSION: "X.Y.Z"
/// where:
///   X = Major version number
///   Y = Minor version number
///   Z = Patch version number
#define TFSC_VERSION STRINGIFY_VALUE(TFSC_MAJOR_VERSION) "." \
                      STRINGIFY_VALUE(TFSC_MINOR_VERSION) "." \
                      STRINGIFY_VALUE(TFSC_PATCH_VERSION)
