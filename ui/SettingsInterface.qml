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
    }

    ColumnLayout {
        x: 6
        y: 6
        width: root.width - 12

        GridLayout {
            id: inputBarLayout
            columns: 2
            Layout.alignment: Qt.AlignHCenter

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Message font size")
            }
            SpinBox {
                id: baseFontSizeSpinBox
                value: settings.baseFontSize
                from: 6
                to: 32
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Timestamp format")
            }
            TextField {
                id: timestampFormatInput
                text: lith.settings.timestampFormat
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Show join/part/quit messages")
            }
            CheckBox {
                id: showJoinPartQuitMessagesCheckbox
                checked: settings.showJoinPartQuitMessages
                Layout.alignment: Qt.AlignLeft
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Align nick length")
            }
            SpinBox {
                id: nickCutoffThresholdSpinBox
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
            Rectangle {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                height: 1
                color: palette.base
            }
            Label {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: qsTr("<b>Color theme</b>")
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Force light theme")
            }
            CheckBox {
                id: forceLightThemeCheckbox
                checked: settings.forceLightTheme
                Layout.alignment: Qt.AlignLeft
                onCheckedChanged: {
                    if (checked)
                        forceDarkThemeCheckbox.checked = false
                }
            }
            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Force dark theme")
            }
            CheckBox {
                id: forceDarkThemeCheckbox
                checked: settings.forceDarkTheme
                Layout.alignment: Qt.AlignLeft
                onCheckedChanged: {
                    if (checked)
                        forceLightThemeCheckbox.checked = false
                }
            }
            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Use black in dark theme")
            }
            CheckBox {
                id: useTrueBlackWithDarkThemeCheckbox
                checked: settings.useTrueBlackWithDarkTheme
                Layout.alignment: Qt.AlignLeft
            }


            ////////////////////////// INPUT BAR
            Rectangle {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                height: 1
                color: palette.base
            }
            Label {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: qsTr("Input bar")
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Show autocomplete button")
            }
            CheckBox {
                id: showAutocompleteButtonCheckbox
                checked: settings.showAutocompleteButton
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Show gallery button")
            }
            CheckBox {
                id: showGalleryButtonCheckbox
                checked: settings.showGalleryButton
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Show send button")
            }
            CheckBox {
                id: showSendButtonCheckbox
                checked: settings.showSendButton
                Layout.alignment: Qt.AlignLeft
            }

            ////////////////////////// HOTLIST
            Rectangle {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                height: 1
                color: palette.base
            }
            Label {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: qsTr("Hotlist")
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Enable hotlist")
            }
            CheckBox {
                id: hotlistEnabledCheckbox
                checked: settings.hotlistEnabled
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                Layout.alignment: Qt.AlignRight
                enabled: hotlistEnabledCheckbox.checked
                text: qsTr("Show unread count")
            }
            CheckBox {
                id: hotlistShowUnreadCountCheckbox
                enabled: hotlistEnabledCheckbox.checked
                checked: settings.hotlistShowUnreadCount
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                Layout.alignment: Qt.AlignRight
                enabled: hotlistEnabledCheckbox.checked
                text: qsTr("Use compact layout")
            }
            CheckBox {
                id: hotlistCompactCheckbox
                enabled: hotlistEnabledCheckbox.checked
                checked: settings.hotlistCompact
                Layout.alignment: Qt.AlignLeft
            }

            ////////////////////////// URL HANDLING
            Rectangle {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                height: 1
                color: palette.base
            }

            Label {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: qsTr("URL handling")
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Shortening Enabled")
            }
            CheckBox {
                id: shortenLongUrlsCheckbox
                checked: settings.shortenLongUrls
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Length threshold")
            }
            TextField {
                id: shortenLongUrlsThreshold
                enabled: shortenLongUrlsCheckbox.checked
                text: settings.shortenLongUrlsThreshold
                inputMethodHints: Qt.ImhPreferNumbers
                validator: IntValidator {
                    bottom: 0
                }
            }

            ////////////////////////// MULTIMEDIA
            Rectangle {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                height: 1
                color: palette.base
            }
            Label {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: qsTr("Multimedia")
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Open links directly")
            }
            CheckBox {
                id: openLinksDirectlyCheckbox
                checked: settings.openLinksDirectly
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                visible: openLinksDirectlyCheckbox.checked
                Layout.alignment: Qt.AlignRight
                text: qsTr("Inside Lith (when possible)")
                color: openLinksDirectlyInBrowserSwitch.checked ? disabledPalette.text : palette.text
            }
            RowLayout {
                visible: openLinksDirectlyCheckbox.checked
                Switch {
                    id: openLinksDirectlyInBrowserSwitch
                    checked: lith.settings.openLinksDirectlyInBrowser
                }
                Label {
                    text: qsTr("In browser")
                    color: openLinksDirectlyInBrowserSwitch.checked ? palette.text : disabledPalette.text
                }
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Loop videos by default")
            }
            CheckBox {
                id: loopVideosByDefaultCheckbox
                checked: settings.showSendButton
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Mute videos by default")
            }
            CheckBox {
                id: muteVideosByDefaultCheckbox
                checked: settings.showSendButton
                Layout.alignment: Qt.AlignLeft
            }

            ////////////////////////// ADVANCED TWEAKS
            Rectangle {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                height: 1
                color: palette.base
            }

            Label {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: qsTr("Advanced tweaks")
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: qsTr("Message spacing")
            }
            SpinBox {
                id: messageSpacingSpinbox
                value: settings.messageSpacing
                Layout.alignment: Qt.AlignLeft
            }
        }
    }
}
