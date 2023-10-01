set(LITH_CORE_SOURCES
    clipboardproxy.cpp clipboardproxy.h
    cmakedefs.h.in
    common.h
    datamodel.cpp
    datamodel.h
    lith.cpp
    lith.h
    logger.cpp
    logger.h
    notificationhandler.h
    notificationhandler.cpp
    lithplatform.cpp
    lithplatform.h
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
    colortheme.cpp
    colortheme.h
    colorutils.cpp
    colorutils.h
    formatstringsplitter.cpp
    formatstringsplitter.h
    formattedstring.cpp
    formattedstring.h
    messagelistfilter.cpp
    messagelistfilter.h
    nicklistfilter.cpp
    nicklistfilter.h
    reflection.cpp
    reflection.h
    sockethelper.cpp
    sockethelper.h
    weechat.cpp
    weechat.h
    windowhelper.cpp
    windowhelper.h
)

configure_file(cmakedefs.h.in ${CMAKE_CURRENT_BINARY_DIR}/defs/cmakedefs.h)

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(LITH_CORE_SOURCES
        ${LITH_CORE_SOURCES}
        iosclipboard.h iosclipboard.mm
        iosnotifications.h iosnotifications.mm
    )
endif()

qt_add_library(LithCore
    ${LITH_CORE_SOURCES}
)
qt_add_qml_module(LithCore
    URI Lith.Core
    VERSION 1.0
    QML_FILES
)
qt_import_qml_plugins(LithCore)

generate_export_header(LithCore)

target_link_libraries(LithCore PRIVATE
    LithAssets
    QCoro6::Coro
    QCoro6::Core
    Qt::Gui
    Qt::GuiPrivate
    Qt::Multimedia
    Qt::Qml
    Qt::Quick
    Qt::WebSockets
    Qt::Widgets
    Qt::Xml
)
if (LITH_DEPENDENCY_ENABLE_KEYCHAIN)
    target_link_libraries(LithCore PRIVATE
        qt6keychain
    )
endif()

target_include_directories(LithCore PRIVATE
    ${qtkeychain_SOURCE_DIR}
    ${qtkeychain_BINARY_DIR}
    ${qcoro_SOURCE_DIR}
    ${qcoro_SOURCE_DIR}/qcoro/core
    ${qcoro_BINARY_DIR}/qcoro/core
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(LithCore PRIVATE
        Qt::DBus
    )
endif()

target_include_directories(LithCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(LithCore PROPERTIES
    MACOSX_PACKAGE_LOCATION Contents
)

qt_add_lupdate(LithCore
    TS_FILES ${CMAKE_BINARY_DIR}/translations_tmp/Lith_partial_Core_en.ts
    SOURCES ${LITH_CORE_SOURCES} ${LITH_CORE_NETWORKING_SOURCES}
)
install(TARGETS LithCore
    LIBRARY DESTINATION ${LITH_INSTALL_LIBDIR}
)
install(TARGETS LithCoreplugin
    LIBRARY DESTINATION ${LITH_INSTALL_MODULEDIR}/Lith/Core/
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qmldir
    ${CMAKE_CURRENT_BINARY_DIR}/LithCore.qmltypes
    DESTINATION ${LITH_INSTALL_MODULEDIR}/Lith/Core/
)


