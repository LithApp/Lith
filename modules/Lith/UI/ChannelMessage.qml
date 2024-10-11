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

    opacity: messageModel.searchCompare(Lith.search.term) ? 1.0 : 0.33

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

    Rectangle {
        anchors.fill: parent
        anchors.topMargin: headerLabel.visible ? headerLabel.height : 0
        color: messageModel.highlight ? ColorUtils.mixColors(LithPalette.regular.highlight, "black", 0.9) : "transparent"
        border.color: LithPalette.regular.windowText
        border.width: isHighlighted ? 1 : 0
    }

    Label {
        id: headerLabel
        visible: text.length > 0
        width: parent.width
        horizontalAlignment: Label.AlignHCenter
        font.pixelSize: FontSizes.message
        topPadding: 3
        bottomPadding: 3
    }

    Label {
        id: dateAndPrefixLabel
        x: 0
        y: headerLabel.visible ? headerLabel.height : 0
        font.pixelSize: FontSizes.message
        text: messageModel.dateAndPrefix
        color: messageModel.highlight ? LithPalette.regular.highlightedText : LithPalette.regular.text
        textFormat: Text.RichText
    }

    Label {
        id: messageText
        x: dateAndPrefixLabel.width
        y: headerLabel.visible ? headerLabel.height : 0
        width: parent.width - x
        text: messageModel.message
        font.pixelSize: FontSizes.message
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: messageModel.highlight ? LithPalette.regular.highlightedText : LithPalette.regular.text
        textFormat: Text.RichText
        onLinkActivated: (link) => {
            linkHandler.show(link, root)
        }
    }

    ListView {
        id: previewListView
        visible: Lith.settings.showImageThumbnails && messageModel.message.urlsWithPreviews.length > 0

        y: (headerLabel.visible ? headerLabel.height : 0) + Math.max(dateAndPrefixLabel.height, messageText.height)
        height: 192
        width: parent.width

        orientation: Qt.Horizontal
        spacing: 12

        reuseItems: true
        model: messageModel.message.urlsWithPreviews
        delegate: ChannelMessageThumbnail {
            height: previewListView.height
            maximumHeightHorizontal: previewListView.height
            maximumWidth: messageText.width
            // asynchronous: true
            thumbnailUrl: modelData
        }
        header: Item {
            height: previewListView.height
            visible: previewListView.width > previewListView.count * previewListView.height
            width: dateAndPrefixLabel.width
        }
    }
}
