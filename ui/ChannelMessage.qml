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
    height: messageRootLayout.height

    color: messageModel.highlight ? "#22880000" : "transparent"
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
        acceptedButtons: (mobilePlatform ? Qt.LeftButton : 0) | Qt.RightButton
        cursorShape: messageText.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.IBeamCursor
        onPressAndHold: {
            channelMessageActionMenu.show(messageModel.message.toPlain(),
                                          messageModel.nick,
                                          messageModel.date)
        }
    }
    RowLayout {
        id: messageRootLayout
        width: parent.width
        spacing: 0
        Text {
            Layout.alignment: Qt.AlignTop
            visible: lith.settings.timestampFormat.length > 0 && (messageModel.isJoinPartQuitMsg ? settings.showJoinPartQuitMessages : true)
            text: messageModel.date.toLocaleTimeString(Qt.locale(), lith.settings.timestampFormat) + "\u00A0"
            font.pointSize: settings.baseFontSize
            color: disabledPalette.text
            textFormat: Text.RichText
            renderType: Text.NativeRendering
        }
        Text {
            Layout.alignment: Qt.AlignTop
            font.bold: true
            text: messageModel.prefix.toTrimmedHtml(lith.settings.nickCutoffThreshold)
            font.pointSize: settings.baseFontSize
            color: palette.text
            textFormat: Text.RichText
            renderType: Text.NativeRendering
            visible: messageModel.isJoinPartQuitMsg ? settings.showJoinPartQuitMessages : true
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
            visible: messageModel.isJoinPartQuitMsg ? settings.showJoinPartQuitMessages : true
            onLinkActivated: {
                linkHandler.show(link, root)
            }
        }
    }
}
