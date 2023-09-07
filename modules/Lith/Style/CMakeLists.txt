set(LITHSTYLE_QML
    ApplicationWindow.qml
    BusyIndicator.qml
    Button.qml
    CheckBox.qml
    Dialog.qml
    ItemDelegate.qml
    Label.qml
    Popup.qml
    ScrollBar.qml
    SpinBox.qml
    Switch.qml
    TabButton.qml
    TextArea.qml
    TextField.qml
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

target_link_libraries(LithStyle PRIVATE
    LithCore
    Qt6::Gui
    Qt6::Quick
    Qt::QuickControls2
    Qt::QuickLayouts
    Qt::QuickDialogs2
    Qt::QuickTemplates2
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
