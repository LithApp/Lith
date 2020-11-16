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
import Qt.labs.platform 1.1

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
        text: qsTr("Welcome to Lith") + "<br>" +
              qsTr("Weechat status:") + " " + lith.status + "<br>" +
              qsTr("Current error status:") + " " + (lith.errorString.length > 0 ? lith.errorString : qsTr("None")) + "<br>"
        onLinkActivated: {
            Qt.openUrlExternally(link)
        }
        textFormat: Text.RichText
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: palette.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    Text {
        visible: !lith.selectedBuffer
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        onLinkActivated: {
            Qt.openUrlExternally(link)
        }
        textFormat: Text.RichText
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: qsTr("Icons made by <a href=\"https://www.flaticon.com/authors/pixel-perfect\" title=\"Pixel perfect\">Pixel perfect</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\"> www.flaticon.com</a>")
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
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

    FileDialog {
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
