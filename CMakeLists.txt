cmake_minimum_required(VERSION 3.20)
project(Lith)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets Gui Quick Multimedia QuickControls2 WebSockets Xml REQUIRED)
find_package(Qt6Gui COMPONENTS Private REQUIRED)
if (UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(Qt6 COMPONENTS DBus REQUIRED)
endif()

set(LITH_SOURCES
    src/clipboardproxy.cpp src/clipboardproxy.h
    src/common.h
    src/datamodel.cpp src/datamodel.h
    src/lith.cpp src/lith.h
    src/main.cpp
    src/protocol.cpp src/protocol.h
    src/qmlobjectlist.cpp src/qmlobjectlist.h
    src/settings.cpp src/settings.h
    src/uploader.cpp src/uploader.h
    src/util/colortheme.cpp src/util/colortheme.h
    src/util/formattedstring.cpp src/util/formattedstring.h
    src/util/messagelistfilter.cpp src/util/messagelistfilter.h
    src/util/nicklistfilter.cpp src/util/nicklistfilter.h
    src/util/sockethelper.cpp src/util/sockethelper.h
    src/weechat.cpp src/weechat.h
    src/windowhelper.cpp src/windowhelper.h
)

set(LITH_RESOURCES
    ui/ui.qrc
    assets/assets.qrc
)


if(IOS)
    set(LITH_SOURCES
        ${LITH_SOURCES}
        src/iosclipboard.h src/iosclipboard.mm
    )
endif()
if (WIN32)
    enable_language("RC")
    set(LITH_SOURCES
        ${LITH_SOURCES}
        dist/win/lith.rc
    )
endif()

qt_add_executable(Lith
    MACOSX_BUNDLE
    ${LITH_SOURCES}
    ${LITH_RESOURCES}
)

target_compile_definitions(Lith PRIVATE
    IMGUR_API_KEY=\"\"
)
target_include_directories(Lith PRIVATE
    src
)
target_link_libraries(Lith PRIVATE
    Qt::Gui
    Qt::GuiPrivate
    Qt::Multimedia
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::WebSockets
    Qt::Widgets
    Qt::Xml
)

if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_link_libraries(Lith PRIVATE
        Qt::DBus
    )
endif()

if(ANDROID)
    if(NOT ANDROID_OPENSSL_SRCDIR)
        message(FATAL_ERROR "ANDROID_OPENSSL_SRCDIR not specified")
    endif()

    include("${ANDROID_OPENSSL_SRCDIR}/CMakeLists.txt")
    set_target_properties(Lith PROPERTIES QT_ANDROID_EXTRA_LIBS "${ANDROID_EXTRA_LIBS}")
endif()

if(IOS)
    target_link_libraries(Lith PRIVATE
        FWFoundation
        FWUIKit
        -F/System/Library/Frameworks
    )
endif()

set_target_properties(Lith PROPERTIES
    BUNDLE True
    MACOSX_BUNDLE_GUI_IDENTIFIER app.lith.Lith
    MACOSX_BUNDLE_BUNDLE_NAME Lith
    MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
    #MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/customtemplate.plist.in
)


if(ANDROID)
    install(TARGETS Lith
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib
        BUNDLE DESTINATION lib
    )
else()
    install(TARGETS Lith
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION bin
    )
endif()
if(UNIX AND NOT APPLE)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/app.lith.Lith.appdata.xml DESTINATION share/metainfo)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/app.lith.Lith.desktop DESTINATION share/applications)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/linux/hicolor DESTINATION share/icons)
endif()

#### Keys ignored in scope 8:.:.:Lith.pro:WASM:
# QMAKE_LFLAGS_DEBUG = "-g4"
# QMAKE_WASM_PTHREAD_POOL_SIZE = "3"
