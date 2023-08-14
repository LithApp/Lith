set(LITH_QML
    BufferList.qml
    ChannelHeader.qml
    ChannelInputBar.qml
    ChannelMessage.qml
    ChannelMessageActionMenu.qml
    ChannelMessageList.qml
    ChannelTextInput.qml
    ChannelView.qml
    DataBrowser.qml
    DialogButtons.qml
    DropHandler.qml
    DynamicDrawer.qml
    ErrorMessage.qml
    HotList.qml
    HotListItem.qml
    MainView.qml
    NickList.qml
    NickListActionMenu.qml
    PreviewPopup.qml
    ScrollHelper.qml
    SearchBar.qml
    SettingsDialog.qml
    SettingsDialogContents.qml
    SettingsFields/Base.qml
    SettingsFields/Boolean.qml
    SettingsFields/Button.qml
    SettingsFields/Header.qml
    SettingsFields/IntSpinBox.qml
    SettingsFields/Integer.qml
    SettingsFields/MultiLineString.qml
    SettingsFields/String.qml
    SettingsHotlistFormatEditor.qml
    SettingsInterface.qml
    SettingsNetwork.qml
    SettingsShortcuts.qml
    main.qml
    util/DebugWindow.qml
    util/PaletteSwitch.qml
)

qt_add_qml_module(LithUI
    STATIC
    URI LithUI
    VERSION 1.0
    QML_FILES ${LITH_QML}
    OUTPUT_DIRECTORY LithUI
)

set(QML_IMPORT_PATH ${QML_IMPORT_PATH} . ${CMAKE_BINARY_DIR}/LithUI CACHE STRING "" FORCE)

install(TARGETS LithUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
