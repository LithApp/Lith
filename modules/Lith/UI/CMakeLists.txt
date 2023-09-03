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
    util/DebugWindow.qml
    util/PaletteSwitch.qml
)

qt_add_library(LithUI STATIC)
qt_add_qml_module(LithUI
    URI Lith.UI
    VERSION 1.0
    QML_FILES ${LITH_QML}
    IMPORTS QtMultimedia
)
qt_import_qml_plugins(LithUI)

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    # This is a workaround, Qt doesn't include this plugin automatically for us
    qt_import_plugins(LithUI INCLUDE Qt::QIosOptionalPlugin_NSPhotoLibraryPlugin )
endif()

target_link_libraries(LithUI PRIVATE
    LithCore
    Qt::Qml
    Qt::Quick
    Qt::Multimedia
    Qt::QuickControls2
    Qt::QuickLayouts
    Qt::QuickDialogs2
    Qt::QuickTemplates2
)

install(TARGETS LithUI
    BUNDLE DESTINATION ..
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
