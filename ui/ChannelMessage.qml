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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

import lith 1.0

Rectangle {
    id: root
    z: index
    width: ListView.view.width // + timeMetrics.width
    property var messageModel: null
    //property var previousMessageModel: ListView.view.contentItem.children[index-1].messageModel
    //property var nextMessageModel: ListView.view.contentItem.children[index+1].messageModel

    height: lith.settings.terminalLikeChat ? terminalLineLayout.height : messageBubble.height

    color: messageModel.highlight ? "#44aa3333" : "transparent"
    Connections {
        target: messageMouseArea
        function onClicked(mouse) {
            if (mouse.button === Qt.RightButton) {
                channelMessageActionMenu.show(messageModel.message.toPlain(),
                                              messageModel.nick,
                                              messageModel.date)
            }
        }
    }

    MouseArea {
        id: messageMouseArea
        anchors.fill: parent
        pressAndHoldInterval: 400 // does this do anything? or is it just for signals?
        hoverEnabled: true
        acceptedButtons: (window.platform.mobile ? Qt.LeftButton : 0) | Qt.RightButton
        cursorShape: messageText.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.IBeamCursor
        onPressAndHold: {
            channelMessageActionMenu.show(messageModel.message.toPlain(),
                                          messageModel.nick,
                                          messageModel.date)
        }
    }

    Item {
        id: messageBubble
        visible: !lith.settings.terminalLikeChat
        width: messageBubbleText.width + 30
        height: messageBubbleText.height + 30

        Rectangle {
            visible: {
                if (messageModel.isSelfMsg)
                    return false
                /*
                if (nextMessageModel.nick == messageModel.nick)
                    return false
                */
                return true
            }
            x: 3
            y: 3
            color: palette.dark
            width: 36
            height: 36
            radius: height / 2
            Text {
                anchors.centerIn: parent
                text: messageModel.colorlessNickname.substring(0, 2)
                color: palette.window
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
            Text {
                id: messageBubbleText
                x: 12
                y: 12
                width: Math.min(implicitWidth, root.width - 75)
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: messageModel.message
                color: palette.text
                textFormat: Text.RichText
                renderType: Text.NativeRendering
            }
        }
    }
    RowLayout {
        id: terminalLineLayout
        visible: lith.settings.terminalLikeChat
        width: parent.width
        spacing: 0
        Text {
            Layout.alignment: Qt.AlignTop
            text: messageModel.date.toLocaleTimeString(Qt.locale(), lith.settings.timestampFormat) + "\u00A0"
            font.pointSize: settings.baseFontSize
            color: disabledPalette.text
            textFormat: Text.RichText
            renderType: Text.NativeRendering
        }
        Text {
            Layout.alignment: Qt.AlignTop
            font.bold: true
            visible: lith.settings.nickCutoffThreshold !== 0
            text: messageModel.prefix.toTrimmedHtml(lith.settings.nickCutoffThreshold) + "\u00A0"
            font.pointSize: settings.baseFontSize
            color: palette.text
            textFormat: Text.RichText
            renderType: Text.NativeRendering
        }

        Text {
            id: messageText
            text: messageModel.message
            Layout.fillWidth: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: palette.text
            font.pointSize: settings.baseFontSize
            textFormat: Text.RichText
            renderType: Text.NativeRendering
            onLinkActivated: {
                linkHandler.show(link, root)
            }
        }
    }
}
