#!/bin/bash

set -x

mkdir build_android
pushd build_android

if [ -z "${ANDROID_OPENSSL_PATH}" ]; then
    ANDROID_OPENSSL_PATH="${ANDROID_SDK_ROOT}/android_openssl/ssl_3"
fi

export QT_HOST_PATH="${Qt6_DIR}/../gcc_64"
export CMAKE_PREFIX_PATH=${Qt6_DIR}/lib/cmake
cmake .. -DQT_DEBUG_FIND_PACKAGE=ON -DCMAKE_BUILD_TYPE=Release -DIMGUR_API_KEY=$IMGUR_API_KEY -DCMAKE_INSTALL_PREFIX=installed -DCMAKE_TOOLCHAIN_FILE=$Qt6_DIR/lib/cmake/Qt6/qt.toolchain.cmake -DANDROID_OPENSSL_PATH="${ANDROID_OPENSSL_PATH}"

cmake --build . --target Lith -- -j2
cmake --build . --target apk -- -j2

popd
