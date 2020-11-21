QT += qml quick widgets multimedia quickcontrols2 xml gui-private

CONFIG += c++17

HEADERS += \
    src/clipboardproxy.h \
    src/datamodel.h \
    src/lith.h \
    src/protocol.h \
    src/qmlobjectlist.h \
    src/settings.h \
    src/uploader.h \
    src/util/nicklistfilter.h \
    src/weechat.h \
    src/common.h \
    src/windowhelper.h

SOURCES += \
    src/lith.cpp \
    src/main.cpp \
    src/clipboardproxy.cpp \
    src/datamodel.cpp \
    src/protocol.cpp \
    src/qmlobjectlist.cpp \
    src/settings.cpp \
    src/uploader.cpp \
    src/util/nicklistfilter.cpp \
    src/weechat.cpp \
    src/windowhelper.cpp


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
linux {
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

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $${PREFIX}/bin
!isEmpty(target.path): INSTALLS += target

