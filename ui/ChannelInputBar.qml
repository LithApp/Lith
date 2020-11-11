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


RowLayout {
    spacing: 3
    // TODO
    property alias textInput: channelTextInput
    property bool hasFocus: channelTextInput.activeFocus
    Button {
        Layout.preferredWidth: height
        icon.source: "qrc:/navigation/download-rotated.png"
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
        icon.source: isBusy ? null : "qrc:/navigation/image-gallery.png"
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
        Connections {
            target: uploader
            onSuccess: {
                console.warn("FINISHED")
                console.warn(url)
                if (channelTextInput.length !== 0 && !channelTextInput.text.endsWith(" "))
                    channelTextInput.text += " "
                channelTextInput.text += url
                channelTextInput.text += " "
            }
            onError: {
                console.warn("ERROR")
                console.warn(message)
            }
        }

    }

    Button {
        focusPolicy: Qt.NoFocus
        id: sendButton
        Layout.preferredWidth: height
        icon.source: "qrc:/navigation/paper-plane.png"
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
