project(Lith)
cmake_minimum_required(VERSION 3.20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(VERSION "0.0.0" CACHE STRING "This is where we start working with the version string as passed from the build script")

if (VERSION)
    set(PROJECT_VERSION ${VERSION})
else()
    message(WARNING "Version passed to CMake was empty, this may have been done on accident")
    set(PROJECT_VERSION "0.0.0")
endif()

string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)


find_package(Qt6 COMPONENTS Widgets Gui Quick Multimedia Qml QuickControls2 QuickLayouts QuickDialogs2 QuickTemplates2 WebSockets Xml REQUIRED)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Qt6 COMPONENTS DBus REQUIRED)
endif()

qt_standard_project_setup(REQUIRES 6.5)

set(LITH_SOURCES
    src/clipboardproxy.cpp src/clipboardproxy.h
    src/common.h
    src/datamodel.cpp
    src/datamodel.h
    src/lith.cpp
    src/lith.h
    src/logger.cpp
    src/logger.h
    src/main.cpp
    src/protocol.cpp
    src/protocol.h
    src/qmlobjectlist.cpp
    src/qmlobjectlist.h
    src/replayproxy.cpp
    src/replayproxy.h
    src/search.cpp
    src/search.h
    src/settings.cpp
    src/settings.h
    src/uploader.cpp
    src/uploader.h
    src/util
    src/util/colortheme.cpp
    src/util/colortheme.h
    src/util/formatstringsplitter.cpp
    src/util/formatstringsplitter.h
    src/util/formattedstring.cpp
    src/util/formattedstring.h
    src/util/messagelistfilter.cpp
    src/util/messagelistfilter.h
    src/util/nicklistfilter.cpp
    src/util/nicklistfilter.h
    src/util/reflection.cpp
    src/util/reflection.h
    src/util/sockethelper.cpp
    src/util/sockethelper.h
    src/weechat.cpp
    src/weechat.h
    src/windowhelper.cpp
    src/windowhelper.h
)

set(LITH_RESOURCES
    assets/assets.qrc
)

set(LITH_QML
    ui/BufferList.qml
    ui/ChannelHeader.qml
    ui/ChannelInputBar.qml
    ui/ChannelMessage.qml
    ui/ChannelMessageActionMenu.qml
    ui/ChannelMessageList.qml
    ui/ChannelTextInput.qml
    ui/ChannelView.qml
    ui/DataBrowser.qml
    ui/DebugDialog.qml
    ui/DialogButtons.qml
    ui/DropHandler.qml
    ui/DynamicDrawer.qml
    ui/ErrorMessage.qml
    ui/HotList.qml
    ui/HotListItem.qml
    ui/MainView.qml
    ui/NickList.qml
    ui/NickListActionMenu.qml
    ui/PreviewPopup.qml
    ui/ScrollHelper.qml
    ui/SearchBar.qml
    ui/SettingsDialog.qml
    ui/SettingsFields/Base.qml
    ui/SettingsFields/Boolean.qml
    ui/SettingsFields/Button.qml
    ui/SettingsFields/Header.qml
    ui/SettingsFields/IntSpinBox.qml
    ui/SettingsFields/Integer.qml
    ui/SettingsFields/MultiLineString.qml
    ui/SettingsFields/String.qml
    ui/SettingsHotlistFormatEditor.qml
    ui/SettingsInterface.qml
    ui/SettingsNetwork.qml
    ui/SettingsShortcuts.qml
    ui/main.qml
    ui/util/PaletteSwitch.qml
)

set(LITHSTYLE_QML
    ui/LithStyle/ApplicationWindow.qml
    ui/LithStyle/BusyIndicator.qml
    ui/LithStyle/Button.qml
    ui/LithStyle/CheckBox.qml
    ui/LithStyle/Dialog.qml
    ui/LithStyle/ItemDelegate.qml
    ui/LithStyle/Label.qml
    ui/LithStyle/Popup.qml
    ui/LithStyle/SpinBox.qml
    ui/LithStyle/Switch.qml
    ui/LithStyle/TabButton.qml
    ui/LithStyle/TextArea.qml
    ui/LithStyle/TextField.qml
)
set(QML_IMPORT_PATH ${QML_IMPORT_PATH} ${CMAKE_SOURCE_DIR}/ui/LithUI ${CMAKE_SOURCE_DIR}/ui/LithStyle CACHE STRING "" FORCE)

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(LITH_SOURCES
        ${LITH_SOURCES}
        src/iosclipboard.h src/iosclipboard.mm
    )
    set(LITH_RESOURCES
        ${LITH_RESOURCES}
    )
endif()
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
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

qt_add_qml_module(LithUI
    URI LithUI
    VERSION 1.0
    QML_FILES ${LITH_QML}
    OUTPUT_DIRECTORY LithUI
    NO_PLUGIN_OPTIONAL
)
qt_add_qml_module(LithStyle
    URI LithStyle
    VERSION 1.0
    QML_FILES ${LITHSTYLE_QML}
    OUTPUT_DIRECTORY LithStyle
    NO_PLUGIN_OPTIONAL
)

target_compile_definitions(Lith PRIVATE
    IMGUR_API_KEY=\"${IMGUR_API_KEY}\"
)
target_include_directories(Lith PRIVATE
    src
)

target_link_libraries(Lith PRIVATE
    LithStyle
    LithUI
    Qt::Gui
    Qt::GuiPrivate
    Qt::Multimedia
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::QuickLayouts
    Qt::QuickDialogs2
    Qt::QuickTemplates2
    Qt::WebSockets
    Qt::Widgets
    Qt::Xml
)

target_compile_definitions(Lith
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
set_target_properties(Lith PROPERTIES
    MACOSX_BUNDLE True
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER cz.rtinbriza.ma.Lith
    MACOSX_BUNDLE_BUNDLE_NAME Lith
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/dist/ios/Info.plist.in"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK YES
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER cz.rtinbriza.ma.Lith
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(Lith PRIVATE
        Qt::DBus
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    find_library(FWFoundation Foundation)
    find_library(FWUIKit UIKit)

    set_target_properties(Lith PROPERTIES
        QT_IOS_LAUNCH_SCREEN "${CMAKE_CURRENT_SOURCE_DIR}/assets/Launch.storyboard")

    target_sources(Lith PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/assets/Assets.xcassets")
    set_source_files_properties(
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/Assets.xcassets"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    target_link_libraries(Lith PRIVATE
        "-framework Foundation"
        "-framework UIKit"
        -F/System/Library/Frameworks
    )

    # This is a workaround, Qt doesn't include this plugin automatically for us
    qt_import_plugins(Lith INCLUDE Qt::QIosOptionalPlugin_NSPhotoLibraryPlugin)
endif()

install(TARGETS Lith LithUI LithStyle
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/app.lith.Lith.appdata.xml DESTINATION share/metainfo)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/app.lith.Lith.desktop DESTINATION share/applications)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/linux/hicolor DESTINATION share/icons)
endif()