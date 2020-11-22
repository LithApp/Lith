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
        flickable: mobilePlatform ? null : parent
        reverse: true
        anchors.fill: mobilePlatform ? null : parent
    }

    ScrollBar.vertical: ScrollBar {
        id: scrollBar
        hoverEnabled: true
        active: hovered || pressed
        rotation: 180
        orientation: Qt.Vertical
        parent: listView.parent
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

    Button {
        anchors {
            top: parent.top
            left: parent.left
            margins: height / 2
        }
        width: height
        flat: false
        icon.source: "qrc:/navigation/"+currentTheme+"/down-arrow.png"
        opacity: listView.yPosition > 0.0 ? 0.5 : 0.0
        Behavior on opacity { NumberAnimation { duration: 100 } }
        rotation: 180
        onClicked: positionViewAtBeginning()
    }
}
