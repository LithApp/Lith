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
    rotation: 180
    height: messageRootLayout.height

    color: messageModel.highlight ? "#22880000" : "transparent"

    SystemPalette {
        id: palette
    }
    RowLayout {
        id: messageRootLayout
        width: parent.width
        spacing: 0
        TextEdit {
            Layout.alignment: Qt.AlignTop
            visible: lith.settings.timestampFormat.length > 0
            text: messageModel.date.toLocaleTimeString(Qt.locale(), lith.settings.timestampFormat) + "\u00A0"
            font.pointSize: settings.baseFontSize
            color: disabledPalette.text
            textFormat: Text.RichText
            renderType: Text.NativeRendering
            selectByMouse: true
            persistentSelection: true
            readOnly: true
        }
        TextEdit {
            Layout.alignment: Qt.AlignTop
            font.bold: true
            text: {
                // ugh
                if (lith.settings.nickCutoffThreshold < 0)
                    return messageModel.prefix + "\u00A0"
                var attr = messageModel.nickAttribute
                var attrL = attr.length
                var attrC = messageModel.nickAttributeColor
                var nick = messageModel.nick
                var nickC = messageModel.nickColor
                if (nickC + attrC == 0)
                    return ""
                var nickL = lith.settings.nickCutoffThreshold - attrL
                var nickS = nick.substring(0, nickL)
                var attrS = attr.padStart(lith.settings.nickCutoffThreshold - nickS.length, "\u00A0")
                // TODO color
                var suffix = attrL + nick.length <= lith.settings.nickCutoffThreshold ? "\u00A0" : "<font color=\""+disabledPalette.text+"\">+</font>"
                return "<font color=\"" + attrC + "\">" + attrS + "</font>" +
                        "<font color=\"" + nickC + "\">" + nickS.replace("<", "&lt;") + "</font>" +
                        suffix
            }
            font.pointSize: settings.baseFontSize
            color: palette.text
            textFormat: Text.RichText
            renderType: Text.NativeRendering
            selectByMouse: true
            persistentSelection: true
            readOnly: true
        }

        TextEdit {
            text: "<span style='white-space: pre-wrap;'>" + messageModel.message + "</span>"
            Layout.fillWidth: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: palette.text
            font.pointSize: settings.baseFontSize
            textFormat: Text.RichText
            renderType: Text.NativeRendering
            onLinkActivated: {
                linkHandler.show(link, root)
            }
            selectByMouse: true
            persistentSelection: true
            readOnly: true
            MouseArea {
                id: linkHoverMouse
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        pressAndHoldInterval: 400 // does this do anything? or is it just for signals?
        hoverEnabled: false
        acceptedButtons: (mobilePlatform ? Qt.LeftButton : 0) | Qt.RightButton
        cursorShape: Qt.IBeamCursor
        onPressAndHold: {
            channelMessageActionMenu.show(messageModel.colorlessText,
                                          messageModel.colorlessNickname,
                                          messageModel.date)
            mouse.accepted = true
        }
        onClicked: {
            if (mouse.button === Qt.RightButton) {
                channelMessageActionMenu.show(messageModel.colorlessText,
                                              messageModel.colorlessNickname,
                                              messageModel.date)
                mouse.accepted = true
            }
        }
    }
}
