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

Item {
    id: root

    // This will be used when the control row is at the top to make it same height as the channel header
    required property real channelHeaderRowHeight

    readonly property real delegateWidth: root.width - (Lith.settings.scrollbarsOverlayContents ? 0 : scrollBar.width)

    readonly property real controlRowHeight: controlRow.height
    readonly property bool controlRowOnBottom: (LithPlatform.mobile && Lith.settings.platformBufferControlPosition) || (!LithPlatform.mobile && !Lith.settings.platformBufferControlPosition)
    property alias currentIndex: bufferList.currentIndex
    function clear() {
        filterField.clear()
        if (LithPlatform.mobile)
            filterField.focus = false
        else
            filterField.forceActiveFocus()
    }
    signal close

    Rectangle {
        anchors.fill: parent
        z: -3
        color: LithPalette.regular.window
    }

    Rectangle {
        width: controlRow.width + controlRow.anchors.margins * 2
        height: controlRow.height
        y: controlRow.y
        anchors.margins: -controlRow.margins
        color: LithPalette.regular.window
        z: 1
    }

    RowLayout {
        id: controlRow
        anchors {
            left: parent.left
            right: parent.right
            margins: 6
        }
        z: 2
        y: root.controlRowOnBottom ? parent.height - height : 0
        height: root.controlRowOnBottom ? implicitHeight : Math.max(channelHeaderRowHeight, implicitHeight)
        //y: root.controlRowOnBottom ? implicitHeight :

        Button {
            id: settingsButton
            Layout.preferredWidth: height
            flat: true
            icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/cogwheel.png"
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
                    bufferList.currentIndex = Lith.mappedSelectedBufferIndex
            }

            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Up) {
                    bufferList.currentIndex--;
                    if (bufferList.currentIndex < -1)
                        bufferList.currentIndex = -1
                    event.accepted = true
                }
                if (event.key === Qt.Key_Down) {
                    bufferList.currentIndex++
                    if (bufferList.currentIndex >= bufferList.count)
                        bufferList.currentIndex = bufferList.count - 1
                    event.accepted = true
                }
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    Lith.selectedBuffer = bufferList.currentItem.buffer
                    filterField.text = ""
                    bufferList.currentIndex = Lith.mappedSelectedBufferIndex
                    root.close()
                }
                if (event.key === Qt.Key_Escape) {
                    filterField.text = ""
                    bufferList.currentIndex = Lith.mappedSelectedBufferIndex
                }
            }
        }
    }

    Rectangle {
        id: separator
        z: 3
        anchors {
            left: parent.left
            right: parent.right
            bottom: root.controlRowOnBottom ? controlRow.top : controlRow.bottom
        }

        height: 1
        color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.5)
    }

    Item {
        id: bufferListStackingOrderWrapper
        z: -1

        anchors {
            left: parent.left
            right: parent.right
            top: root.controlRowOnBottom ? parent.top : controlRow.bottom
        }
        height: parent.height - controlRow.height - 2

        ListView {
            id: bufferList
            anchors.fill: parent

            model: Lith.buffers
            currentIndex: Lith.mappedSelectedBufferIndex
            highlightMoveDuration: root.position > 0.0 ? 120 : 0

            Connections {
                target: Lith
                function onSelectedBufferIndexChanged() {
                    bufferList.currentIndex = Lith.mappedSelectedBufferIndex
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
                highlighted: bufferList.currentItem ? bufferList.currentItem.index == index : false

                required property var modelData
                required property int index
                property var buffer: modelData
                text: buffer ? buffer.short_name.toPlain() === "" ? buffer.name : buffer.short_name : ""
                textFormat: Text.RichText

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
                    visible: !buffer.isServer
                    x: bufferDelegate.spacing
                    height: parent.height - 12
                    width: height
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 1
                    color: buffer && buffer.number > 0 && buffer.number <= 10 && !buffer.isServer ? ColorUtils.mixColors(LithPalette.regular.text, bufferDelegate.background.color, 0.05) : bufferDelegate.background.color
                    radius: 2
                    Label {
                        text: buffer && buffer.number > 0 ? buffer.number : ""
                        font.pixelSize: FontSizes.small
                        anchors.centerIn: parent
                        color: buffer && buffer.number > 0 && buffer.number <= 10 && !buffer.isServer ? LithPalette.regular.text : LithPalette.disabled.text
                    }
                }
                Rectangle {
                    id: hotMessageIndicator
                    visible: modelData.hotMessages > 0 || modelData.unreadMessages > 0
                    color: modelData.hotMessages ? ColorUtils.darken(LithPalette.regular.highlight, 1.3) : LithPalette.regular.alternateBase
                    border.color: LithPalette.regular.text
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
                        color: modelData.hotMessages > 0 ? LithPalette.regular.highlightedText : LithPalette.regular.windowText
                    }
                }
            }
        }
    }
}
