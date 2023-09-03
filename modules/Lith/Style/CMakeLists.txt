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

qt_add_library(LithStyle STATIC
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
    BUNDLE DESTINATION ..
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
