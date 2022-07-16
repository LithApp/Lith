#!/bin/bash

set -x

mkdir build_android
pushd build_android

chmod +x "$Qt6_DIR"/bin/qt-cmake
"$Qt6_DIR"/bin/qt-cmake -DCMAKE_BUILD_TYPE=Release -DQT_HOST_PATH="$Qt6_DIR"/../gcc_64 -DANDROID_OPENSSL_SRCDIR="$ANDROID_OPENSSL_SRCDIR" -DIMGUR_API_KEY="$IMGUR_API_KEY" ..
make -j5
