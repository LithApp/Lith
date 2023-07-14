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
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQml 2.12

import lith 1.0


Item {
    id: root
    implicitHeight: inputBarLayout.implicitHeight + 1

    // TODO
    property alias textInput: channelTextInput
    property bool hasFocus: channelTextInput.activeFocus

    signal requestSearchBar

    onVisibleChanged: {
        if (visible) {
            channelTextInput.forceActiveFocus()
        }
    }

    RowLayout {
        id: inputBarLayout
        y: 1
        height: parent.height
        width: parent.width

        spacing: 6
        Button {
            id: searchModeButton
            implicitWidth: implicitHeight
            icon.source: "qrc:/navigation/"+currentTheme+"/loupe.png"
            focusPolicy: Qt.NoFocus
            flat: true
            visible: settings.showSearchButton
            onClicked: {
                root.requestSearchBar()
            }
            ToolTip.text: "Enter search mode"
            ToolTip.visible: searchModeButton.hovered
            ToolTip.delay: 800
        }
        Button {
            id: autocompleteButton
            Layout.fillHeight: true
            Layout.preferredWidth: height
            icon.source: "qrc:/navigation/"+currentTheme+"/download-rotated.png"
            focusPolicy: Qt.NoFocus
            flat: true
            visible: settings.showAutocompleteButton
            onClicked: {
                channelTextInput.autocomplete();
            }
            ToolTip.text: "Autocomplete nickname"
            ToolTip.visible: autocompleteButton.hovered
            ToolTip.delay: 800
        }

        ChannelTextInput {
            id: channelTextInput
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Button {
            id: imageButton
            visible: settings.showGalleryButton
            Layout.fillHeight: true
            Layout.preferredWidth: height
            property bool isBusy: uploader.working
            icon.source: isBusy ? "" : "qrc:/navigation/"+currentTheme+"/image-gallery.png"
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
            Layout.fillHeight: true
            Layout.preferredWidth: height
            icon.source: "qrc:/navigation/"+currentTheme+"/paper-plane.png"
            visible: settings.showSendButton
            focusPolicy: Qt.NoFocus
            flat: true
            ToolTip.text: "Send the message"
            ToolTip.visible: sendButton.hovered
            ToolTip.delay: 800
            onClicked: {
                if (channelTextInput.inputFieldAlias.text.length > 0) {
                    lith.selectedBuffer.input(channelTextInput.inputFieldAlias.text)
                    channelTextInput.inputFieldAlias.text = ""
                }
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 1
        color: colorUtils.mixColors(palette.text, palette.window, 0.3)
    }
}
