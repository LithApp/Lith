#!/bin/bash

set -x

mkdir build_android
pushd build_android
mkdir package

#mkdir /home/user/build/dist/
${Qt6_DIR}/bin/qmake -r .. CONFIG+=debug ANDROID_EXTRA_LIBS+=../android_openssl/latest/arm64/libcrypto_1_1.so ANDROID_EXTRA_LIBS+=../android_openssl/latest/arm64/libssl_1_1.so QMAKE_CFLAGS+=-funwind-tables QMAKE_CXXFLAGS+=-funwind-tables IMGUR_API_KEY=$IMGUR_API_KEY
make -j5
make apk_install_target
#make install INSTALL_ROOT=/home/user/build/dist/

${Qt6_DIR}/bin/androiddeployqt --input android-Lith-deployment-settings.json --output android-build/ --android-platform 29 --deployment bundled --gradle --debug

