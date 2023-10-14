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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core
import Lith.Style

/* Hold-to-copy action menu */

Dialog {
    id: channelMessageActionMenuDialog
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    width: parent.width / 1.25

    // Center the popup/dialog, whatever
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)


    property var lineModel
    readonly property string message: lineModel ? lineModel.message.toPlain() : ""
    readonly property string nickname: lineModel ? lineModel.nick : ""
    readonly property var timestamp: lineModel ? lineModel.date : null

    function show(model) {
        lineModel = model
        visible = true
    }

    header: Label {
        topPadding: 20
        text: qsTr("Copy")
        font.bold: true
        font.pixelSize: FontSizes.medium
        horizontalAlignment: Label.AlignHCenter
    }

    GridLayout {
        columns: window.WindowHelper.landscapeMode ? 2 : 1
        width: parent.width - 2
        x: 1

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 16

            Button {
                id: button1
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Layout.preferredHeight: 1.5 * implicitHeight
                Layout.topMargin: 16
                flat: true

                visible: !(nickname == "")
                text: timestamp ? timestamp.toLocaleTimeString(Qt.locale(), Lith.settings.timestampFormat) + " <" + nickname + "> " + message : ""

                onClicked: {
                    channelMessageActionMenuDialog.close()
                    ClipboardProxy.setText(button1.text)
                }
            }
            Button {
                id: button2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Layout.preferredHeight: 1.5 * implicitHeight
                flat: true

                visible: !(nickname == "")
                text: "<" + nickname + "> " + message

                onClicked: {
                    channelMessageActionMenuDialog.close()
                    ClipboardProxy.setText(button2.text)
                }
            }
            Button {
                id: button3
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Layout.preferredHeight: 1.5 * implicitHeight
                Layout.bottomMargin: 16
                flat: true
                text: message


                onClicked: {
                    channelMessageActionMenuDialog.close()
                    ClipboardProxy.setText(button3.text)
                }
            }
        }

        ColumnLayout {
            visible: Lith.settings.showInternalData
            Layout.fillWidth: true
            Layout.margins: 6
            Label {
                Layout.fillWidth: true
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: "Pointer: " + (lineModel ? "0x" + lineModel.ptr.toString(16) : "N/A")
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: "Date: " + (lineModel ? Qt.formatDateTime(lineModel.date, Qt.RFC2822Date) : "N/A")
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: "Higlight: " + (lineModel ? lineModel.highlight ? "true" : "false" : "N/A")
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: "Prefix: " + (lineModel ? lineModel.prefix.toHtml() : "N/A")
                textFormat: Label.PlainText
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: "Message: " + (lineModel ? lineModel.message.toHtml() : "N/A")
                textFormat: Label.PlainText
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: "Tags:"
            }

            Repeater {
                model: lineModel ? lineModel.tags_array : null
                Label {
                    Layout.fillWidth: true
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    text: " - " + modelData
                }
            }
        }
    }
}


