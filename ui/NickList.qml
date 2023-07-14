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
    id: root
    SystemPalette {
        id: palette
    }

    readonly property bool isClosed: position < 1

    onVisibleChanged: {
        if (visible) {
            if (window.platform.mobile)
                nickFilter.focus = false
            else
                nickFilter.focus = true
        }
        nickListView.currentIndex = 0
    }

    function openNickActionMenu(nick) {
        nickListActionMenu.visible = true
        nickListActionMenu.nickname = nick
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
            horizontalAlignment: Label.AlignHCenter
            Layout.fillWidth: true
            text: lith.selectedBuffer ? lith.selectedBuffer.full_name : ""
            size: Label.Medium
            color: palette.windowText
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Label.AlignHCenter
            Layout.fillWidth: true
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
            visible: lith.selectedBuffer && lith.selectedBuffer.isChannel
            text: lith.selectedBuffer ? lith.selectedBuffer.title : ""
            color: palette.windowText
            onLinkActivated: {
                linkHandler.show(link, this)
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }

        Label {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Label.AlignHCenter
            visible: lith.selectedBuffer && lith.selectedBuffer.isChannel
            text: lith.selectedBuffer ? qsTr("%1 users, %2 voice, %3 ops (%4 total)").arg(lith.selectedBuffer.normals).arg(lith.selectedBuffer.voices).arg(lith.selectedBuffer.ops).arg(lith.selectedBuffer.normals + lith.selectedBuffer.voices + lith.selectedBuffer.ops) : ""
            size: Label.Tiny
            color: palette.windowText
            opacity: 0.7
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        }

        Item {
            visible: lith.selectedBuffer && lith.selectedBuffer.isChannel
            Layout.fillWidth: true
            height: nickFilter.height
            TextField {
                id: nickFilter
                focus: false
                placeholderText: qsTr("Filter nicks")
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 3
                }
                onTextChanged: lith.selectedBufferNicks.filterWord = text

                Keys.onPressed: {
                    if (event.key === Qt.Key_Up) {
                        nickListView.currentIndex--;
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Down) {
                        nickListView.currentIndex++;
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        openNickActionMenu(nickListView.currentItem.nick.name)
                        nickFilter.text = ""
                    }
                }
            }
        }

        ListView {
            id: nickListView
            visible: lith.selectedBuffer && lith.selectedBuffer.isChannel
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: lith.selectedBufferNicks
            currentIndex: 0

            delegate: ItemDelegate {
                width: ListView.view.width
                visible: modelData.visible && modelData.level === 0
                height: visible ? implicitHeight : 0
                text: (modelData.prefix === " " ? "" : modelData.prefix) + modelData.name
                onClicked: {
                    nickListView.currentIndex = index
                    openNickActionMenu(modelData.colorlessName)
                }
            }
        }
    }
}
