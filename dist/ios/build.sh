#!/bin/bash

set -x

mkdir build_ios
cd build_ios

export CMAKE_PREFIX_PATH=$Qt6_DIR/lib/cmake
cmake .. -DCMAKE_BUILD_TYPE=Release -DVERSION=$TAG_NAME -DQMAKE_DEVELOPMENT_TEAM=Z52EFCPL6D -DIMGUR_API_KEY=$IMGUR_API_KEY

xcodebuild -project Lith.xcodeproj -sdk iphoneos -configuration Release clean
xcodebuild -project Lith.xcodeproj -scheme Lith -sdk iphoneos -configuration Release archive -archivePath Lith.xcarchive
xcodebuild -exportArchive -archivePath Lith.xcarchive -exportOptionsPlist ../dist/ios/exportOptions.plist -exportPath .

cd ..
