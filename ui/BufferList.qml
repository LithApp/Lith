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

Drawer {
    id: root
    SystemPalette {
        id: palette
    }
    onVisibleChanged: {
        bufferList.currentIndex = lith.selectedBufferIndex

        if (visible) {
            filterField.clear()
            if (Qt.platform.os === "android" || Qt.platform.os === "ios")
                filterField.focus = false
            else
                filterField.focus = true
        }
    }

    Rectangle {
        anchors.fill: parent
        color: palette.window

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 3

            RowLayout {
                Layout.fillWidth: true

                Item {
                    width: 6
                }
                Text {
                    font.pointSize: settings.baseFontSize * 1.5
                    text: "Lith"
                    color: palette.windowText
                }
                Item {
                    Layout.fillWidth: true
                }

                Button {
                    Layout.preferredWidth: height
                    font.pointSize: settings.baseFontSize * 1.25
                    icon.source: "qrc:/navigation/screwdriver.png"
                    onClicked: dataBrowser.open()
                }

                Button {
                    Layout.preferredWidth: height
                    font.pointSize: settings.baseFontSize * 1.25
                    icon.source: "qrc:/navigation/cogwheel.png"
                    onClicked: settingsDialog.visible = true
                }
                Item {
                    width: 6
                }
            }

            Item {
                height: 1
            }
            TextField {
                id: filterField
                Layout.fillWidth: true
                placeholderText: qsTr("Filter buffers")
                text: lith.buffers.filterWord
                onTextChanged: lith.buffers.filterWord = text
                font.pointSize: settings.baseFontSize
                color: "black"
                placeholderTextColor: "gray"

                Keys.onPressed: {
                    if (event.key === Qt.Key_Up) {
                        bufferList.currentIndex--;
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Down) {
                        bufferList.currentIndex++;
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        lith.selectedBuffer = bufferList.currentItem.buffer
                        filterField.text = ""
                        root.close()
                    }
                }
                /*
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        lith.selectedBufferIndex = -1
                        bufferDrawer.close()
                    }
                }
                */
            }
            Rectangle {
                height: 1
                Layout.fillWidth: true
                color: "dark gray"
            }


            ListView {
                id: bufferList
                clip: true
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: lith.buffers
                currentIndex: 0
                highlightMoveDuration: 120

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                    hoverEnabled: true
                    active: hovered || pressed
                    orientation: Qt.Vertical
                }

                delegate: Rectangle {
                    width: ListView.view.width
                    height: childrenRect.height + 12
                    property var buffer: modelData
                    visible: buffer
                    color: index == bufferList.currentIndex ? "#bb6666" : bufferMouse.pressed ? "gray" : palette.base

                    Behavior on color {
                        ColorAnimation {

                        }
                    }

                    RowLayout {
                        x: 3
                        y: 6
                        width: parent.width - 6

                        Rectangle {
                            width: bufferName.height + 6
                            height: width
                            color: "#22000000"
                            opacity: index < 10 && filterField.text.length == 0 ? 0.5 : 0.0
                            radius: 2
                            Text {
                                text: index + 1
                                anchors.centerIn: parent
                            }
                            Behavior on opacity { NumberAnimation { duration: 100 } }
                        }

                        Text {
                            id: bufferName
                            Layout.fillWidth: true
                            clip: true
                            text: buffer ? buffer.short_name === "" ? buffer.name : buffer.short_name
                                         : ""
                            font.pointSize: settings.baseFontSize * 1.125
                            color: palette.windowText
                            MouseArea {
                                id: bufferMouse
                                anchors.fill: parent
                                onClicked: {
                                    lith.selectedBuffer = buffer
                                    bufferDrawer.visible = false
                                }
                            }
                        }
                        Rectangle {
                            visible: modelData.hotMessages > 0 || modelData.unreadMessages > 0
                            color: modelData.hotMessages ? "red" : palette.alternateBase
                            border.color: palette.text
                            border.width: 1
                            height: bufferName.height + 6
                            width: height
                            radius: 2
                            Text {
                                text: modelData.hotMessages > 0 ? modelData.hotMessages : modelData.unreadMessages
                                font.pointSize: settings.baseFontSize
                                anchors.centerIn: parent
                                color: palette.windowText
                            }
                        }
                    }
                }
            }
        }
    }
}
