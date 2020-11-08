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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2 as Dialogs

import lith 1.0

ColumnLayout {
    spacing: 0
    SystemPalette {
        id: palette
    }

    property bool inputBarHasFocus: inputBar.hasFocus
    property alias textInput: inputBar.textInput

    ChannelHeader {
        id: channelHeader
        Layout.fillWidth: true
    }

    Text {
        Layout.fillHeight: true
        Layout.fillWidth: true
        visible: !lith.selectedBuffer
        text: "Welcome to Lith\n" +
              "Weechat status: " + lith.status + "\n" +
              "Current error status: " + (lith.errorString.length > 0 ? lith.errorString : "None")
        color: palette.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        ChannelMessageList {
            id: channelMessageList
            width: parent.width
            height: parent.height
            visible: lith.selectedBuffer
        }
    }

    ChannelInputBar {
        id: inputBar
        Layout.fillWidth: true
    }

    Dialogs.FileDialog {
        id: fileDialog
        folder: shortcuts.pictures
        nameFilters: [ "Image files (*.jpg *.png)" ]
        onAccepted: {
            //inputField.text += " " + fileUrl
            //imageButton.isBusy = false
            uploader.upload(fileUrl)
            Qt.inputMethod.hide()
            inputBar.textInput.forceActiveFocus()
        }
        onRejected: {
            Qt.inputMethod.hide()
            inputBar.textInput.forceActiveFocus()
        }
    }
}
