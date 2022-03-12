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

    function onAccepted() {
        settings.shortenLongUrls = shortenLongUrlsCheckbox.checked
        settings.shortenLongUrlsThreshold = shortenLongUrlsThreshold.text
        settings.showAutocompleteButton = showAutocompleteButtonCheckbox.checked
        settings.showGalleryButton = showGalleryButtonCheckbox.checked
        settings.showSendButton = showSendButtonCheckbox.checked
        settings.baseFontSize = baseFontSizeSpinBox.value
        settings.nickCutoffThreshold = nickCutoffThresholdSpinBox.value
        settings.timestampFormat = timestampFormatInput.text
        settings.muteVideosByDefault = muteVideosByDefaultCheckbox.checked
        settings.loopVideosByDefault = loopVideosByDefaultCheckbox.checked
        settings.openLinksDirectly = openLinksDirectlyCheckbox.checked
        lith.settings.openLinksDirectlyInBrowser = openLinksDirectlyInBrowserSwitch.checked
        settings.forceLightTheme = forceLightThemeCheckbox.checked
        settings.forceDarkTheme = forceDarkThemeCheckbox.checked
        settings.useTrueBlackWithDarkTheme = useTrueBlackWithDarkThemeCheckbox.checked
        settings.hotlistEnabled = hotlistEnabledCheckbox.checked
        settings.hotlistCompact = hotlistCompactCheckbox.checked
        settings.hotlistShowUnreadCount = hotlistShowUnreadCountCheckbox.checked
        settings.messageSpacing = messageSpacingSpinbox.value
        settings.showJoinPartQuitMessages = showJoinPartQuitMessagesCheckbox.checked
        settings.baseFontFamily = fontDialog.currentFont.family
        settings.showBufferListOnStartup = showBufferListOnStartupCheckbox.checked
    }
    function onRejected() {
        shortenLongUrlsCheckbox.checked = settings.shortenLongUrls
        shortenLongUrlsThreshold.text = settings.shortenLongUrlsThreshold
        showAutocompleteButtonCheckbox.checked = settings.showAutocompleteButton
        showGalleryButtonCheckbox.checked = settings.showGalleryButton
        showSendButtonCheckbox.checked = settings.showSendButton
        baseFontSizeSpinBox.value = settings.baseFontSize
        nickCutoffThresholdSpinBox.value = settings.nickCutoffThreshold
        timestampFormatInput.text = settings.timestampFormat
        muteVideosByDefaultCheckbox.checked = settings.muteVideosByDefault
        loopVideosByDefaultCheckbox.checked = settings.loopVideosByDefault
        openLinksDirectlyCheckbox.checked = settings.openLinksDirectly
        openLinksDirectlyInBrowserSwitch.checked = lith.settings.openLinksDirectlyInBrowser
        forceLightThemeCheckbox.checked = settings.forceLightTheme
        forceDarkThemeCheckbox.checked = settings.forceDarkTheme
        useTrueBlackWithDarkThemeCheckbox.checked = settings.useTrueBlackWithDarkTheme
        hotlistEnabledCheckbox.checked = settings.hotlistEnabled
        hotlistCompactCheckbox.checked = settings.hotlistCompact
        hotlistShowUnreadCountCheckbox.checked = settings.hotlistShowUnreadCount
        messageSpacingSpinbox.value = settings.messageSpacing
        showJoinPartQuitMessagesCheckbox.checked = settings.showJoinPartQuitMessages
        fontChangeButton.text = settings.baseFontFamily
        fontChangeButton.font.family = settings.baseFontFamily
        fontDialog.currentFont.family = settings.baseFontFamily
        showBufferListOnStartupCheckbox.checked = settings.showBufferListOnStartup
    }

    FontDialog {
        id: fontDialog
        currentFont.family: settings.baseFontFamily
        flags: FontDialog.MonospacedFonts
        onAccepted: {
            fontChangeButton.text = fontDialog.selectedFont.family
            fontChangeButton.font.family = fontDialog.selectedFont.family
        }
    }

    Item {
        width: parent.width
        implicitHeight: settingsPaneLayout.implicitHeight

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
                visible: !mobilePlatform
                text: settings.baseFontFamily
                onClicked: fontDialog.open()
            }

            Fields.IntSpinBox {
                id: baseFontSizeSpinBox
                Layout.alignment: Qt.AlignLeft
                summary: qsTr("Message font size")
                value: settings.baseFontSize
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
                id: showJoinPartQuitMessagesCheckbox
                checked: settings.showJoinPartQuitMessages
                summary: qsTr("Show join/part messages")
            }

            Fields.IntSpinBox {
                id: nickCutoffThresholdSpinBox
                summary: qsTr("Align nick length")
                from: -1
                to: 100
                value: settings.nickCutoffThreshold
                textFromValue: function(value, locale) {
                    if (value >= 0)
                        return Number(value)
                    return qsTr("Disabled")
                }
            }

            ////////////////////////// COLOR THEME
            Fields.Header {
                text: qsTr("Color theme")
            }

            Fields.Boolean {
                id: forceLightThemeCheckbox
                summary: qsTr("Force light theme")
                checked: settings.forceLightTheme
                onCheckedChanged: {
                    if (checked)
                        forceDarkThemeCheckbox.checked = false
                }
            }

            Fields.Boolean {
                id: forceDarkThemeCheckbox
                summary: qsTr("Force dark theme")
                checked: settings.forceDarkTheme
                onCheckedChanged: {
                    if (checked)
                        forceLightThemeCheckbox.checked = false
                }
            }

            Fields.Boolean {
                id: useTrueBlackWithDarkThemeCheckbox
                summary: qsTr("Use black in dark theme")
                checked: settings.useTrueBlackWithDarkTheme
            }

            ////////////////////////// INPUT BAR
            Fields.Header {
                text: qsTr("Input bar")
            }

            Fields.Boolean {
                id: showAutocompleteButtonCheckbox
                summary: qsTr("Show autocomplete button")
                checked: settings.showAutocompleteButton
            }

            Fields.Boolean {
                id: showGalleryButtonCheckbox
                summary: qsTr("Show gallery button")
                checked: settings.showGalleryButton
            }

            Fields.Boolean {
                id: showSendButtonCheckbox
                summary: qsTr("Show send button")
                checked: settings.showSendButton
            }

            ////////////////////////// HOTLIST
            Fields.Header {
                text: qsTr("Hotlist")
            }

            Fields.Boolean {
                id: hotlistEnabledCheckbox
                summary: qsTr("Enable hotlist")
                checked: settings.hotlistEnabled
            }

            Fields.Boolean {
                id: hotlistShowUnreadCountCheckbox
                summary: qsTr("Show unread count")
                enabled: hotlistEnabledCheckbox.checked
                checked: settings.hotlistShowUnreadCount
            }

            Fields.Boolean {
                id: hotlistCompactCheckbox
                summary: qsTr("Use compact layout")
                enabled: hotlistEnabledCheckbox.checked
                checked: settings.hotlistCompact
            }

            ////////////////////////// BUFFER LIST
            Fields.Header {
                text: qsTr("Buffer list")
            }

            Fields.Boolean {
                id: showBufferListOnStartupCheckbox
                summary: qsTr("Show buffer list on startup")
                enabled: hotlistEnabledCheckbox.checked
                checked: settings.showBufferListOnStartup
            }

            ////////////////////////// URL HANDLING
            Fields.Header {
                text: qsTr("URL handling")
            }

            Fields.Boolean {
                id: shortenLongUrlsCheckbox
                summary: qsTr("Shortening Enabled")
                checked: settings.shortenLongUrls
            }

            Fields.String {
                id: shortenLongUrlsThreshold
                summary: qsTr("Length threshold")
                enabled: shortenLongUrlsCheckbox.checked
                text: settings.shortenLongUrlsThreshold
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
                id: openLinksDirectlyCheckbox
                summary: qsTr("Open links directly")
                checked: settings.openLinksDirectly
                columnComponent: RowLayout {
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    Layout.bottomMargin: 6
                    Layout.fillWidth: true
                    enabled: openLinksDirectlyCheckbox.checked
                    // Layout balancing wrapper
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Label {
                            anchors.fill: parent
                            verticalAlignment: Label.AlignVCenter
                            horizontalAlignment: Label.AlignRight
                            elide: Label.ElideRight
                            text: qsTr("Inside Lith (if possible)")
                            color: openLinksDirectlyInBrowserSwitch.checked ? disabledPalette.text : palette.text
                        }
                    }
                    Switch {
                        id: openLinksDirectlyInBrowserSwitch
                        checked: lith.settings.openLinksDirectlyInBrowser
                    }
                    // Layout balancing wrapper
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Label {
                            anchors.fill: parent
                            verticalAlignment: Label.AlignVCenter
                            elide: Label.ElideRight
                            text: qsTr("In a web browser")
                            color: openLinksDirectlyInBrowserSwitch.checked ? palette.text : disabledPalette.text
                        }
                    }
                }
            }

            Fields.Boolean {
                id: loopVideosByDefaultCheckbox
                summary: qsTr("Loop videos by default")
                checked: settings.showSendButton
            }

            Fields.Boolean {
                id: muteVideosByDefaultCheckbox
                summary: qsTr("Mute videos by default")
                checked: settings.showSendButton
            }


            ////////////////////////// ADVANCED TWEAKS
            Fields.Header {
                text: qsTr("Advanced tweaks")
            }

            Fields.IntSpinBox {
                id: messageSpacingSpinbox
                summary: qsTr("Message spacing")
                value: settings.messageSpacing
            }
        }
    }
}
