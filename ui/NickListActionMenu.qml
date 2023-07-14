// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Jakub Mach
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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import lith 1.0

Dialog {
    id: nickListActionMenuDialog
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    width: parent.width / 1.5

    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    property string nickname;

    SystemPalette {
        id: palette
    }

    header: Label {
        topPadding: 20
        bottomPadding: 10
        id: nickActionNicknameText
        text: nickname
        horizontalAlignment: Qt.AlignCenter
        size: Label.Medium
        font.bold: true
    }

    ListModel {
        id: nicklistActionMenuModel

        ListElement {
            name: qsTr("Open query")
            operation: "query"
        }
        ListElement {
            name: qsTr("Op")
            operation: "op"
        }
        ListElement {
            name: qsTr("Deop")
            operation: "deop"
        }
        ListElement {
            name: qsTr("Kick")
            operation:"kick"
        }
        ListElement {
            name: qsTr("Kickban")
            operation: "kickban"

        }
    }

    ColumnLayout {
        width: parent.width
        anchors.centerIn: parent
        Repeater {
            model: nicklistActionMenuModel
            delegate: Button {
                Layout.margins: 9
                text: name
                onClicked: {
                    lith.selectedBuffer.input("/" + operation + " " + nickname)
                    nickListActionMenuDialog.close()
                    nickDrawer.close()
                }
                width: parent.fillWidth
                Layout.fillWidth: true
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }

}
