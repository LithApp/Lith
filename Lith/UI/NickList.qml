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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core
import Lith.Style

Drawer {
    id: root
    SystemPalette {
        id: palette
    }

    signal requestSearchBar

    readonly property bool isClosed: position < 1

    onVisibleChanged: {
        if (visible) {
            if (platform.mobile) {
                Qt.inputMethod.hide()
                showKeyboardTimer.start()
            }
            else {
                nickFilter.forceActiveFocus()
            }

        }
        nickListView.currentIndex = 0
    }

    Timer {
        id: showKeyboardTimer
        interval: 1
        //onTriggered: Qt.inputMethod.show()
    }

    function openNickActionMenu(nick) {
        nickListActionMenu.visible = true
        nickListActionMenu.nickModel = nick
    }

    Rectangle {
        anchors.fill: parent
        color: palette.window
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 9
        visible: Lith.selectedBuffer

        Label {
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Label.AlignHCenter
            Layout.fillWidth: true
            text: Lith.selectedBuffer ? Lith.selectedBuffer.full_name : ""
            font.pointSize: FontSizes.medium
            color: palette.windowText
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Label.AlignHCenter
            Layout.fillWidth: true
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
            visible: Lith.selectedBuffer && Lith.selectedBuffer.isChannel
            text: Lith.selectedBuffer ? Lith.selectedBuffer.title : ""
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
            visible: Lith.selectedBuffer && Lith.selectedBuffer.isChannel
            text: Lith.selectedBuffer ? qsTr("%1 users, %2 voice, %3 ops (%4 total)").arg(Lith.selectedBuffer.normals).arg(Lith.selectedBuffer.voices).arg(Lith.selectedBuffer.ops).arg(Lith.selectedBuffer.normals + Lith.selectedBuffer.voices + Lith.selectedBuffer.ops) : ""
            font.pointSize: FontSizes.tiny
            color: palette.windowText
            opacity: 0.7
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        }

        GridLayout {
            visible: Lith.settings.showInternalData
            Layout.leftMargin: 6
            Layout.rightMargin: 6
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 1
            Label {
                Layout.fillWidth: true
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                font.pointSize: FontSizes.tiny
                text: "Number: " + (Lith.selectedBuffer ? Lith.selectedBuffer.number : "N/A")
            }
            Label {
                Layout.fillWidth: true
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                font.pointSize: FontSizes.tiny
                text: "Pointer: " + (Lith.selectedBuffer ? "0x" + Lith.selectedBuffer.ptr.toString(16) : "N/A")
            }
            Repeater {
                model: Lith.selectedBuffer ? Lith.selectedBuffer.local_variables_stringList : null
                Label {
                    Layout.fillWidth: true
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    font.pointSize: FontSizes.tiny
                    text: modelData
                }
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Search messages")
            onClicked: {
                root.requestSearchBar()
                root.close()
            }
        }

        Item {
            visible: Lith.selectedBuffer && Lith.selectedBuffer.isChannel
            Layout.fillWidth: true
            height: nickFilter.height
            TextField {
                id: nickFilter
                placeholderText: qsTr("Filter nicks")
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 3
                }
                onTextChanged: Lith.selectedBufferNicks.filterWord = text

                Keys.onPressed: (event) => {
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
            visible: Lith.selectedBuffer && Lith.selectedBuffer.isChannel
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: Lith.selectedBufferNicks
            currentIndex: 0

            delegate: ItemDelegate {
                width: ListView.view.width
                visible: modelData.visible && modelData.level === 0
                height: visible ? implicitHeight : 0
                text: (modelData.prefix === " " ? "" : modelData.prefix) + modelData.name
                onClicked: {
                    nickListView.currentIndex = index
                    openNickActionMenu(modelData)
                }
            }
        }
    }
}
