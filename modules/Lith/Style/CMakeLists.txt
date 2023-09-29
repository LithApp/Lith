set(LITHSTYLE_QML
    ApplicationWindow.qml
    BusyIndicator.qml
    Button.qml
    CheckBox.qml
    Dialog.qml
    ItemDelegate.qml
    Label.qml
    LithPalette.qml
    Popup.qml
    ScrollBar.qml
    SpinBox.qml
    Switch.qml
    TabButton.qml
    TextArea.qml
    TextField.qml
)

set_source_files_properties(LithPalette.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

set(LITHSTYLE_SOURCES
    constants.cpp constants.h
    lithstyle.h lithstyle.cpp
    liththeme.h liththeme.cpp
    lith_styleplugin.cpp
)

qt_add_qml_module(LithStyle
    URI Lith.Style
    VERSION 1.0
    DEPENDENCIES
        QtQuick/auto
    CLASS_NAME Lith_StylePlugin
    PLUGIN_TARGET LithStyle
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    SOURCES ${LITHSTYLE_SOURCES}
    QML_FILES ${LITHSTYLE_QML}
)
qt_import_qml_plugins(LithStyle)

target_link_libraries(LithStyle PRIVATE
    LithCore
    QCoro6::Coro
    Qt6::Gui
    Qt6::Quick
    Qt::QuickControls2 Qt::QuickControls2Private
    Qt::QuickLayouts
    Qt::QuickDialogs2
    Qt::QuickTemplates2 Qt::QuickTemplates2Private
)

target_include_directories(LithStyle PRIVATE
    ${qtkeychain_SOURCE_DIR}
    ${qtkeychain_BINARY_DIR}
    ${qcoro_SOURCE_DIR}
    ${qcoro_SOURCE_DIR}/qcoro/core
    ${qcoro_BINARY_DIR}/qcoro/core
)

install(TARGETS LithStyle
    LIBRARY DESTINATION ${LITH_INSTALL_MODULEDIR}/Lith/Style/
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qmldir
    ${CMAKE_CURRENT_BINARY_DIR}/LithStyle.qmltypes
    DESTINATION ${LITH_INSTALL_MODULEDIR}/Lith/Style/
)
