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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import "SettingsFields" as Fields
import Lith.Core
import Lith.Style

ScrollView {
    id: root
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    function save() {
        Lith.settings.shortenLongUrls = shortenLongUrlsCheckbox.checked
        Lith.settings.shortenLongUrlsThreshold = shortenLongUrlsThreshold.text
        Lith.settings.useLocalInputBar = useLocalInputBarCheckbox.checked
        Lith.settings.showAutocompleteButton = showAutocompleteButtonCheckbox.checked
        Lith.settings.showGalleryButton = showGalleryButtonCheckbox.checked
        Lith.settings.showSendButton = showSendButtonCheckbox.checked
        Lith.settings.showSearchButton = showSearchButtonCheckbox.checked
        Lith.settings.baseFontSize = baseFontSizeSpinBox.value
        Lith.settings.nickCutoffThreshold = nickCutoffThresholdSpinBox.value
        Lith.settings.timestampFormat = timestampFormatInput.text
        Lith.settings.showDateHeaders = showDateHeadersCheckbox.checked
        Lith.settings.muteVideosByDefault = muteVideosByDefaultCheckbox.checked
        Lith.settings.loopVideosByDefault = loopVideosByDefaultCheckbox.checked
        Lith.settings.showImageThumbnails = showImageThumbnailsCheckbox.checked
        Lith.settings.openLinksDirectly = openLinksDirectlyCheckbox.checked
        Lith.settings.openLinksDirectlyInBrowser = openLinksDirectlyInBrowserSwitch.checked
        Lith.settings.scrollbarsOverlayContents = scrollbarsOverlayContentsCheckbox.checked
        Lith.settings.forceLightTheme = forceLightThemeCheckbox.checked
        Lith.settings.forceDarkTheme = forceDarkThemeCheckbox.checked
        Lith.settings.useTrueBlackWithDarkTheme = useTrueBlackWithDarkThemeCheckbox.checked
        Lith.settings.hotlistEnabled = hotlistEnabledCheckbox.checked
        Lith.settings.hotlistCompact = hotlistCompactCheckbox.checked
        Lith.settings.messageSpacing = messageSpacingSpinbox.value
        Lith.settings.showJoinPartQuitMessages = showJoinPartQuitMessagesCheckbox.checked
        Lith.settings.baseFontFamily = fontDialog.currentFont.family
        Lith.settings.showBufferListOnStartup = showBufferListOnStartupCheckbox.checked
        Lith.settings.platformBufferControlPosition = platformBufferControlPositionCheckbox.checked
        Lith.settings.bufferListShowsOnlyBuffersWithNewMessages = bufferListShowsOnlyBuffersWithNewMessagesCheckbox.checked
        Lith.settings.enableNotifications = enableNotificationsCheckbox.checked
    }
    function restore() {
        shortenLongUrlsCheckbox.checked = Lith.settings.shortenLongUrls
        shortenLongUrlsThreshold.text = Lith.settings.shortenLongUrlsThreshold
        useLocalInputBarCheckbox.checked = Lith.settings.useLocalInputBar
        showAutocompleteButtonCheckbox.checked = Lith.settings.showAutocompleteButton
        showGalleryButtonCheckbox.checked = Lith.settings.showGalleryButton
        showSendButtonCheckbox.checked = Lith.settings.showSendButton
        showSearchButtonCheckbox.checked = Lith.settings.showSearchButton
        baseFontSizeSpinBox.value = Lith.settings.baseFontSize
        nickCutoffThresholdSpinBox.value = Lith.settings.nickCutoffThreshold
        timestampFormatInput.text = Lith.settings.timestampFormat
        showDateHeadersCheckbox.checked = Lith.settings.showDateHeaders
        muteVideosByDefaultCheckbox.checked = Lith.settings.muteVideosByDefault
        loopVideosByDefaultCheckbox.checked = Lith.settings.loopVideosByDefault
        showImageThumbnailsCheckbox.checked = Lith.settings.showImageThumbnails
        openLinksDirectlyCheckbox.checked = Lith.settings.openLinksDirectly
        openLinksDirectlyInBrowserSwitch.checked = Lith.settings.openLinksDirectlyInBrowser
        scrollbarsOverlayContentsCheckbox.checked = Lith.settings.scrollbarsOverlayContents
        forceLightThemeCheckbox.checked = Lith.settings.forceLightTheme
        forceDarkThemeCheckbox.checked = Lith.settings.forceDarkTheme
        useTrueBlackWithDarkThemeCheckbox.checked = Lith.settings.useTrueBlackWithDarkTheme
        hotlistEnabledCheckbox.checked = Lith.settings.hotlistEnabled
        hotlistCompactCheckbox.checked = Lith.settings.hotlistCompact
        messageSpacingSpinbox.value = Lith.settings.messageSpacing
        showJoinPartQuitMessagesCheckbox.checked = Lith.settings.showJoinPartQuitMessages
        try {
            fontChangeButton.text = Lith.settings.baseFontFamily
            fontChangeButton.font.family = Lith.settings.baseFontFamily
        }
        catch(e) {

        }
        fontDialog.currentFont.family = Lith.settings.baseFontFamily
        showBufferListOnStartupCheckbox.checked = Lith.settings.showBufferListOnStartup
        platformBufferControlPositionCheckbox.checked = Lith.settings.platformBufferControlPosition
        bufferListShowsOnlyBuffersWithNewMessagesCheckbox.checked = Lith.settings.bufferListShowsOnlyBuffersWithNewMessages
        enableNotificationsCheckbox.checked = Lith.settings.enableNotifications
    }

    FontDialog {
        id: fontDialog
        currentFont.family: Lith.settings.baseFontFamily
        currentFont.pointSize: Lith.settings.baseFontSize
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
            width: WindowHelper.landscapeMode ? Math.min(Math.min(420, 1.33 * implicitWidth), parent.width) : parent.width
            spacing: -1

            Fields.Header {
                text: qsTr("Message settings")
            }

            Fields.Button {
                id: fontChangeButton
                summary: "Font family"
                details: "(requires restart)"
                visible: !LithPlatform.mobile
                text: Lith.settings.baseFontFamily
                onClicked: fontDialog.open()
            }

            Fields.IntSpinBox {
                id: baseFontSizeSpinBox
                Layout.alignment: Qt.AlignLeft
                summary: qsTr("Message font size")
                value: Lith.settings.baseFontSize
                from: 6
                to: 32
            }

            Fields.String {
                id: timestampFormatInput
                Layout.alignment: Qt.AlignLeft
                summary: qsTr("Timestamp format")
                text: Lith.settings.timestampFormat
            }

            Fields.Boolean {
                id: showDateHeadersCheckbox
                Layout.alignment: Qt.AlignLeft
                summary: qsTr("Show date headers in the buffer")
                checked: Lith.settings.showDateHeaders
            }

            Fields.Boolean {
                id: showJoinPartQuitMessagesCheckbox
                checked: Lith.settings.showJoinPartQuitMessages
                summary: qsTr("Show join/part messages")
            }

            Fields.IntSpinBox {
                id: nickCutoffThresholdSpinBox
                summary: qsTr("Align nick length")
                from: -1
                to: 100
                value: Lith.settings.nickCutoffThreshold
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
                value: Lith.settings.messageSpacing
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
                checked: Lith.settings.scrollbarsOverlayContents
            }

            ////////////////////////// COLOR THEME
            Fields.Header {
                text: qsTr("Color theme")
            }

            Fields.Boolean {
                id: forceLightThemeCheckbox
                summary: qsTr("Force light theme")
                checked: Lith.settings.forceLightTheme
                onCheckedChanged: {
                    if (checked)
                        forceDarkThemeCheckbox.checked = false
                }
            }

            Fields.Boolean {
                id: forceDarkThemeCheckbox
                summary: qsTr("Force dark theme")
                checked: Lith.settings.forceDarkTheme
                onCheckedChanged: {
                    if (checked)
                        forceLightThemeCheckbox.checked = false
                }
            }

            Fields.Boolean {
                id: useTrueBlackWithDarkThemeCheckbox
                summary: qsTr("Use black in dark theme")
                checked: Lith.settings.useTrueBlackWithDarkTheme
            }

            ////////////////////////// INPUT BAR
            Fields.Header {
                text: qsTr("Input bar")
            }

            Fields.Boolean {
                id: useLocalInputBarCheckbox
                summary: qsTr("Use local input bar")
                details: checked ? qsTr("Last input will be remembered in each buffer") : qsTr("Input contents won't change when switching buffers")
                checked: Lith.settings.useLocalInputBar
            }

            Fields.Boolean {
                id: showAutocompleteButtonCheckbox
                summary: qsTr("Show autocomplete button")
                checked: Lith.settings.showAutocompleteButton
            }

            Fields.Boolean {
                id: showGalleryButtonCheckbox
                summary: qsTr("Show gallery button")
                checked: Lith.settings.showGalleryButton
            }

            Fields.Boolean {
                id: showSendButtonCheckbox
                summary: qsTr("Show send button")
                checked: Lith.settings.showSendButton
            }

            Fields.Boolean {
                id: showSearchButtonCheckbox
                summary: qsTr("Show search button")
                checked: Lith.settings.showSearchButton
            }

            ////////////////////////// HOTLIST
            Fields.Header {
                text: qsTr("Hotlist")
            }

            Fields.Boolean {
                id: hotlistEnabledCheckbox
                summary: qsTr("Enable hotlist")
                checked: Lith.settings.hotlistEnabled
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
                checked: Lith.settings.hotlistCompact
            }

            ////////////////////////// BUFFER LIST
            Fields.Header {
                text: qsTr("Buffer list")
            }

            Fields.Boolean {
                id: showBufferListOnStartupCheckbox
                summary: qsTr("Show buffer list on startup")
                checked: Lith.settings.showBufferListOnStartup
            }

            Fields.Boolean {
                id: platformBufferControlPositionCheckbox
                summary: LithPlatform.mobile ? qsTr("Render buffer search on the bottom") : qsTr("Render buffer search on the top")
                checked: Lith.settings.platformBufferControlPosition
            }

            Fields.Boolean {
                id: bufferListShowsOnlyBuffersWithNewMessagesCheckbox
                summary: qsTr("Show only buffers with new messages")
                details: qsTr("When using the text filter, all buffers will be shown.")
                checked: Lith.settings.bufferListShowsOnlyBuffersWithNewMessages
            }

            ////////////////////////// URL HANDLING
            Fields.Header {
                text: qsTr("URL handling")
            }

            Fields.Boolean {
                id: shortenLongUrlsCheckbox
                summary: qsTr("Shortening Enabled")
                checked: Lith.settings.shortenLongUrls
            }

            Fields.String {
                id: shortenLongUrlsThreshold
                summary: qsTr("Length threshold")
                enabled: shortenLongUrlsCheckbox.checked
                text: Lith.settings.shortenLongUrlsThreshold
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
                checked: Lith.settings.showImageThumbnails
            }

            Fields.Boolean {
                id: openLinksDirectlyCheckbox
                summary: qsTr("Open links directly")
                details: checked ? qsTr("Lith will either show media (images, video) directly with a built-in player or open your default browser (unsupported media, other link types)") : qsTr("Lith will show a popup allowing you to choose between copying the URL, opening it in your default browser or showing it inside Lith if supported (images, video)")
                checked: Lith.settings.openLinksDirectly
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
                            color: openLinksDirectlyInBrowserSwitch.checked ? LithPalette.disabled.text : LithPalette.regular.text
                            MouseArea {
                                id: insideLithMouse
                                anchors.fill: parent
                                hoverEnabled: true

                                ToolTip.text: insideLithLabel.text
                                ToolTip.visible: LithPlatform.mobile ? insideLithMouse.containsPress : insideLithMouse.containsMouse
                                ToolTip.delay: LithPlatform.mobile ? 0 : 800
                            }
                        }
                    }
                    Switch {
                        id: openLinksDirectlyInBrowserSwitch
                        checked: Lith.settings.openLinksDirectlyInBrowser
                        //onColor: offColor
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
                            color: openLinksDirectlyInBrowserSwitch.checked ? LithPalette.regular.text : LithPalette.disabled.text
                            MouseArea {
                                id: inBrowserMouse
                                anchors.fill: parent
                                hoverEnabled: true

                                ToolTip.text: inBrowserLabel.text
                                ToolTip.visible: LithPlatform.mobile ? inBrowserMouse.containsPress : inBrowserMouse.containsMouse
                                ToolTip.delay: LithPlatform.mobile ? 0 : 800
                            }
                        }
                    }
                }
            }

            Fields.Boolean {
                id: loopVideosByDefaultCheckbox
                summary: qsTr("Loop videos by default")
                checked: Lith.settings.showSendButton
            }

            Fields.Boolean {
                id: muteVideosByDefaultCheckbox
                summary: qsTr("Mute videos by default")
                checked: Lith.settings.showSendButton
            }

            Fields.Header {
                text: qsTr("Notification settings")
            }

            Fields.Boolean {
                id: enableNotificationsCheckbox
                summary: qsTr("Enable notifications")
                details: LithPlatform.ios ? qsTr("This setting won't do anything for now, we need to implement a server and WeeChat plugin to actually pass notifications to Lith.") : ""
                checked: Lith.settings.enableNotifications
            }

            Fields.Base {
                id: deviceTokenField
                summary: "Device token"
                visible: LithPlatform.ios

                rowComponent: Label {
                    Layout.preferredWidth: settingsPaneLayout.width / 2
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    text: Lith.notificationHandler.deviceToken
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            ClipboardProxy.setText(Lith.notificationHandler.deviceToken)
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
