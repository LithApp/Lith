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

import Lith.Core
import Lith.Style

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
    readonly property color overshootMarkerColor: LithPalette.regular.highlight

    readonly property real delegateWidth: listView.width - (Lith.settings.scrollbarsOverlayContents ? 0 : scrollBar.width)

    TextMetrics {
        id: timeMetrics
        text: Qt.formatTime(new Date(), Locale.LongFormat)
        font.pointSize: Lith.settings.baseFontSize
    }

    ScrollBar.vertical: ScrollBar {
        id: scrollBar
    }

    orientation: Qt.Vertical
    verticalLayoutDirection: ListView.BottomToTop
    spacing: Lith.settings.messageSpacing
    model: Lith.selectedBuffer ? Lith.selectedBuffer.lines_filtered : null
    /*
    delegate: ChannelMessage {
        messageModel: modelData
        readonly property string sectionName: ListView.section
        readonly property string nextSectionName: ListView.nextSection
        header: Lith.settings.showDateHeaders && sectionName !== nextSectionName ? sectionName : ""
        width: listView.delegateWidth
    }
    */
    delegate: TextArea {
        id: delegateRoot
        padding: 0
        width: ListView.view.width
        textFormat: TextEdit.RichText
        renderType: TextEdit.NativeRendering
        wrapMode: Text.Wrap
        color: LithPalette.regular.text
        activeFocusOnPress: false
        activeFocusOnTab: false

        required property var modelData
        readonly property var messageModel: modelData
        required property int index

        text: messageModel.formatted
        /*
        text: ("<table style='' border=0 cellspacing=0 cellpadding=0>"
                +"<tr>"
                    +"<td style='margins: 0px; padding: 0px'>%1 %2</td>"
                    +"<td style='padding: 0px'>%3</td>"
                +"</tr>"
               //+"<tr><td colspan=2><span>"
                    //+"<img width='100' height='100' src='https://cdn.discordapp.com/attachments/854369705943236609/1157400238814924820/eoo30koaf6rb1.png?ex=65187878&is=651726f8&hm=277d825c1634cbb742c63ab20b2945993155bc90048e224d0ca85609b9c51912&'/>"
                    //+"<img width='100' src='https://shitpost.fun/euova5.png'/>"
               //+"</span></td></tr>
               +"</table>")
                .arg(messageModel.date.toLocaleString(Qt.locale(), Lith.settings.timestampFormat))
                .arg(messageModel.prefix.toTrimmedHtml(Lith.settings.nickCutoffThreshold))
                .arg(messageModel.message)
        */

        /*
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
        }
        */

        onLinkActivated: (link) => {
            linkHandler.show(link, delegateRoot)
        }
        onHoveredLinkChanged: {
            if (hoveredLink) {
                selectionArea.itemWithHoveredUrl = this
            }
            else {
                if (selectionArea.itemWithHoveredUrl == this) {
                    selectionArea.itemWithHoveredUrl = null
                }
            }
        }

        Connections {
            target: selectionArea
            function onSelectionChanged() {
                updateSelection();
            }
        }

        Component.onCompleted: updateSelection()

        function updateSelection() {
            if (index < selectionArea.selTopIndex || index > selectionArea.selBottomIndex)
                delegateRoot.select(0, 0);
            else if (index > selectionArea.selTopIndex && index < selectionArea.selBottomIndex)
                delegateRoot.selectAll();
            else if (index === selectionArea.selTopIndex && index === selectionArea.selBottomIndex)
                delegateRoot.select(selectionArea.selTopPos, selectionArea.selBottomPos);
            else if (index === selectionArea.selTopIndex)
                //delegateRoot.select(selectionArea.selStartPos, delegateRoot.length);
                delegateRoot.select(0, selectionArea.selTopPos);
            else if (index === selectionArea.selBottomIndex)
                //delegateRoot.select(0, selectionArea.selEndPos);
                delegateRoot.select(selectionArea.selBottomPos, delegateRoot.length);
        }
    }

    function indexAtRelative(x, y) {
        return indexAt(x + contentX, y + contentY)
    }

    MouseArea {
        id: selectionArea
        z: 1000000
        readonly property int selTopIndex: selStartIndex < selEndIndex ? selStartIndex : selEndIndex
        readonly property int selBottomIndex: selStartIndex < selEndIndex ? selEndIndex : selStartIndex
        readonly property int selTopPos: selStartIndex < selEndIndex ? selStartPos : selEndPos
        readonly property int selBottomPos: selStartIndex < selEndIndex ? selEndPos : selStartPos
        property int selStartIndex
        property int selEndIndex
        property int selStartPos
        property int selEndPos

        signal selectionChanged

        property var itemWithHoveredUrl: null

        anchors.fill: parent
        enabled: !scrollBar.hovered
        cursorShape: enabled ? itemWithHoveredUrl ? Qt.PointingHandCursor : Qt.IBeamCursor : Qt.ArrowCursor
        propagateComposedEvents: false

        function indexAndPos(x, y) {
            const index = listView.indexAtRelative(x, y)
            if (index === -1)
                return
            const item = listView.itemAtIndex(index)
            const relItemY = item.y - listView.contentY
            const itemLocalCoords = Qt.point(x, y - relItemY)
            const pos = item.positionAt(itemLocalCoords.x, itemLocalCoords.y)

            return [index, pos, item, itemLocalCoords]
        }

        onPressed: (mouse) => {
            const result = indexAndPos(mouse.x, mouse.y)
            if (!result)
                return
            selStartIndex = result[0]
            selStartPos = result[1]
            selEndIndex = result[0]
            selEndPos = result[1]
            selectionChanged()
        }

        onReleased: (mouse) => {
            const result = indexAndPos(mouse.x, mouse.y)
            if (!result)
                return
            selEndIndex = result[0]
            selEndPos = result[1]
            const item = result[2]
            const localCoords = result[3]

            selectionChanged()
            if (selStartIndex == selEndIndex && selStartPos == selEndPos) {
                mouse.accepted = false
                const link = item.linkAt(localCoords.x, localCoords.y)
                if (link.length > 0)
                    linkHandler.show(link, item)

            }
            else {
                mouse.accepted = true
            }
        }

        onClicked: (mouse) => {
            mouse.accepted = false
        }

        onPositionChanged: (mouse) => {
            if (pressed) {
                const result = indexAndPos(mouse.x, mouse.y)
                if (!result)
                    return
                selEndIndex = result[0]
                selEndPos = result[1]
                selectionChanged()
            }
        }
    }

    function getSelectedText() {
        var result = ""
        for (let i = selectionArea.selBottomIndex; i >= selectionArea.selTopIndex; i--) {
            const item = listView.itemAtIndex(i)
            console.warn(item.selectedText)
            result += item.selectedText
        }
        return result
    }

    Keys.onShortcutOverride: (event) => {
        console.warn("A")
    }

    Shortcut {
        sequence: "Ctrl+M"
        onActivated: {
            console.warn("CCCCC")
            listView.getSelectedText()
        }
    }

    section.criteria: ViewSection.FullString
    section.labelPositioning: ViewSection.InlineLabels
    section.property: "modelData.dateString"

    interactive: false
    Connections {
        target: Lith.search
        function onHighlightedMatchIndexChanged() {
            if (!Lith.search.highlightedLine)
                return
            for(var i = 0; i < listView.model.count(); i++) {
                var modelLine = listView.model.at(i)
                if (Lith.search.highlightedLine === modelLine) {
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
        if (!Lith.selectedBuffer)
            return
        if (visibleArea.heightRatio >= 0.5 || yPosition + visibleArea.heightRatio <= 0.5) {
            Lith.selectedBuffer.fetchMoreLines()
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
        icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/down-arrow.png"
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
            GradientStop { position: 1.0; color: ColorUtils.mixColors(listView.overshootMarkerColor, LithPalette.regular.window, 0.5) }
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
            GradientStop { position: 0.0; color: ColorUtils.mixColors(listView.overshootMarkerColor, LithPalette.regular.window, 0.5) }
            GradientStop { position: 1.0; color: listView.overshootMarkerColor }
        }
        visible: listView.dragging && listView.verticalOvershoot > 0
    }
}
