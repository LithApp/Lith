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
    implicitHeight: inputBarLayout.height

    // TODO
    property alias textInput: channelTextInput
    property bool hasFocus: channelTextInput.activeFocus
    property bool isSearching: false

    RowLayout {
        width: parent.width
        id: inputBarLayout

        spacing: 3

        Button {
            focusPolicy: Qt.NoFocus
            id: searchButton
            Layout.preferredWidth: height
            icon.source: "qrc:/navigation/"+currentTheme+"/download-rotated.png"
            visible: settings.showSendButton
            font.pointSize: settings.baseFontSize
            onClicked: {
                isSearching = !isSearching
                if (isSearching)
                {
                    textInput.inputField.text = "asdf" // undefined, nefunguje
                }

                if (!isSearching) {
                   textInput.inputField.text = ""
                   lith.selectedBuffer.lines_filtered.filterWord = ""
                }
            }
        }

        Button {
            id: autocompleteButton
            Layout.preferredWidth: height
            icon.source: "qrc:/navigation/"+currentTheme+"/download-rotated.png"
            font.pointSize: settings.baseFontSize
            focusPolicy: Qt.NoFocus
            visible: settings.showAutocompleteButton
            onClicked: {
                channelTextInput.autocomplete();
            }
        }

        ChannelTextInput {
            id: channelTextInput
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }

        Button {
            focusPolicy: Qt.NoFocus
            id: imageButton
            visible: settings.showGalleryButton
            Layout.preferredWidth: height
            property bool isBusy: uploader.working
            icon.source: isBusy ? "" : "qrc:/navigation/"+currentTheme+"/image-gallery.png"
            enabled: !isBusy
            font.pointSize: settings.baseFontSize
            onClicked: {
                fileDialog.open()
            }
            BusyIndicator {
                id: busy
                visible: parent.isBusy
                anchors.fill: parent
                scale: 0.7
            }
        }

        Button {
            focusPolicy: Qt.NoFocus
            id: sendButton
            Layout.preferredWidth: height
            icon.source: "qrc:/navigation/"+currentTheme+"/paper-plane.png"
            visible: settings.showSendButton
            font.pointSize: settings.baseFontSize
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
        color: palette.text
        opacity: 0.3
    }
}
