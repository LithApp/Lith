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
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: listView


    // ugh, this is an ugly hack to fix the button under the Drawer drag area
    // Qt doesn't seem to care about this https://bugreports.qt.io/browse/QTBUG-59141
    // note the whole widget is rotated by 180 degress so it has to be subtracted from the height
    property real scrollToBottomButtonPosition: scrollToBottomButton.visible ? height - scrollToBottomButton.y - scrollToBottomButton.height
                                                                             : height

    onHeightChanged: {
        if (atYEnd)
            positionViewAtBeginning()
    }

    // The ListView jumps around when new messages are being loaded while dragging, to avoid that annoyance, handle overshoots by ourselves.
    // Second part of this is at the bottom, overshooting is marked with expanding rectangles
    boundsMovement: Flickable.StopAtBounds
    readonly property color overshootMarkerColor: palette.highlight

    TextMetrics {
        id: timeMetrics
        text: Qt.formatTime(new Date(), Locale.LongFormat)
        font.pointSize: settings.baseFontSize
    }

    ScrollBar.vertical: ScrollBar {
        id: scrollBar
        hoverEnabled: true
    }

    orientation: Qt.Vertical
    verticalLayoutDirection: ListView.BottomToTop
    spacing: lith.settings.messageSpacing
    model: lith.selectedBuffer ? lith.selectedBuffer.lines_filtered : null
    delegate: ChannelMessage {
        messageModel: modelData
        readonly property string sectionName: ListView.section
        readonly property string nextSectionName: ListView.nextSection
        header: lith.settings.showDateHeaders && sectionName !== nextSectionName ? sectionName : ""
    }

    section.criteria: ViewSection.FullString
    section.labelPositioning: ViewSection.InlineLabels
    section.property: "modelData.dateString"

    Connections {
        target: lith.search
        function onHighlightedMatchIndexChanged() {
            if (!lith.search.highlightedLine)
                return
            for(var i = 0; i < listView.model.count(); i++) {
                var modelLine = listView.model.at(i)
                if (lith.search.highlightedLine === modelLine) {
                    listView.positionViewAtIndex(i, ListView.Contain)
                    break
                }
            }
        }
    }

    ChannelMessageActionMenu {
        id: channelMessageActionMenu
    }

    function fillTopOfList() {
        if (!lith.selectedBuffer)
            return
        if (visibleArea.heightRatio >= 0.5 || yPosition + visibleArea.heightRatio <= 0.5) {
            lith.selectedBuffer.fetchMoreLines()
        }
    }

    property real yPosition: visibleArea.yPosition
    onYPositionChanged: fillTopOfList()
    onContentHeightChanged: fillTopOfList()
    onModelChanged: fillTopOfList()

    property real absoluteYPosition: yPosition + visibleArea.heightRatio
    onAbsoluteYPositionChanged: {
        /* FIXME
           This would close the keyboard when the user starts scrolling through messages
           This is now disabled because the ListView gets slightly scrolled when keyboard opens
        if (Qt.inputMethod.visible && absoluteYPosition < 1)
            Qt.inputMethod.hide()
        */
    }

    Button {
        id: scrollToBottomButton
        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: height / 2
        }
        width: height
        flat: false
        icon.source: "qrc:/navigation/"+currentTheme+"/down-arrow.png"
        opacity: listView.atYEnd ? 0.0 : 0.5
        visible: opacity > 0.0
        Behavior on opacity { NumberAnimation { duration: 100 } }
        onClicked: positionViewAtBeginning()
    }

    Rectangle {
        id: topOvershootMarker
        anchors.top: parent.top
        width: parent.width
        height: Math.pow(Math.max(0, Math.abs(listView.verticalOvershoot)), 0.3)
        gradient: Gradient {
            GradientStop { position: 1.0; color: colorUtils.setAlpha(listView.overshootMarkerColor, 0.5) }
            GradientStop { position: 0.0; color: listView.overshootMarkerColor }
        }
        visible: listView.dragging && listView.verticalOvershoot < 0
    }

    Rectangle {
        id: bottomOvershootMarker
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -1
        width: parent.width
        height: Math.pow(Math.max(0, listView.verticalOvershoot), 0.3)
        gradient: Gradient {
            GradientStop { position: 0.0; color: colorUtils.setAlpha(listView.overshootMarkerColor, 0.5) }
            GradientStop { position: 1.0; color: listView.overshootMarkerColor }
        }
        visible: listView.dragging && listView.verticalOvershoot > 0
    }
}
