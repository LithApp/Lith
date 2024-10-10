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
import QtQuick.Layouts
import QtQuick.Controls
import QtQml

import Lith.Core
import Lith.Style

import "util" as Util

Item {
    id: root
    implicitHeight: inputBarLayout.implicitHeight + Lith.settings.uiMargins * 2

    // TODO
    property alias textInput: channelTextInput
    property bool hasFocus: channelTextInput.activeFocus
    readonly property real implicitContentHeight: inputBarLayout.height

    signal requestSearchBar

    onVisibleChanged: {
        if (visible) {
            channelTextInput.forceActiveFocus()
        }
    }

    Util.ControlPanel {
        anchors.fill: parent
        anchors.leftMargin: Lith.settings.uiMargins
        anchors.topMargin: 0
        anchors.rightMargin: anchors.leftMargin
        anchors.bottomMargin: Lith.settings.uiMargins
        radius: Math.pow(Lith.settings.uiMargins, 0.9)
    }

    RowLayout {
        id: inputBarLayout
        height: parent.height - 2 * Lith.settings.uiMargins

        spacing: 3 + Lith.settings.uiMargins / 3
        anchors.top: parent.top
        anchors.topMargin: 0.5 * Lith.settings.uiMargins
        anchors.left: parent.left
        anchors.leftMargin: 2 * Lith.settings.uiMargins
        anchors.right: parent.right
        anchors.rightMargin: 2 * Lith.settings.uiMargins

        Button {
            id: searchModeButton
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: height
            icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/loupe.png"
            focusPolicy: Qt.NoFocus
            flat: true
            visible: Lith.settings.showSearchButton
            onClicked: {
                Window.activeFocusItem.focus = false
                root.requestSearchBar()
            }
            ToolTip.text: "Enter search mode"
            ToolTip.visible: searchModeButton.hovered
            ToolTip.delay: 800
        }

        Button {
            id: autocompleteButton
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: height
            icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/download-rotated.png"
            focusPolicy: Qt.NoFocus
            flat: true
            visible: Lith.settings.showAutocompleteButton
            onClicked: {
                channelTextInput.autocomplete();
            }
            ToolTip.text: "Autocomplete nickname"
            ToolTip.visible: autocompleteButton.hovered
            ToolTip.delay: 800
        }

        // First part of a hack to show more lines of text when writing a long message
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: autocompleteButton.implicitHeight
            ChannelTextInput {
                id: channelTextInput
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: parent.height
            }
        }

        Button {
            id: imageButton
            visible: Lith.settings.showGalleryButton
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: height
            property bool isBusy: Uploader.working
            icon.source: isBusy ? "" : "qrc:/navigation/"+WindowHelper.currentThemeName+"/image-gallery.png"
            enabled: !isBusy
            focusPolicy: Qt.NoFocus
            flat: true
            onClicked: {
                fileDialog.open()
            }
            ToolTip.text: "Upload a picture"
            ToolTip.visible: imageButton.hovered
            ToolTip.delay: 800
            BusyIndicator {
                id: busy
                visible: parent.isBusy
                anchors.fill: parent
                scale: 0.7
            }
        }

        Button {
            id: sendButton
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: height
            icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/paper-plane.png"
            visible: Lith.settings.showSendButton
            focusPolicy: Qt.NoFocus
            flat: true
            ToolTip.text: "Send the message"
            ToolTip.visible: sendButton.hovered
            ToolTip.delay: 800
            onClicked: {
                channelTextInput.input()
            }
        }
    }
}
