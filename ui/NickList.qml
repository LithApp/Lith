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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Drawer {
    SystemPalette {
        id: palette
    }

    onVisibleChanged: {
        if (visible) {
            if (Qt.platform.os === "android" || Qt.platform.os === "ios")
                nickFilter.focus = false
            else
                nickFilter.focus = true
        }
    }


    Rectangle {
        anchors.fill: parent
        color: palette.window
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 9
        visible: lith && lith.selectedBuffer

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: lith.selectedBuffer ? lith.selectedBuffer.full_name : ""
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize * 1.125
            color: palette.windowText
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
            horizontalAlignment: Label.AlignHCenter
            text: lith.selectedBuffer ? lith.selectedBuffer.title : ""
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize
            color: palette.windowText
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "0 users, 0 voice, 0 ops"
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize * 0.75
            color: palette.windowText
        }

        Item {
            Layout.fillWidth: true
            height: nickFilter.height
            TextField {
                id: nickFilter
                focus: false
                placeholderText: "Filter nicks"
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 3
                }
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1
            }
        }

        ListView {
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: lith.selectedBuffer ? lith.selectedBuffer.nicks : null

            delegate: Rectangle {
                width: ListView.view.width
                visible: modelData.visible && modelData.level === 0
                height: visible ? nickTextItem.height + 12 : 0
                color: nickItemMouse.pressed ? "gray" : palette.base

                MouseArea {
                    id: nickItemMouse
                    anchors.fill: parent
                    onClicked: {
                        console.warn("something, soon?");
                        nickListActionMenu.visible = true
                        nickListActionMenu.nickname = modelData.name
                    }
                }

                Behavior on color {
                    ColorAnimation {

                    }
                }

                RowLayout {
                    x: 3
                    y: 6
                    width: parent.width - 6
                    Text {
                        id: nickTextItem
                        clip: true
                        text: (modelData.prefix === " " ? "" : modelData.prefix) + modelData.name
                        font.family: "Menlo"
                        font.pointSize: settings.baseFontSize * 1.125
                        color: palette.windowText


                    }
                }
            }
        }
    }
}
