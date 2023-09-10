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
import QtQuick.Layouts
import QtQuick.Controls

import Lith.Core
import Lith.Style

Rectangle {
    id: root
    color: LithPalette.regular.window

    height: headerLayout.height + 12

    DragHandler {
        onActiveChanged: if (active) window.startSystemMove();
        target: null
    }

    Rectangle {
        anchors {
            left: parent.left
            leftMargin: 1
            right: parent.right
            bottom: parent.bottom
        }
        height: 1
        color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.5)
    }

    RowLayout {
        id: headerLayout
        x: 6
        y: 6
        width: parent.width - 12
        Button {
            id: bufferListButton
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: implicitHeight
            flat: true
            icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/menu.png"
            onClicked: {
                bufferDrawer.toggle()
            }
            ToolTip.text: "Open buffer list,\nAccess configuration"
            ToolTip.visible: bufferListButton.hovered
            ToolTip.delay: 800
        }
        Item { width: 1 }
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0
                Item {
                    // Wrapper around the title so recording info can be placed next to it and title still stays in center
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    implicitWidth: bufferNameLabel.implicitWidth
                    implicitHeight: bufferNameLabel.implicitHeight

                    Label {
                        id: bufferNameLabel
                        // First part is calculated as half of the area the parent item got, second part is putting the label to the right side of its parent (when space runs out)
                        x: Math.min((parent.width - width + replayInfoLayout.width + 1) / 2, parent.width - width - 6)
                        color: LithPalette.regular.text
                        font.bold: true
                        text: Lith.selectedBuffer ? Lith.selectedBuffer.name : "Lith"
                        anchors.verticalCenter: parent.verticalCenter
                        width: Math.min(implicitWidth, parent.width)
                        elide: Label.ElideRight
                    }
                }

                RowLayout {
                    id: replayInfoLayout
                    visible: Lith.networkProxy.recording || Lith.status === Lith.REPLAY
                    Layout.alignment: Qt.AlignRight
                    Label {
                        font.pointSize: FontSizes.tiny
                        Layout.alignment: Qt.AlignVCenter
                        text: {
                            if (Lith.networkProxy.recording)
                                return ("Recording into slot %1").arg(Lith.networkProxy.slot)
                            else if (Lith.networkProxy.replaying)
                                return ("Replaying event %1/%2").arg(Lith.networkProxy.currentEvent).arg(Lith.networkProxy.totalEvents)
                            else if (Lith.status === Lith.REPLAY)
                                return ("Replayed %1 events").arg(Lith.networkProxy.totalEvents)
                            else
                                return ""
                        }
                        SequentialAnimation on color {
                            id: breathingAnimation
                            running: Lith.networkProxy.recording
                            loops: Animation.Infinite
                            property color firstColor: LithPalette.regular.text
                            onFirstColorChanged: breathingAnimation.restart()
                            property color secondColor: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.highlight, 0.5)
                            onSecondColorChanged: breathingAnimation.restart()
                            property real animationDuration: 3000
                            ColorAnimation { from: breathingAnimation.firstColor; to: breathingAnimation.secondColor; duration: breathingAnimation.animationDuration }
                            ColorAnimation { from: breathingAnimation.secondColor; to: breathingAnimation.firstColor; duration: breathingAnimation.animationDuration }
                        }
                    }
                    Rectangle {
                        color: "red"
                        Layout.alignment: Qt.AlignVCenter
                        visible: Lith.networkProxy.recording

                        width: Lith.settings.baseFontSize * 1.25
                        height: width
                        radius: width / 2
                    }
                }
            }
            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: LithPalette.regular.text
                visible: !Lith.selectedBuffer || Lith.selectedBuffer.title.length > 0
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: Lith.selectedBuffer ? Lith.selectedBuffer.title.toPlain() :
                      Lith.status === Lith.UNCONFIGURED ? "Not configured" :
                      Lith.status === Lith.CONNECTING ? "Connecting" :
                      Lith.status === Lith.CONNECTED ? "Connected" :
                      Lith.status === Lith.DISCONNECTED ? "Disconnected" :
                      Lith.status === Lith.ERROR ? "Error: " + Lith.errorString :
                      Lith.status === Lith.REPLAY ? "Replaying a recording" :
                                                   ""
                elide: Text.ElideRight
                maximumLineCount: 2
                font.pointSize: FontSizes.tiny
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                onLinkActivated: {
                    linkHandler.show(link, root)
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
        Item { width: 1 }
        Button {
            id: nickListButton
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: height
            flat: true
            onClicked: {
                nickDrawer.visible = !nickDrawer.visible
                if(!LithPlatform.mobile) nickDrawer.open()
            }
            icon.source: Lith.status === Lith.UNCONFIGURED ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/sleeping.png" :
                         Lith.status === Lith.CONNECTING   ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/transfer.png" :
                         Lith.status === Lith.CONNECTED    ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/smile.png" :
                         Lith.status === Lith.DISCONNECTED ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/no-wifi.png" :
                         Lith.status === Lith.ERROR        ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/sleeping.png" :
                         Lith.status === Lith.REPLAY       ? (Lith.networkProxy.replaying ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/play-color.png" : "qrc:/navigation/"+WindowHelper.currentThemeName+"/pause-color.png") :
                                                             "qrc:/navigation/"+WindowHelper.currentThemeName+"/dizzy.png"
            ToolTip.text: "Open channel information"
            ToolTip.visible: nickListButton.hovered
            ToolTip.delay: 800
        }
    }
}
