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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ListView {
    id: listView
    TextMetrics {
        id: timeMetrics
        text: Qt.formatTime(new Date(), Locale.LongFormat)
        font.pointSize: settings.baseFontSize
    }

    ScrollHelper {
        flickable: parent
        reverse: true
        anchors.fill: parent
    }

    // TODO breaks time dragging from outside of the screen
    ScrollBar.vertical: ScrollBar {
        id: scrollBar
        hoverEnabled: true
        active: hovered || pressed
        rotation: 180
        orientation: Qt.Vertical
        parent: listView.parent
        //size: root.height / root.contentHeight
        anchors.top: listView.top
        anchors.right: listView.right
        anchors.bottom: listView.bottom
    }

    rotation: 180
    spacing: 0
    model: lith.selectedBuffer ? lith.selectedBuffer.lines : null
    delegate: ChannelMessage {
        messageModel: modelData
    }

    ChannelMessageActionMenu {
        id: channelMessageActionMenu
    }
    //reuseItems: true

    MouseArea {
        id: messageListMouse
        z: -1
        enabled: false
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        hoverEnabled: true
        drag.target: listView
        drag.axis: Drag.XAxis
        drag.maximumX: 0
        drag.minimumX: timeMetrics.width + 6
    }

    function fillTopOfList() {
        if (!lith.selectedBuffer)
            return
        if (yPosition + visibleArea.heightRatio > 0.65) {
            lith.selectedBuffer.fetchMoreLines()
        }
    }

    property real yPosition: visibleArea.yPosition
    onYPositionChanged: fillTopOfList()
    onContentHeightChanged: fillTopOfList()
    onModelChanged: fillTopOfList()

    Dialog {
        id: linkHandler
        z: 99999999
        width: parent.width
        height: linkHandlerLayout.height + 12
        anchors.centerIn: parent ? parent : listView
        visible: false
        padding: 0
        topPadding: 0

        property string currentLink
        property string currentExtension: lith.getLinkFileExtension(currentLink)
        property bool containsImage: currentExtension.endsWith("png") ||
                                     currentExtension.endsWith("jpg")
        property bool containsVideo: currentExtension.endsWith("avi") ||
                                     currentExtension.endsWith("mov") ||
                                     currentExtension.endsWith("mp4") ||
                                     currentExtension.endsWith("webm") ||
                                     currentExtension.endsWith("gif")

        onVisibleChanged: {
            if (!visible)
                parent = listView
        }

        function show(link, item) {
            visible = true
            parent = item
            currentLink = link
        }

        RowLayout {
            id: linkHandlerLayout
            y: 6
            x: 6
            width: parent.width - 12
            spacing: 9
            Text {
                id: linkText
                Layout.fillWidth: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: lith.settings.baseFontSize
                textFormat: Text.RichText
                text: "<a href=\""+linkHandler.currentLink+"\">"+linkHandler.currentLink+"</a>"
            }
            Button {
                focusPolicy: Qt.NoFocus
                font.pointSize: settings.baseFontSize
                Layout.preferredHeight: 36
                Layout.preferredWidth: height
                icon.source: "qrc:/navigation/copy.png"
                onClicked: {
                    clipboard.setText(linkHandler.currentLink)
                    linkHandler.visible = false
                }
            }
            Button {
                focusPolicy: Qt.NoFocus
                font.pointSize: settings.baseFontSize
                Layout.preferredHeight: 36
                Layout.preferredWidth: height
                onClicked: {
                    Qt.openUrlExternally(linkHandler.currentLink)
                    linkHandler.visible = false
                }
                icon.source: "qrc:/navigation/resize.png"
            }
            Button {
                visible: linkHandler.containsImage || linkHandler.containsVideo
                focusPolicy: Qt.NoFocus
                icon.source: "qrc:/navigation/image.png"
                font.pointSize: settings.baseFontSize
                Layout.preferredHeight: 36
                Layout.preferredWidth: height
                onClicked: {
                    if (linkHandler.containsImage)
                        previewPopup.showImage(linkHandler.currentLink)
                    if (linkHandler.containsVideo)
                        previewPopup.showVideo(linkHandler.currentLink)
                    linkHandler.visible = false
                }
            }
        }
        /*
        Rectangle {
            anchors.centerIn: parent
            z: -1
            width: 100000
            height: 100000
            color: "#44000000"
            MouseArea {
                anchors.fill: parent
                onClicked: linkHandler.visible = false
            }
        }
        */
    }
}
