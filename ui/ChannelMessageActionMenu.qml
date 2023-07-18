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

/* Hold-to-copy action menu */

Dialog {
    id: channelMessageActionMenuDialog
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    width: parent.width / 1.25

    // Center the popup/dialog, whatever
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)


    property string message;
    property string nickname;
    property var timestamp;

    function show(msg, nick, time) {
        message = msg
        nickname = nick
        timestamp = time
        visible = true
    }

    SystemPalette {
        id: palette
    }

    header: Label {
        topPadding: 20
        text: qsTr("Copy")
        font.bold: true
        size: Label.Medium
        horizontalAlignment: Label.AlignHCenter
    }

    ColumnLayout
    {
        spacing: 16
        // columns: 1
        width: parent.width - 2
        x: 1

        Button {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 1.5 * implicitHeight
            Layout.topMargin: 16
            flat: true

            Label {
                id: label1
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 6
                    verticalCenter: parent.verticalCenter
                }
                maximumLineCount: 2
                elide: Text.ElideRight
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: timestamp ? timestamp.toLocaleTimeString(Qt.locale(), lith.settings.timestampFormat) + " <" + nickname + "> " + message : ""
            }

            visible: !(nickname == "")

            onClicked: {
                channelMessageActionMenuDialog.close()
                clipboardProxy.setText(label1.text)
            }
        }
        Button {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 1.5 * implicitHeight
            flat: true

            visible: !(nickname == "")

            Label {
                id: label2
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 6
                    verticalCenter: parent.verticalCenter
                }
                maximumLineCount: 2
                elide: Text.ElideRight
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: "<" + nickname + "> " + message
            }

            onClicked: {
                channelMessageActionMenuDialog.close()
                clipboardProxy.setText(label2.text)
            }
        }
        Button {
            clip: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 1.5 * implicitHeight
            Layout.bottomMargin: 16
            flat: true

            Label {
                id: label3
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 6
                    verticalCenter: parent.verticalCenter
                }
                maximumLineCount: 2
                elide: Text.ElideRight
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: message
            }

            onClicked: {
                channelMessageActionMenuDialog.close()
                clipboardProxy.setText(label3.text)
            }
        }
    }
}


