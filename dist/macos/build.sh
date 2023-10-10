#!/bin/bash

mkdir build_macos
cd build_macos
export CMAKE_PREFIX_PATH=$Qt6_DIR/lib/cmake
cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" -DCMAKE_BUILD_TYPE=Release -DIMGUR_API_KEY=$IMGUR_API_KEY -DVERSION=$TAG_NAME -DCMAKE_INSTALL_PREFIX=installed
cmake --build . --parallel
