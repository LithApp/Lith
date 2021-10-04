#!/bin/bash

set -x

mkdir build_ios
cd build_ios

$Qt6_DIR/bin/qmake .. VERSION=$TAG_NAME QMAKE_DEVELOPMENT_TEAM=Z52EFCPL6D IMGUR_API_KEY=$IMGUR_API_KEY

xcodebuild -project Lith.xcodeproj -sdk iphoneos -configuration Release clean
xcodebuild -project Lith.xcodeproj -scheme Lith -sdk iphoneos -configuration Release archive -archivePath Lith.xcarchive
xcodebuild -exportArchive -archivePath Lith.xcarchive -exportOptionsPlist ../dist/ios/exportOptions.plist -exportPath .

cd ..
