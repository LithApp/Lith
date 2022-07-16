#!/bin/bash

set -x

mkdir build_wasm
pushd build_wasm

export CMAKE_PREFIX_PATH=$Qt6_DIR/lib/cmake
cmake .. -DCMAKE_BUILD_TYPE=Release -DIMGUR_API_KEY=$IMGUR_API_KEY
make -j5

