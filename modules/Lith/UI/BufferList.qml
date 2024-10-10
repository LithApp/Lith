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

import "util" as Util

Item {
    id: root

    // This will be used when the control row is at the top to make it same height as the channel header
    required property real channelHeaderRowContentHeight
    required property real channelInputBarContentHeight

    readonly property real delegateWidth: bufferList.width - (Lith.settings.scrollbarsOverlayContents ? 0 : scrollBar.width)

    readonly property real controlRowHeight: controlRow.height
    readonly property bool controlRowOnBottom: (LithPlatform.mobile && Lith.settings.platformBufferControlPosition) || (!LithPlatform.mobile && !Lith.settings.platformBufferControlPosition)
    property alias currentIndex: bufferList.currentIndex

    function enforceFocus() {
        filterField.forceActiveFocus()
    }

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

    Util.ControlPanel {
        x: controlRow.x - Lith.settings.uiMargins - 1
        y: root.controlRowOnBottom ? parent.height - height - Lith.settings.uiMargins : controlRow.y + (controlRow.height - height) / 2
        width: controlRow.width + 2 * Lith.settings.uiMargins + 3// + controlRow.anchors.margins * 2
        height: root.controlRowOnBottom ? root.channelInputBarContentHeight + Lith.settings.uiMargins : 0 + controlRow.height
        radius: Math.pow(Lith.settings.uiMargins, 0.9)
        z: 1
    }

    RowLayout {
        id: controlRow
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 2 * Lith.settings.uiMargins + 1
            rightMargin: anchors.leftMargin + 2
        }
        z: 2
        y: root.controlRowOnBottom ? parent.height - height - 1.5 * Lith.settings.uiMargins : Lith.settings.uiMargins
        height: root.controlRowOnBottom ? implicitHeight : Math.max(channelHeaderRowContentHeight, implicitHeight)
        spacing: Lith.settings.uiMargins / 4 + 1

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
            property bool interactedWith: false
            onActiveFocusChanged: {
                interactedWith = false

                if (activeFocus) {
                    bufferList.currentIndex = Lith.mappedSelectedBufferIndex
                }
                else {
                    bufferList.currentIndex = -1
                }
            }
            onTextChanged: {
                interactedWith = true
                Lith.buffers.filterWord = text
                if (text === "")
                    bufferList.currentIndex = Lith.mappedSelectedBufferIndex
            }

            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Up) {
                    interactedWith = true
                    bufferList.currentIndex--;
                    if (bufferList.currentIndex < 0)
                        bufferList.currentIndex = 0
                    event.accepted = true
                }
                if (event.key === Qt.Key_Down) {
                    interactedWith = true
                    bufferList.currentIndex++
                    if (bufferList.currentIndex >= bufferList.count)
                        bufferList.currentIndex = bufferList.count - 1
                    event.accepted = true
                }
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    interactedWith = true
                    Lith.selectedBuffer = bufferList.currentItem.buffer
                    filterField.text = ""
                    bufferList.currentIndex = Lith.mappedSelectedBufferIndex
                    root.close()
                }
                if (event.key === Qt.Key_Escape) {
                    interactedWith = false
                    filterField.text = ""
                    bufferList.currentIndex = Lith.mappedSelectedBufferIndex
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
            leftMargin: Lith.settings.uiMargins
            rightMargin: anchors.leftMargin + 1
            topMargin: root.controlRowOnBottom ? Lith.settings.uiMargins : Lith.settings.uiMargins
            bottomMargin: Lith.settings.uiMargins
            top: root.controlRowOnBottom ? parent.top : controlRow.bottom
        }
        height: parent.height - controlRow.height - 5 - 3 * Lith.settings.uiMargins

        ListView {
            id: bufferList
            anchors.fill: parent

            model: Lith.buffers
            currentIndex: filterField.activeFocus ? Lith.mappedSelectedBufferIndex : -1
            highlightMoveDuration: root.position > 0.0 ? 120 : 0
            spacing: 0

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

            section.property: Lith.settings.bufferListGroupingByServer ? "server" : ""
            section.criteria: ViewSection.FullString
            section.delegate: Rectangle {
                id: sectionDelegate
                color: ColorUtils.mixColors(LithPalette.regular.base, LithPalette.regular.window, 0.5)
                required property string section
                readonly property bool showSection: {
                    if (Lith.buffers.filterWord.length <= 0)
                        return false
                    if (section.toLowerCase().indexOf(Lith.buffers.filterWord.toLowerCase()) >= 0)
                        return false
                    return true
                }

                width: root.delegateWidth
                height: {
                    if (showSection)
                        return implicitHeight
                    return 16
                }
                implicitHeight: sectionLabel.implicitHeight + sectionLabel.anchors.topMargin + sectionLabel.anchors.bottomMargin - 2

                Label {
                    id: sectionLabel
                    visible: sectionDelegate.showSection
                    anchors.fill: parent
                    anchors.topMargin: 3
                    anchors.bottomMargin: 3
                    anchors.rightMargin: 9
                    anchors.leftMargin: 9
                    text: section
                    verticalAlignment: Label.AlignVCenter
                    color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.6)
                    font.pixelSize: Lith.settings.baseFontPixelSize * 0.75
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 6
                    anchors.rightMargin: 6
                    anchors.bottomMargin: 0
                    height: 1
                    color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.4)
                }
            }

            delegate: ItemDelegate {
                id: bufferDelegate
                width: root.delegateWidth
                height: implicitHeight

                checked: Lith.selectedBuffer == buffer
                highlighted: filterField.interactedWith && bufferList.currentIndex === index

                required property var modelData
                required property int index
                property var buffer: modelData
                text: buffer ? buffer.short_name.toPlain() === "" ? buffer.name.toPlain() : buffer.short_name.toPlain() : ""
                textFormat: Text.PlainText
                font.bold: Lith.settings.bufferListGroupingByServer && buffer.isServer
                font.pixelSize: buffer && buffer.isServer ? Lith.settings.baseFontPixelSize /* * 1.125 */ : Lith.settings.baseFontPixelSize

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

                leftPadding: horizontalPadding + (buffer.isServer && Lith.settings.bufferListGroupingByServer ? 0 : numberIndicator.width + spacing)
                rightPadding: horizontalPadding + (hotMessageIndicator.visible ? hotMessageIndicator.width + spacing : 0)

                verticalPadding: 10

                Rectangle {
                    id: numberIndicator

                    readonly property bool firstTenBuffers: buffer && buffer.number > 0 && buffer.number <= 10 && !buffer.isServer

                    visible: !buffer.isServer
                    x: bufferDelegate.horizontalPadding
                    height: parent.height - (bufferDelegate.topPadding + bufferDelegate.bottomPadding) / 2 - 1
                    width: height
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0.5
                    color: buffer && buffer.number > 0 && buffer.number <= 10 && !buffer.isServer ? ColorUtils.mixColors(LithPalette.regular.text, bufferDelegate.background.color, 0.05) : bufferDelegate.background.color
                    radius: 2
                    Label {
                        text: buffer && buffer.number > 0 ? buffer.number : ""
                        font.pixelSize: FontSizes.small
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: 0.5
                        anchors.horizontalCenterOffset: -0.5
                        color: buffer && buffer.number > 0 && buffer.number <= 10 && !buffer.isServer ? LithPalette.regular.text : LithPalette.disabled.text
                    }
                }

                Rectangle {
                    id: hotMessageIndicator
                    visible: modelData.hotMessages > 0 || modelData.unreadMessages > 0
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: modelData.hotMessages ? ColorUtils.darken(LithPalette.regular.highlight, 1.3) : ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.text, 0.4)
                        }
                        GradientStop {
                            position: 1.0
                            color: "red"
                        }
                    }

                    border.color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.button, 0.1)
                    border.width: 1
                    height: parent.height - 12
                    width: Math.max(height, hotListItemCount.width + 6)
                    anchors.verticalCenter: bufferDelegate.verticalCenter
                    anchors.right: bufferDelegate.right
                    anchors.rightMargin: bufferDelegate.horizontalPadding + (Lith.settings.bufferListGroupingByServer ? 1 : 0)
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
