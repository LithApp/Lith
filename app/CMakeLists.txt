set(LITH_SOURCES
    main.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    enable_language("RC")
    set(LITH_SOURCES
        ${LITH_SOURCES}
        ../dist/win/lith.rc
    )
endif()

qt_add_executable(Lith
    MACOSX_BUNDLE MANUAL_FINALIZATION
    ${LITH_SOURCES}
)
set_target_properties(Lith PROPERTIES
    QT_QML_IMPORT_PATH "${CMAKE_BINARY_DIR}/modules"
    QT_QML_ROOT_PATH "${CMAKE_SOURCE_DIR}"
    QT_ANDROID_EXTRA_LIBS "${ANDROID_OPENSSL_PATH}/${ANDROID_ABI}/libssl_3.so;${ANDROID_OPENSSL_PATH}/${ANDROID_ABI}/libcrypto_3.so"
)
qt_finalize_target(Lith)
qt_add_qml_module(Lith
    URI App # Seems to have to to be like this in this case, otherwise there are build directory name clashes when linking the binary
    QML_FILES main.qml
)
qt_import_qml_plugins(Lith)
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    qt_android_generate_deployment_settings(LithStyleplugin)
    qt_android_generate_deployment_settings(Lith)
    qt_android_add_apk_target(Lith)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    target_link_libraries(Lith PRIVATE
        Qt6::QWasmIntegrationPlugin
    )
endif()

target_link_libraries(Lith PRIVATE
    LithAssets
    LithCore
    LithUI
    LithStyle
    QCoro6::Coro
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

target_include_directories(Lith PRIVATE
    ${qtkeychain_SOURCE_DIR}
    ${qtkeychain_BINARY_DIR}
    ${qcoro_SOURCE_DIR}
    ${qcoro_SOURCE_DIR}/qcoro/core
    ${qcoro_BINARY_DIR}/qcoro/core
)

get_target_property(PLUGIN_TARGET_TYPE LithCore TYPE)
if (PLUGIN_TARGET_TYPE STREQUAL STATIC_LIBRARY)
    target_link_libraries(Lith PRIVATE
        LithCoreplugin
        LithUIplugin
        LithStyleplugin
    )
endif()

target_compile_definitions(Lith
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
set_target_properties(Lith PROPERTIES
    MACOSX_BUNDLE True
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER cz.rtinbriza.ma.Lith
    MACOSX_BUNDLE_BUNDLE_NAME Lith
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/dist/ios/Info.plist.in"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK YES
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER cz.rtinbriza.ma.Lith
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
    XCODE_ATTRIBUTE_INFOPLIST_KEY_LSApplicationCategoryType "public.app-category.utilities"
)

if (APPLE)
    # The asset catalog (icon especially) will be shared between macOS and iOS
    target_sources(Lith PRIVATE "${CMAKE_SOURCE_DIR}/assets/Assets.xcassets")
    set_source_files_properties(
        "${CMAKE_CURRENT_SOURCE_DIR}/../assets/Assets.xcassets"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(Lith PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_SOURCE_DIR}/dist/macos/sandbox.entitlements"
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    find_library(FWFoundation Foundation)
    find_library(FWUIKit UIKit)

    set_target_properties(Lith PROPERTIES
        QT_IOS_LAUNCH_SCREEN "${CMAKE_SOURCE_DIR}/assets/Launch.storyboard"
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_SOURCE_DIR}/dist/ios/notifications.entitlements"
    )

    target_link_libraries(Lith PRIVATE
        "-framework Foundation"
        "-framework UIKit"
        "-framework usernotifications"
        -F/System/Library/Frameworks
    )

    # This is a workaround, Qt doesn't include this plugin automatically for us
    # It needs to be referenced against the app, not the module that actually uses it
    qt_import_plugins(Lith INCLUDE Qt::QIosOptionalPlugin_NSPhotoLibraryPlugin )
endif()

install(TARGETS Lith
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${LITH_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
