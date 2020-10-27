QT += qml quick widgets webview multimedia quickcontrols2

CONFIG += c++11

HEADERS += \
    src/clipboardproxy.h \
    src/datamodel.h \
    src/lith.h \
    src/protocol.h \
    src/qmlobjectlist.h \
    src/settings.h \
    src/uploader.h \
    src/weechat.h \
    src/common.h

SOURCES += \
    src/lith.cpp \
    src/main.cpp \
    src/clipboardproxy.cpp \
    src/datamodel.cpp \
    src/protocol.cpp \
    src/qmlobjectlist.cpp \
    src/settings.cpp \
    src/uploader.cpp \
    src/weechat.cpp

RESOURCES += ui/ui.qrc assets/assets.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

android {
    include(<path/to/android_openssl/openssl.pri)
}

ios: {
  QMAKE_INFO_PLIST = dist/info/Info.plist
  QMAKE_TARGET_BUNDLE_PREFIX=cz.rtinbriza.ma
#  QMAKE_DEVELOPMENT_TEAM="Z52EFCPL6D"
#  QMAKE_CODE_SIGN_IDENTITY="Martin Briza"
#  QMAKE_PROVISIONING_PROFILE=61f84d3f-1729-4bb5-a058-54896936692b
  QMAKE_ASSET_CATALOGS=$$PWD/assets/icons/Assets.xcassets
  QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

