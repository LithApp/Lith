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
import QtQuick.Layouts
import QtQuick.Controls

import Lith.Core
import Lith.Style

Item {
    id: root
    z: index
    property int index
    property var messageModel: null
    //property var previousMessageModel: ListView.view.contentItem.children[index-1].messageModel
    //property var nextMessageModel: ListView.view.contentItem.children[index+1].messageModel

    height: (Lith.settings.terminalLikeChat ? terminalLineLayout.height : messageBubble.height) + (headerLabel.visible ? headerLabel.height : 0)

    opacity: messageModel.searchCompare(Lith.search.term) ? 1.0 : 0.3

    property alias header: headerLabel.text
    readonly property bool isHighlighted: Lith.search.highlightedLine && messageModel && Lith.search.highlightedLine === messageModel

    Connections {
        target: messageMouseArea
        function onClicked(mouse) {
            if (mouse.button === Qt.RightButton) {
                channelMessageActionMenu.show(messageModel)
            }
        }
    }

    MouseArea {
        id: messageMouseArea
        anchors.fill: parent
        pressAndHoldInterval: 400 // does this do anything? or is it just for signals?
        hoverEnabled: true
        acceptedButtons: (LithPlatform.mobile ? Qt.LeftButton : 0) | Qt.RightButton
        cursorShape: messageText.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
        onPressAndHold: {
            channelMessageActionMenu.show(messageModel)
        }
    }

    /* Disabled until this feature gets completed
    Item {
        id: messageBubble
        visible: !Lith.settings.terminalLikeChat
        width: messageBubbleText.width + 30
        height: messageBubbleText.height + 30

        Rectangle {
            visible: {
                if (messageModel.isSelfMsg)
                    return false
                // if (nextMessageModel.nick == messageModel.nick)
                //   return false
                return true
            }
            x: 3
            y: 3
            color: LithPalette.regular.dark
            width: 36
            height: 36
            radius: height / 2
            Label {
                anchors.centerIn: parent
                text: messageModel.colorlessNickname.substring(0, 2)
                color: LithPalette.regular.window
                textFormat: Text.RichText
                renderType: Text.NativeRendering
            }
        }

        Rectangle {
            x: messageModel.isSelfMsg ? root.width - messageBubble.width : 46
            y: 3
            radius: 24
            width: messageBubbleText.width + 24
            height: messageBubbleText.height + 24
            antialiasing: true
            property color origColor: messageModel.nickColor
            function noir(col) {
                let newColor = col
                newColor.hslLightness *= 0.6
                return newColor
            }
            color: noir(origColor)
            Label {
                id: messageBubbleText
                x: 12
                y: 12
                width: Math.min(implicitWidth, root.width - 75)
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: messageModel.message
                color: LithPalette.regular.text
                textFormat: Text.RichText
                renderType: Text.NativeRendering
            }
        }
    }
    */

    Label {
        id: headerLabel
        visible: text.length > 0
        width: parent.width
        horizontalAlignment: Label.AlignHCenter
        topPadding: 3
        bottomPadding: 3
        Rectangle {
            anchors.fill: parent
            z: -1
            height: 1
            color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.1)
        }
    }

    Rectangle {
        anchors.fill: terminalLineLayout
        color: messageModel.highlight ? ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.window, 0.5) : isHighlighted ? ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window,  0.1) : "transparent"
        border {
            color: LithPalette.regular.highlight
            width: root.isHighlighted ? 1.5 : 0
        }
    }

    ColumnLayout {
        id: terminalLineLayout
        visible: Lith.settings.terminalLikeChat
        width: parent.width
        y: headerLabel.visible ? headerLabel.height + 1 : 1
        spacing: 0
        RowLayout {
            Layout.fillWidth: true
            spacing: 0
            Label {
                Layout.alignment: Qt.AlignTop
                text: messageModel.date.toLocaleString(Qt.locale(), Lith.settings.timestampFormat) + "\u00A0"
                color: LithPalette.disabled.text
                textFormat: Text.RichText
                renderType: Text.NativeRendering
                lineHeight: messageText.lineHeight
                lineHeightMode: messageText.lineHeightMode
            }
            Label {
                id: nickLabel
                Layout.alignment: Qt.AlignTop
                font.bold: true
                visible: Lith.settings.nickCutoffThreshold !== 0
                text: messageModel.prefix.toTrimmedHtml(Lith.settings.nickCutoffThreshold) + "\u00A0"
                color: LithPalette.regular.text
                textFormat: Text.RichText
                renderType: Text.NativeRendering
                lineHeight: messageText.lineHeight
                lineHeightMode: messageText.lineHeightMode
                verticalAlignment: Label.AlignVCenter
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Label {
                    id: messageText
                    text: messageModel.message
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: LithPalette.regular.text
                    textFormat: Text.RichText
                    renderType: Text.NativeRendering
                    lineHeight: font.pixelSize + 1
                    lineHeightMode: Label.FixedHeight
                    onLinkActivated: (link) => {
                        linkHandler.show(link, root)
                    }
                }

                Flow {
                    id: thumbnailLayout
                    Layout.fillWidth: true
                    spacing: 6
                    visible: Lith.settings.showImageThumbnails
                    Repeater {
                        model: Lith.settings.showImageThumbnails ? messageModel.message.urls : null
                        ChannelMessageThumbnail {
                            thumbnailUrl: modelData
                            maximumWidth: thumbnailLayout.width
                        }
                    }
                }
            }
        }
    }
}
