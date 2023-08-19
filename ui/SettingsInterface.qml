// Lith
// Copyright (C) 2020 Martin Bříza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs

import "SettingsFields" as Fields

ScrollView {
    id: root
    clip: true
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    function save() {
        lith.settings.shortenLongUrls = shortenLongUrlsCheckbox.checked
        lith.settings.shortenLongUrlsThreshold = shortenLongUrlsThreshold.text
        lith.settings.useLocalInputBar = useLocalInputBarCheckbox.checked
        lith.settings.showAutocompleteButton = showAutocompleteButtonCheckbox.checked
        lith.settings.showGalleryButton = showGalleryButtonCheckbox.checked
        lith.settings.showSendButton = showSendButtonCheckbox.checked
        lith.settings.showSearchButton = showSearchButtonCheckbox.checked
        lith.settings.baseFontSize = baseFontSizeSpinBox.value
        lith.settings.nickCutoffThreshold = nickCutoffThresholdSpinBox.value
        lith.settings.timestampFormat = timestampFormatInput.text
        lith.settings.showDateHeaders = showDateHeadersCheckbox.checked
        lith.settings.muteVideosByDefault = muteVideosByDefaultCheckbox.checked
        lith.settings.loopVideosByDefault = loopVideosByDefaultCheckbox.checked
        lith.settings.showImageThumbnails = showImageThumbnailsCheckbox.checked
        lith.settings.openLinksDirectly = openLinksDirectlyCheckbox.checked
        lith.settings.openLinksDirectlyInBrowser = openLinksDirectlyInBrowserSwitch.checked
        lith.settings.scrollbarsOverlayContents = scrollbarsOverlayContentsCheckbox.checked
        lith.settings.forceLightTheme = forceLightThemeCheckbox.checked
        lith.settings.forceDarkTheme = forceDarkThemeCheckbox.checked
        lith.settings.useTrueBlackWithDarkTheme = useTrueBlackWithDarkThemeCheckbox.checked
        lith.settings.hotlistEnabled = hotlistEnabledCheckbox.checked
        lith.settings.hotlistCompact = hotlistCompactCheckbox.checked
        lith.settings.messageSpacing = messageSpacingSpinbox.value
        lith.settings.showJoinPartQuitMessages = showJoinPartQuitMessagesCheckbox.checked
        lith.settings.baseFontFamily = fontDialog.currentFont.family
        lith.settings.showBufferListOnStartup = showBufferListOnStartupCheckbox.checked
        lith.settings.platformBufferControlPosition = platformBufferControlPositionCheckbox.checked
        lith.settings.enableNotifications = enableNotificationsCheckbox.checked
    }
    function restore() {
        shortenLongUrlsCheckbox.checked = lith.settings.shortenLongUrls
        shortenLongUrlsThreshold.text = lith.settings.shortenLongUrlsThreshold
        useLocalInputBarCheckbox.checked = lith.settings.useLocalInputBar
        showAutocompleteButtonCheckbox.checked = lith.settings.showAutocompleteButton
        showGalleryButtonCheckbox.checked = lith.settings.showGalleryButton
        showSendButtonCheckbox.checked = lith.settings.showSendButton
        showSearchButtonCheckbox.checked = lith.settings.showSearchButton
        baseFontSizeSpinBox.value = lith.settings.baseFontSize
        nickCutoffThresholdSpinBox.value = lith.settings.nickCutoffThreshold
        timestampFormatInput.text = lith.settings.timestampFormat
        showDateHeadersCheckbox.checked = lith.settings.showDateHeaders
        muteVideosByDefaultCheckbox.checked = lith.settings.muteVideosByDefault
        loopVideosByDefaultCheckbox.checked = lith.settings.loopVideosByDefault
        showImageThumbnailsCheckbox.checked = lith.settings.showImageThumbnails
        openLinksDirectlyCheckbox.checked = lith.settings.openLinksDirectly
        openLinksDirectlyInBrowserSwitch.checked = lith.settings.openLinksDirectlyInBrowser
        scrollbarsOverlayContentsCheckbox.checked = lith.settings.scrollbarsOverlayContents
        forceLightThemeCheckbox.checked = lith.settings.forceLightTheme
        forceDarkThemeCheckbox.checked = lith.settings.forceDarkTheme
        useTrueBlackWithDarkThemeCheckbox.checked = lith.settings.useTrueBlackWithDarkTheme
        hotlistEnabledCheckbox.checked = lith.settings.hotlistEnabled
        hotlistCompactCheckbox.checked = lith.settings.hotlistCompact
        messageSpacingSpinbox.value = lith.settings.messageSpacing
        showJoinPartQuitMessagesCheckbox.checked = lith.settings.showJoinPartQuitMessages
        try {
            fontChangeButton.text = lith.settings.baseFontFamily
            fontChangeButton.font.family = lith.settings.baseFontFamily
        }
        catch(e) {

        }
        fontDialog.currentFont.family = lith.settings.baseFontFamily
        showBufferListOnStartupCheckbox.checked = lith.settings.showBufferListOnStartup
        platformBufferControlPositionCheckbox.checked = lith.settings.platformBufferControlPosition
        enableNotificationsCheckbox.checked = lith.settings.enableNotifications
    }

    FontDialog {
        id: fontDialog
        currentFont.family: lith.settings.baseFontFamily
        flags: FontDialog.MonospacedFonts
        onAccepted: {
            fontChangeButton.text = fontDialog.selectedFont.family
            fontChangeButton.font.family = fontDialog.selectedFont.family
        }
    }

    Item {
        width: parent.width
        implicitHeight: settingsPaneLayout.implicitHeight + 12

        ColumnLayout {
            id: settingsPaneLayout
            anchors.horizontalCenter: parent.horizontalCenter
            width: window.landscapeMode ? Math.min(Math.min(420, 1.33 * implicitWidth), parent.width) : parent.width
            spacing: -1

            Fields.Header {
                text: qsTr("Message settings")
            }

            Fields.Button {
                id: fontChangeButton
                summary: "Font family"
                details: "(requires restart)"
                visible: !window.platform.mobile
                text: lith.settings.baseFontFamily
                onClicked: fontDialog.open()
            }

            Fields.IntSpinBox {
                id: baseFontSizeSpinBox
                Layout.alignment: Qt.AlignLeft
                summary: qsTr("Message font size")
                value: lith.settings.baseFontSize
                from: 6
                to: 32
            }

            Fields.String {
                id: timestampFormatInput
                Layout.alignment: Qt.AlignLeft
                summary: qsTr("Timestamp format")
                text: lith.settings.timestampFormat
            }

            Fields.Boolean {
                id: showDateHeadersCheckbox
                Layout.alignment: Qt.AlignLeft
                summary: qsTr("Show date headers in the buffer")
                checked: lith.settings.showDateHeaders
            }

            Fields.Boolean {
                id: showJoinPartQuitMessagesCheckbox
                checked: lith.settings.showJoinPartQuitMessages
                summary: qsTr("Show join/part messages")
            }

            Fields.IntSpinBox {
                id: nickCutoffThresholdSpinBox
                summary: qsTr("Align nick length")
                from: -1
                to: 100
                value: lith.settings.nickCutoffThreshold
                textFromValue: function(value, locale) {
                    if (value >= 0)
                        return Number(value)
                    return qsTr("Disabled")
                }
                valueFromText: function(value, locale) {
                    if (value === qsTr("Disabled"))
                        return -1
                    return Number(value)
                }
            }

            Fields.IntSpinBox {
                id: messageSpacingSpinbox
                summary: qsTr("Message spacing")
                value: lith.settings.messageSpacing
                from: -to
            }

            ////////////////////////// GENERAL
            Fields.Header {
                text: qsTr("General")
            }

            Fields.Boolean {
                id: scrollbarsOverlayContentsCheckbox
                summary: qsTr("Mobile-like scrollbar")
                details: checked ? qsTr("Scrollbar will be thin and appear above contents") : qsTr("Scrollbar will be wide and always visible next to contents")
                checked: lith.settings.scrollbarsOverlayContents
            }

            ////////////////////////// COLOR THEME
            Fields.Header {
                text: qsTr("Color theme")
            }

            Fields.Boolean {
                id: forceLightThemeCheckbox
                summary: qsTr("Force light theme")
                checked: lith.settings.forceLightTheme
                onCheckedChanged: {
                    if (checked)
                        forceDarkThemeCheckbox.checked = false
                }
            }

            Fields.Boolean {
                id: forceDarkThemeCheckbox
                summary: qsTr("Force dark theme")
                checked: lith.settings.forceDarkTheme
                onCheckedChanged: {
                    if (checked)
                        forceLightThemeCheckbox.checked = false
                }
            }

            Fields.Boolean {
                id: useTrueBlackWithDarkThemeCheckbox
                summary: qsTr("Use black in dark theme")
                checked: lith.settings.useTrueBlackWithDarkTheme
            }

            ////////////////////////// INPUT BAR
            Fields.Header {
                text: qsTr("Input bar")
            }

            Fields.Boolean {
                id: useLocalInputBarCheckbox
                summary: qsTr("Use local input bar")
                details: checked ? qsTr("Last input will be remembered in each buffer") : qsTr("Input contents won't change when switching buffers")
                checked: lith.settings.useLocalInputBar
            }

            Fields.Boolean {
                id: showAutocompleteButtonCheckbox
                summary: qsTr("Show autocomplete button")
                checked: lith.settings.showAutocompleteButton
            }

            Fields.Boolean {
                id: showGalleryButtonCheckbox
                summary: qsTr("Show gallery button")
                checked: lith.settings.showGalleryButton
            }

            Fields.Boolean {
                id: showSendButtonCheckbox
                summary: qsTr("Show send button")
                checked: lith.settings.showSendButton
            }

            Fields.Boolean {
                id: showSearchButtonCheckbox
                summary: qsTr("Show search button")
                checked: lith.settings.showSearchButton
            }

            ////////////////////////// HOTLIST
            Fields.Header {
                text: qsTr("Hotlist")
            }

            Fields.Boolean {
                id: hotlistEnabledCheckbox
                summary: qsTr("Enable hotlist")
                checked: lith.settings.hotlistEnabled
            }

            Fields.Button {
                id: hotlistEditorButton
                summary: qsTr("Hotlist format")
                text: qsTr("Edit")
                onClicked: hotlistEditor.open()


                SettingsHotlistFormatEditor {
                    id: hotlistEditor
                }
            }

            Fields.Boolean {
                id: hotlistCompactCheckbox
                summary: qsTr("Use compact layout")
                enabled: hotlistEnabledCheckbox.checked
                checked: lith.settings.hotlistCompact
            }

            ////////////////////////// BUFFER LIST
            Fields.Header {
                text: qsTr("Buffer list")
            }

            Fields.Boolean {
                id: showBufferListOnStartupCheckbox
                summary: qsTr("Show buffer list on startup")
                checked: lith.settings.showBufferListOnStartup
            }

            Fields.Boolean {
                id: platformBufferControlPositionCheckbox
                summary: window.platform.mobile ? qsTr("Render buffer search on the bottom") : qsTr("Render buffer search on the top")
                checked: lith.settings.platformBufferControlPosition
            }

            ////////////////////////// URL HANDLING
            Fields.Header {
                text: qsTr("URL handling")
            }

            Fields.Boolean {
                id: shortenLongUrlsCheckbox
                summary: qsTr("Shortening Enabled")
                checked: lith.settings.shortenLongUrls
            }

            Fields.String {
                id: shortenLongUrlsThreshold
                summary: qsTr("Length threshold")
                enabled: shortenLongUrlsCheckbox.checked
                text: lith.settings.shortenLongUrlsThreshold
                inputMethodHints: Qt.ImhPreferNumbers
                validator: IntValidator {
                    bottom: 0
                }
            }

            ////////////////////////// MULTIMEDIA
            Fields.Header {
                text: qsTr("Multimedia")
            }

            Fields.Boolean {
                id: showImageThumbnailsCheckbox
                summary: qsTr("Show image thumbnails")
                checked: lith.settings.showImageThumbnails
            }

            Fields.Boolean {
                id: openLinksDirectlyCheckbox
                summary: qsTr("Open links directly")
                details: checked ? qsTr("Lith will either show media (images, video) directly with a built-in player or open your default browser (unsupported media, other link types)") : qsTr("Lith will show a popup allowing you to choose between copying the URL, opening it in your default browser or showing it inside Lith if supported (images, video)")
                checked: lith.settings.openLinksDirectly
                columnComponent: RowLayout {
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    Layout.bottomMargin: 6
                    Layout.topMargin: 6
                    Layout.fillWidth: true
                    enabled: openLinksDirectlyCheckbox.checked
                    // Layout balancing wrapper
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumHeight: insideLithLabel.implicitHeight
                        Label {
                            id: insideLithLabel
                            width: parent.width
                            verticalAlignment: Label.AlignVCenter
                            horizontalAlignment: Label.AlignRight
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                            maximumLineCount: 2
                            elide: Label.ElideRight
                            text: qsTr("Inside Lith (if possible)")
                            color: openLinksDirectlyInBrowserSwitch.checked ? disabledPalette.text : palette.text
                            MouseArea {
                                id: insideLithMouse
                                anchors.fill: parent
                                hoverEnabled: true

                                ToolTip.text: insideLithLabel.text
                                ToolTip.visible: window.platform.mobile ? insideLithMouse.containsPress : insideLithMouse.containsMouse
                                ToolTip.delay: window.platform.mobile ? 0 : 800
                            }
                        }
                    }
                    Switch {
                        id: openLinksDirectlyInBrowserSwitch
                        checked: lith.settings.openLinksDirectlyInBrowser
                        onColor: offColor
                    }
                    // Layout balancing wrapper
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumHeight: inBrowserLabel.implicitHeight
                        Label {
                            id: inBrowserLabel
                            anchors.fill: parent
                            verticalAlignment: Label.AlignVCenter
                            elide: Label.ElideRight
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                            maximumLineCount: 2
                            text: qsTr("In a web browser")
                            color: openLinksDirectlyInBrowserSwitch.checked ? palette.text : disabledPalette.text
                            MouseArea {
                                id: inBrowserMouse
                                anchors.fill: parent
                                hoverEnabled: true

                                ToolTip.text: inBrowserLabel.text
                                ToolTip.visible: window.platform.mobile ? inBrowserMouse.containsPress : inBrowserMouse.containsMouse
                                ToolTip.delay: window.platform.mobile ? 0 : 800
                            }
                        }
                    }
                }
            }

            Fields.Boolean {
                id: loopVideosByDefaultCheckbox
                summary: qsTr("Loop videos by default")
                checked: lith.settings.showSendButton
            }

            Fields.Boolean {
                id: muteVideosByDefaultCheckbox
                summary: qsTr("Mute videos by default")
                checked: lith.settings.showSendButton
            }

            Fields.Header {
                text: qsTr("Notification settings")
            }

            Fields.Boolean {
                id: enableNotificationsCheckbox
                summary: qsTr("Enable notifications")
                details: platform.ios ? qsTr("This setting won't do anything for now, we need to implement a server and WeeChat plugin to actually pass notifications to Lith.") : ""
                checked: lith.settings.enableNotifications
            }

            Fields.Base {
                id: deviceTokenField
                summary: "Device token"
                visible: platform.ios

                rowComponent: Label {
                    Layout.preferredWidth: settingsPaneLayout.width / 2
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    text: lith.notificationHandler.deviceToken
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            clipboardProxy.setText(lith.notificationHandler.deviceToken)
                            deviceTokenField.details = qsTr("Copied to clipboard")
                        }
                    }
                }
                onDetailsChanged: deviceTokenCopiedTimer.start()
                Timer {
                    id: deviceTokenCopiedTimer
                    interval: 5000
                    onTriggered: deviceTokenField.details = ""
                }
            }
        }
    }
}
