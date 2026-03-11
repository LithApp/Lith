set(LITH_QML
    BufferList.qml
    ChannelHeader.qml
    ChannelInputBar.qml
    ChannelMessage.qml
    ChannelMessageActionMenu.qml
    ChannelMessageList.qml
    ChannelMessageThumbnail.qml
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
    SettingsFields/RadioList.qml
    SettingsFields/String.qml
    SettingsFields/Slider.qml
    SettingsHotlistFormatEditor.qml
    SettingsInterface.qml
    SettingsNetwork.qml
    SettingsShortcuts.qml
    StatusBar.qml
    util/DebugWindow.qml
    util/PaletteSwitch.qml
    util/ControlPanel.qml
)

qt_add_qml_module(LithUI
    URI Lith.UI
    VERSION 1.0
    DISCARD_QML_CONTENTS
    QML_FILES ${LITH_QML}
    IMPORTS QtMultimedia
)

set_target_properties(LithUI PROPERTIES
    QT_QMLCACHEGEN_ARGUMENTS "--only-bytecode"
)

target_link_libraries(LithUI PRIVATE
    LithCore
    LithStyle
    LithAssets
    Qt::Qml
    Qt::Quick
    Qt::Multimedia
    Qt::QuickControls2
    Qt::QuickLayouts
    Qt::QuickDialogs2
    Qt::QuickTemplates2
)
get_target_property(LITH_UI_PLUGIN_TARGET_TYPE LithUI TYPE)
if (LITH_UI_PLUGIN_TARGET_TYPE STREQUAL STATIC_LIBRARY)
     qt_import_qml_plugins(LithUI)
endif()

install(TARGETS LithUI
    LIBRARY DESTINATION ${LITH_INSTALL_LIBDIR}
)
install(TARGETS LithUIplugin
    LIBRARY DESTINATION ${LITH_INSTALL_MODULEDIR}/Lith/UI/
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qmldir
    ${CMAKE_CURRENT_BINARY_DIR}/LithUI.qmltypes
    DESTINATION ${LITH_INSTALL_MODULEDIR}/Lith/UI/
)
