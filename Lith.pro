QT += qml quick widgets multimedia quickcontrols2 xml gui-private websockets

!versionAtLeast(QT_VERSION, 6.2.0) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 6.2 or newer")
}

CONFIG += c++17

HEADERS += \
    src/clipboardproxy.h \
    src/datamodel.h \
    src/lith.h \
    src/logger.h \
    src/protocol.h \
    src/qmlobjectlist.h \
    src/search.h \
    src/settings.h \
    src/uploader.h \
    src/util/formatstringsplitter.h \
    src/util/formattedstring.h \
    src/util/messagelistfilter.h \
    src/util/nicklistfilter.h \
    src/util/reflection.h \
    src/weechat.h \
    src/common.h \
    src/windowhelper.h \
    src/util/colortheme.h \
    src/util/sockethelper.h

SOURCES += \
    src/lith.cpp \
    src/logger.cpp \
    src/main.cpp \
    src/clipboardproxy.cpp \
    src/datamodel.cpp \
    src/protocol.cpp \
    src/qmlobjectlist.cpp \
    src/search.cpp \
    src/settings.cpp \
    src/uploader.cpp \
    src/util/formatstringsplitter.cpp \
    src/util/formattedstring.cpp \
    src/util/messagelistfilter.cpp \
    src/util/nicklistfilter.cpp \
    src/util/reflection.cpp \
    src/weechat.cpp \
    src/windowhelper.cpp \
    src/util/colortheme.cpp \
    src/util/sockethelper.cpp


INCLUDEPATH += \
    src

RESOURCES += ui/ui.qrc assets/assets.qrc

#placeholders for sed
#VERSION=
#IMGUR_API_KEY=
DEFINES += IMGUR_API_KEY=\\\"$${IMGUR_API_KEY}\\\"

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

android {
    #include(<path/to/android_openssl/openssl.pri)
}
linux:!android {
    QT += dbus

    icon.files += \
        assets/icons/linux/hicolor
        #assets/icons/linux/hicolor/16x16 \
        #assets/icons/linux/hicolor/22x22 \
        #assets/icons/linux/hicolor/24x24 \
        #assets/icons/linux/hicolor/32x32 \
        #assets/icons/linux/hicolor/48x48 \
        #assets/icons/linux/hicolor/64x64 \
        #assets/icons/linux/hicolor/96x96 \
        #assets/icons/linux/hicolor/128x128 \
        #assets/icons/linux/hicolor/192x192 \
        #assets/icons/linux/hicolor/256x256 \
        #assets/icons/linux/hicolor/512x512 \
    icon.path = $${PREFIX}/share/icons

    desktopfile.files = "dist/linux/app.lith.Lith.desktop"
    desktopfile.path = "$${PREFIX}/share/applications/"

    appdatafile.files = "dist/linux/app.lith.Lith.appdata.xml"
    appdatafile.path = "$${PREFIX}/share/metainfo/"

    INSTALLS += icon desktopfile appdatafile
}
win32 {
    RC_ICONS = assets/icons/icon.ico
}

ios {
    OBJECTIVE_HEADERS += \
    src/iosclipboard.h
    OBJECTIVE_SOURCES += \
    src/iosclipboard.mm

    QMAKE_LFLAGS += -F/System/Library/Frameworks
    LIBS += -framework Foundation
    LIBS += -framework UIKit
}


ios: {
  QMAKE_INFO_PLIST = dist/ios/Info.plist
  QMAKE_TARGET_BUNDLE_PREFIX=cz.rtinbriza.ma
#  QMAKE_DEVELOPMENT_TEAM="Z52EFCPL6D"
#  QMAKE_CODE_SIGN_IDENTITY="Martin Briza"
#  QMAKE_PROVISIONING_PROFILE=61f84d3f-1729-4bb5-a058-54896936692b
  QMAKE_ASSET_CATALOGS=$$PWD/assets/icons/Assets.xcassets
  QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"
}

wasm {
    QMAKE_WASM_PTHREAD_POOL_SIZE=3
    QMAKE_LFLAGS_DEBUG += -g4
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $${PREFIX}/bin
!isEmpty(target.path): INSTALLS += target

