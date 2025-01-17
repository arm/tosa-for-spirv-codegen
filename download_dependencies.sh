#!/usr/bin/env bash

#
# Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

if [ -z "$1" ]
then
    USERNAME=`whoami`
else
    USERNAME=$1
fi

function DownloadGoogleTests {
  scripts/clone_google_tests.sh || exit 1
}

function DownloadSpirvHeaders {
  scripts/clone_spirv_headers.sh $USERNAME || exit 1
}

function DownloadSpirvTools {
  scripts/clone_spirv_tools.sh $USERNAME || exit 1
}

function DownloadTOSASerializationLibrary {
  scripts/clone_tosa_serialization_library.sh || exit 1
}

function DownloadTosaSpec {
  scripts/clone_tosa_spec.sh || exit 1
}

function DownloadVgfEncoder {
  scripts/clone_vgf_encoder.sh $USERNAME || exit 1
}

function DownloadNgpRuntime {
  scripts/clone_ngp_runtime.sh $USERNAME || exit 1 # To be moved to external repo in IVGCVSW-8415
}

function DownloadEffcee {
  scripts/clone_effcee.sh || exit 1
}

function DownloadArgparse {
  scripts/clone_argparse.sh || exit 1
}

function DownloadJson {
  scripts/clone_json.sh || exit 1
}

echo "*** Downloading tosa2spirv dependencies"
mkdir -p external

DownloadGoogleTests
DownloadSpirvHeaders
DownloadSpirvTools
DownloadTOSASerializationLibrary
DownloadTosaSpec
DownloadVgfEncoder
DownloadNgpRuntime
DownloadEffcee
DownloadArgparse
DownloadJson

echo "*** All dependencies downloaded."
