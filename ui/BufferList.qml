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

import Lith.Core

Rectangle {
    id: root
    color: palette.window

    readonly property real delegateWidth: root.width - (Lith.settings.scrollbarsOverlayContents ? 0 : scrollBar.width)

    readonly property bool controlRowOnBottom: (window.platform.mobile && Lith.settings.platformBufferControlPosition) || (!window.platform.mobile && !Lith.settings.platformBufferControlPosition)
    property alias currentIndex: bufferList.currentIndex
    function clear() {
        filterField.clear()
        if (window.platform.mobile)
            filterField.focus = false
        else
            filterField.forceActiveFocus()
    }
    signal close

    SystemPalette {
        id: palette
    }

    RowLayout {
        id: controlRow
        anchors {
            left: parent.left
            right: parent.right
            margins: 6
        }
        y: root.controlRowOnBottom ? parent.height - height - anchors.margins : anchors.margins

        spacing: 6

        Button {
            id: settingsButton
            Layout.preferredWidth: height
            flat: true
            icon.source: "qrc:/navigation/"+currentTheme+"/cogwheel.png"
            onClicked: settingsDialog.visible = true
            ToolTip.text: "Open settings"
            ToolTip.visible: settingsButton.hovered
            ToolTip.delay: 800
        }
        TextField {
            id: filterField
            Layout.fillWidth: true
            placeholderText: qsTr("Filter buffers")
            text: Lith.buffers.filterWord
            onTextChanged: {
                Lith.buffers.filterWord = text
                if (text === "")
                    bufferList.currentIndex = Lith.selectedBufferIndex
            }

            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Up) {
                    bufferList.currentIndex--;
                    event.accepted = true
                }
                if (event.key === Qt.Key_Down) {
                    bufferList.currentIndex++;
                    event.accepted = true
                }
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    Lith.selectedBuffer = bufferList.currentItem.buffer
                    filterField.text = ""
                    bufferList.currentIndex = Lith.selectedBufferIndex
                    root.close()
                }
                if (event.key === Qt.Key_Escape) {
                    filterField.text = ""
                    bufferList.currentIndex = Lith.selectedBufferIndex
                }
            }
        }
    }

    ListView {
        id: bufferList
        clip: true

        anchors {
            left: parent.left
            right: parent.right
            top: root.controlRowOnBottom ? parent.top : controlRow.bottom
            topMargin: root.controlRowOnBottom ? 0 : 9
        }
        height: parent.height - controlRow.height - 15

        model: Lith.buffers
        currentIndex: Lith.selectedBufferIndex
        highlightMoveDuration: root.position > 0.0 ? 120 : 0

        Connections {
            target: Lith
            function onSelectedBufferIndexChanged() {
                bufferList.currentIndex = Lith.selectedBufferIndex
            }
        }

        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            hoverEnabled: true
            active: hovered || pressed
            orientation: Qt.Vertical
        }

        delegate: ItemDelegate {
            id: bufferDelegate
            width: root.delegateWidth

            checked: Lith.selectedBuffer == buffer
            highlighted: bufferList.currentIndex === index

            required property var modelData
            required property int index
            property var buffer: modelData
            text: buffer ? buffer.short_name.toPlain() === "" ? buffer.name : buffer.short_name : ""

            onClicked: {
                Lith.selectedBuffer = buffer
                root.close()
            }
            onPressAndHold: {
                if (Lith.selectedBuffer === buffer) {
                    Lith.selectedBuffer = null
                    root.close()
                }
            }

            leftPadding: horizontalPadding + numberIndicator.width + spacing
            rightPadding: horizontalPadding + (hotMessageIndicator.visible ? hotMessageIndicator.width + spacing : 0)

            Rectangle {
                id: numberIndicator
                x: bufferDelegate.spacing
                height: parent.height - 12
                width: height
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 1
                color: buffer && buffer.number > 0 && buffer.number <= 10 && !buffer.isServer ? "#22000000" : "transparent"
                radius: 2
                Label {
                    text: buffer && buffer.number > 0 ? buffer.number : ""
                    size: Label.Small
                    anchors.centerIn: parent
                    color: disabledPalette.text
                    opacity: buffer && buffer.number > 0 && buffer.number <= 10 && !buffer.isServer ? 1.0 : 0.4
                }
                Behavior on opacity { NumberAnimation { duration: 100 } }
            }
            Rectangle {
                id: hotMessageIndicator
                visible: modelData.hotMessages > 0 || modelData.unreadMessages > 0
                color: modelData.hotMessages ? colorUtils.darken(palette.highlight, 1.3) : palette.alternateBase
                border.color: palette.text
                border.width: 1
                height: parent.height - 12
                width: Math.max(height, hotListItemCount.width + 6)
                anchors.verticalCenter: bufferDelegate.verticalCenter
                anchors.right: bufferDelegate.right
                anchors.rightMargin: bufferDelegate.spacing
                radius: 2
                Label {
                    id: hotListItemCount
                    text: modelData.hotMessages > 0 ? modelData.hotMessages : modelData.unreadMessages
                    anchors.centerIn: parent
                    color: modelData.hotMessages > 0 ? palette.highlightedText : palette.windowText
                }
            }
        }
    }

    Rectangle {
        id: separator
        anchors {
            left: parent.left
            right: parent.right
            bottom: root.controlRowOnBottom ? controlRow.top : controlRow.bottom
            bottomMargin: root.controlRowOnBottom ? 6 : -6
        }

        height: 1
        color: palette.text
        opacity: 0.5
    }
}
