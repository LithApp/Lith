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

    property bool inputBarHasFocus: inputBar.hasFocus
    property alias textInput: inputBar.textInput
    property alias messageArea: messageArea

    ChannelHeader {
        id: channelHeader
        Layout.fillWidth: true
    }

    Item {
        id: messageArea
        Layout.fillHeight: true
        Layout.fillWidth: true
        clip: true

        ColumnLayout {
            visible: !lith.selectedBuffer
            anchors.fill: parent
            Label {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: ""
                onLinkActivated: {
                    Qt.openUrlExternally(link)
                }
                textFormat: Text.RichText
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            Label {
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
        }

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
        folder: StandardPaths.standardLocations(StandardPaths.PicturesLocation).slice(-1)[0]
        nameFilters: [ "Image files (*.jpg *.jpeg *.png)" ]
        onAccepted: {
            //inputField.text += " " + fileUrl
            //imageButton.isBusy = false
            uploader.upload(file)
            Qt.inputMethod.hide()
            inputBar.textInput.forceActiveFocus()
        }
        onRejected: {
            Qt.inputMethod.hide()
            inputBar.textInput.forceActiveFocus()
        }
    }
}
