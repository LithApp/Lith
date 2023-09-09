#!/bin/bash

set -x

mkdir build_wasm
pushd build_wasm

export QT_HOST_PATH="${Qt6_DIR}/../gcc_64"
export CMAKE_PREFIX_PATH=$Qt6_DIR/lib/cmake
cmake .. -DCMAKE_BUILD_TYPE=Release -DIMGUR_API_KEY=$IMGUR_API_KEY -DCMAKE_INSTALL_PREFIX=installed -DCMAKE_TOOLCHAIN_FILE=$Qt6_DIR/lib/cmake/Qt6/qt.toolchain.cmake
cmake --build .
cmake --install .
