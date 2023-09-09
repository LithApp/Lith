#!/bin/bash

set -x

mkdir build_android
pushd build_android
mkdir package

#mkdir /home/user/build/dist/
export CMAKE_PREFIX_PATH=$Qt6_DIR/lib/cmake
$Qt6_DIR/bin/qt-cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release -DANDROID_EXTRA_LIBS=../android_openssl/latest/arm/libcrypto_1_1.so:../android_openssl/latest/arm/libssl_1_1.so: -DQMAKE_CFLAGS=-funwind-tables -DQMAKE_CXXFLAGS+=-funwind-tables -DIMGUR_API_KEY=$IMGUR_API_KEY -DLITH_FORCE_DOWNLOAD_DEPENDENCIES=ON
cmake --build . --target Lith
cmake --build . --target apk

#${Qt6_DIR}/../gcc_64/bin/androiddeployqt --input android-Lith-deployment-settings.json --output android-build/ --deployment bundled --gradle --debug
