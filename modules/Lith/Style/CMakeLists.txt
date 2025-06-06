set(LITHSTYLE_QML
    util/ButtonBackground.qml
    util/InputBackground.qml

    ApplicationWindow.qml
    BusyIndicator.qml
    Button.qml
    CheckBox.qml
    Dialog.qml
    ItemDelegate.qml
    Label.qml
    LithPalette.qml
    Popup.qml
    RadioButton.qml
    ScrollBar.qml
    Slider.qml
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
)

qt_add_library(LithStyle
    ${LITHSTYLE_SOURCES}
)
qt_add_qml_module(LithStyle
    URI Lith.Style
    VERSION 1.0
    QML_FILES ${LITHSTYLE_QML}
)
qt_import_qml_plugins(LithStyle)

set_target_properties(LithStyle PROPERTIES
    QT_QMLCACHEGEN_ARGUMENTS "--only-bytecode"
)

target_link_libraries(LithStyle PRIVATE
    LithCore
    QCoro6::Coro
    QCoro6::Core
    Qt6::Gui
    Qt6::Quick
    Qt::QuickControls2
    Qt::QuickLayouts
    Qt::QuickDialogs2
    Qt::QuickTemplates2
)

target_include_directories(LithStyle PRIVATE
    ${qtkeychain_SOURCE_DIR}
    ${qtkeychain_BINARY_DIR}
    ${qcoro_SOURCE_DIR}
    ${qcoro_SOURCE_DIR}/qcoro/core
    ${qcoro_BINARY_DIR}/qcoro/core
)

install(TARGETS LithStyle
    LIBRARY DESTINATION ${LITH_INSTALL_LIBDIR}
)
install(TARGETS LithStyleplugin
    LIBRARY DESTINATION ${LITH_INSTALL_MODULEDIR}/Lith/Style/
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qmldir
    ${CMAKE_CURRENT_BINARY_DIR}/LithStyle.qmltypes
    DESTINATION ${LITH_INSTALL_MODULEDIR}/Lith/Style/
)
