#!/usr/bin/env bash

PROJECT_ROOT="$(git rev-parse --show-toplevel)"
EMSCRIPTEN="~/Workspace/WebAssembly-C-Cpp-Reference/thirdparty/emscripten" # obviously not good

function generateAndMoveToProjectWorkspaceSubdirectory()
{
    if (( $# != 1)); then echo "$0 requires 1 args"; exit 1; fi

        BUILD_DIR="$1"

            if [ ! -d "${BUILD_DIR}" ]; then mkdir "${BUILD_DIR}"; fi

                pushd "${BUILD_DIR}"
}

generateAndMoveToProjectWorkspaceSubdirectory 'web' 

cmake $PROJECT_ROOT \
          -G "Unix Makefiles" \
          -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake \
          && make

