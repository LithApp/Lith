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

    height: (Lith.settings.terminalLikeChat ? Math.max(dateAndPrefixLabel.height, messageText.height) : messageBubble.height) + (headerLabel.visible ? headerLabel.height : 0) + (previewListView.visible ? previewListView.height : 0)

    opacity: messageModel.searchCompare(Lith.search.term) ? 1.0 : 0.3

    property alias header: headerLabel.text
    readonly property bool isHighlighted: Lith.search.highlightedLine && messageModel && Lith.search.highlightedLine === messageModel

    MouseArea {
        id: messageMouseArea
        anchors.fill: parent
        pressAndHoldInterval: 400 // does this do anything? or is it just for signals?
        hoverEnabled: true
        acceptedButtons: (LithPlatform.mobile ? Qt.LeftButton : 0) | Qt.RightButton
        cursorShape: messageText.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
        propagateComposedEvents: true
        onPressAndHold: {
            channelMessageActionMenu.show(messageModel)
        }
        onClicked: (mouse) => {
            if (mouse.button === Qt.RightButton) {
                channelMessageActionMenu.show(messageModel)
            }
        }
    }

    Label {
        id: headerLabel
        visible: text.length > 0
        width: parent.width
        horizontalAlignment: Label.AlignHCenter
        font.pixelSize: FontSizes.message
        topPadding: 3
        bottomPadding: 3
        Rectangle {
            anchors.fill: parent
            z: -1
            height: 1
            color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.1)
        }
    }

    Label {
        id: dateAndPrefixLabel
        x: 0
        y: headerLabel.visible ? headerLabel.height : 0
        font.pixelSize: FontSizes.message
        text: messageModel.dateAndPrefix
        color: LithPalette.regular.text
        textFormat: Text.RichText
        lineHeight: messageText.lineHeight
        lineHeightMode: messageText.lineHeightMode
    }

    Label {
        id: messageText
        x: dateAndPrefixLabel.width
        y: headerLabel.visible ? headerLabel.height : 0
        width: parent.width - x
        text: messageModel.message
        font.pixelSize: FontSizes.message
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: LithPalette.regular.text
        textFormat: Text.RichText
        lineHeight: font.pixelSize + 1
        lineHeightMode: Label.FixedHeight
        onLinkActivated: (link) => {
            linkHandler.show(link, root)
        }
    }

    ListView {
        id: previewListView
        visible: Lith.settings.showImageThumbnails && messageModel.message.urls.length > 0

        y: (headerLabel.visible ? headerLabel.height : 0) + Math.max(dateAndPrefixLabel.height, messageText.height)
        height: 192
        width: parent.width

        orientation: Qt.Horizontal
        spacing: 12

        reuseItems: true
        model: messageModel.message.urls
        delegate: ChannelMessageThumbnail {
            width: previewListView.height
            height: previewListView.height
            // asynchronous: true
            thumbnailUrl: modelData
        }
        header: Item {
            height: previewListView.height
            visible: previewListView.width > previewListView.count * previewListView.height
            width: Math.max(0, previewListView.width - previewListView.count * previewListView.height) / 2
        }
    }
}
