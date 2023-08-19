
set(LITH_SOURCES
    clipboardproxy.cpp clipboardproxy.h
    ${CMAKE_CURRENT_BINARY_DIR}/defs/cmakedefs.h
    common.h
    datamodel.cpp
    datamodel.h
    lith.cpp
    lith.h
    logger.cpp
    logger.h
    main.cpp
    notificationhandler.h
    notificationhandler.cpp
    protocol.cpp
    protocol.h
    qmlobjectlist.cpp
    qmlobjectlist.h
    replayproxy.cpp
    replayproxy.h
    search.cpp
    search.h
    settings.cpp
    settings.h
    uploader.cpp
    uploader.h
    util/colortheme.cpp
    util/colortheme.h
    util/formatstringsplitter.cpp
    util/formatstringsplitter.h
    util/formattedstring.cpp
    util/formattedstring.h
    util/messagelistfilter.cpp
    util/messagelistfilter.h
    util/nicklistfilter.cpp
    util/nicklistfilter.h
    util/reflection.cpp
    util/reflection.h
    util/sockethelper.cpp
    util/sockethelper.h
    weechat.cpp
    weechat.h
    windowhelper.cpp
    windowhelper.h
)

configure_file(cmakedefs.h.in ${CMAKE_CURRENT_BINARY_DIR}/defs/cmakedefs.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/defs .)

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(LITH_SOURCES
        ${LITH_SOURCES}
        iosclipboard.h iosclipboard.mm
        iosnotifications.h iosnotifications.mm
    )
endif()
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    enable_language("RC")
    set(LITH_SOURCES
        ${LITH_SOURCES}
        ../dist/win/lith.rc
    )
endif()

qt_add_executable(Lith
    MACOSX_BUNDLE
    ${LITH_SOURCES}
)

target_compile_definitions(Lith PRIVATE
    IMGUR_API_KEY=\"${IMGUR_API_KEY}\"
)
target_include_directories(Lith PRIVATE
    src
)

target_link_libraries(Lith PRIVATE
    LithAssets
    LithStyleplugin
    LithUIplugin
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
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../dist/ios/Info.plist.in"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK YES
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER cz.rtinbriza.ma.Lith
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
    XCODE_ATTRIBUTE_INFOPLIST_KEY_LSApplicationCategoryType "public.app-category.utilities"
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(Lith PRIVATE
        Qt::DBus
    )
endif()

if (APPLE)
    # The asset catalog (icon especially) will be shared between macOS and iOS
    target_sources(Lith PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../assets/Assets.xcassets")
    set_source_files_properties(
        "${CMAKE_CURRENT_SOURCE_DIR}/../assets/Assets.xcassets"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(Lith PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/../dist/macos/sandbox.entitlements"
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    find_library(FWFoundation Foundation)
    find_library(FWUIKit UIKit)

    set_target_properties(Lith PROPERTIES
        QT_IOS_LAUNCH_SCREEN "${CMAKE_CURRENT_SOURCE_DIR}/../assets/Launch.storyboard"
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/../dist/ios/notifications.entitlements"
    )

    target_link_libraries(Lith PRIVATE
        "-framework Foundation"
        "-framework UIKit"
        "-framework usernotifications"
        -F/System/Library/Frameworks
    )

    # This is a workaround, Qt doesn't include this plugin automatically for us
    qt_import_plugins(Lith INCLUDE Qt::QIosOptionalPlugin_NSPhotoLibraryPlugin)
endif()

install(TARGETS Lith
    BUNDLE DESTINATION ..
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
