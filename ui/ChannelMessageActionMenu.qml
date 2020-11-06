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

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import lith 1.0

/* Hold-to-copy action menu */

Dialog {
    id: channelMessageActionMenuDialog
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onVisibleChanged: {
        console.log(" visible = " + !(nickname == ""))
    }

    width: parent.width / 1.25

    // Center the popup/dialog, whatever
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)


    property string message;
    property string nickname;
    property string timestamp;

    SystemPalette {
        id: palette
    }

    header: Label {
        topPadding: 20
        leftPadding: 20
        text: "Copy"
        font.bold: true
        font.pointSize: settings.baseFontSize * 1.125
    }

    background: Rectangle {
        color: "#eeeeee"
    }

    ColumnLayout
    {
        spacing: 16
        // columns: 1
        width: parent.width

        Rectangle {
            clip: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter

            width: parent.width
            visible: !(nickname == "")
            height: visible ? 45 : 0
            color: mouseNicknameTimestamp.pressed ? "gray" : "#eeeeee"

            Behavior on color {
                ColorAnimation {

                }
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                maximumLineCount: 2
                id: messageTextWithNicknameTimestamp
                text: timestamp + " <" + nickname + "> " + message

                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
                font.pointSize: settings.baseFontSize

                MouseArea {
                    id: mouseNicknameTimestamp
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }
            }
        }
        Rectangle {
            clip: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter

            visible: !(nickname == "")
            height: visible ? 45 : 0
            color: mouseNickname.pressed ? "gray" : "#eeeeee"

            Behavior on color {
                ColorAnimation {

                }
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                maximumLineCount: 2
                id: messageTextWithNickname
                text: "<" + nickname + "> " + message

                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
                font.pointSize: settings.baseFontSize

                MouseArea {
                    id: mouseNickname
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }
            }
        }

        Rectangle {
            clip: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter

            visible: true
            height: 45
            color: mouseText.pressed ? "gray" : "#eeeeee"

            Behavior on color {
                ColorAnimation {

                }
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                maximumLineCount: 2
                id: messageText
                visible: true
                text: message

                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
                font.pointSize: settings.baseFontSize

                MouseArea {
                    id: mouseText
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }
            }
        }
    }
}


