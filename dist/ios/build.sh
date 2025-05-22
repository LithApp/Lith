#!/bin/bash

set -e
set -x

mkdir -p build_ios
cd build_ios

export QT_HOST_PATH="${QT_ROOT_DIR}/../macos"
cmake .. -GXcode -DCMAKE_PREFIX_PATH=$QT_ROOT_DIR -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_TOOLCHAIN_FILE=$QT_ROOT_DIR/lib/cmake/Qt6/qt.toolchain.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLITH_VERSION=$VERSION_NAME -DIMGUR_API_KEY=$IMGUR_API_KEY -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE="Manual"

xcodebuild -project Lith.xcodeproj -scheme Lith -configuration RelWithDebInfo archive -archivePath Lith.xcarchive DEVELOPMENT_TEAM=Z52EFCPL6D
xcodebuild -exportArchive -archivePath Lith.xcarchive -exportOptionsPlist ../dist/ios/exportOptions.plist -exportPath .

cd ..
