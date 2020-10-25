#!/bin/bash

set -x

mkdir build_ios
cd build_ios

$Qt5_Dir/bin/qmake .. VERSION=$TAG_NAME QMAKE_DEVELOPMENT_TEAM=Z52EFCPL6D

xcodebuild -project Lith.xcodeproj -sdk iphoneos -configuration Release clean
xcodebuild -project Lith.xcodeproj -scheme Lith -sdk iphoneos -configuration Release archive -archivePath Lith.xcarchive
xcodebuild -exportArchive -archivePath Lith.xcarchive -exportOptionsPlist ../dist/ios/exportOptions.plist -exportPath .
xcrun altool --validate-app --file Lith.ipa --username "$APPLEID_NAME" --password "$APPLEID_PASSWORD"
xcrun altool --upload-app --file Lith.ipa --username "$APPLEID_NAME" --password "$APPLEID_PASSWORD"
