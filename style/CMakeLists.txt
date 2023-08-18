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

qt_add_qml_module(LithStyle
    STATIC
    URI LithStyle
    VERSION 1.0
    QML_FILES ${LITHSTYLE_QML}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/imports/LithStyle
)

set(QML_IMPORT_PATH ${QML_IMPORT_PATH} ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "" FORCE)

install(TARGETS LithStyle
    BUNDLE DESTINATION ..
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
