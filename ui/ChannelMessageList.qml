import QtQuick 2.15
import QtQuick.Controls 2.12

ListView {
    id: listView
    TextMetrics {
        id: timeMetrics
        text: Qt.formatTime(new Date(), Locale.LongFormat)
        font.family: "Menlo"
        font.pointSize: settings.baseFontSize
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
    model: lith.selectedBuffer ? lith.selectedBuffer.lines : null
    delegate: ChannelMessage {
        messageModel: modelData
    }
    //reuseItems: true

    MouseArea {
        z: -1
        enabled: false
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        drag.target: listView
        drag.axis: Drag.XAxis
        drag.maximumX: 0
        drag.minimumX: timeMetrics.width + 6
    }
    /*
    onContentYChanged: {
        if (contentY + height - contentHeight < 50)
            lith.selectedBuffer.fetchMoreLines()
        else
            console.warn(contentY + height - contentHeight)
        console.warn(contentY + " " + contentHeight + " " +height)
    }
    */
    function fillTopOfList() {
        if (yPosition + visibleArea.heightRatio > 0.65)
            lith.selectedBuffer.fetchMoreLines()
    }

    property real yPosition: visibleArea.yPosition
    onYPositionChanged: fillTopOfList()
    onContentHeightChanged: fillTopOfList()
    onModelChanged: fillTopOfList()



    /*
    onVerticalOvershootChanged: {
        if (verticalOvershoot > 30)
            lith.selectedBuffer.fetchMoreLines()
        console.warn(verticalOvershoot)
    }
    */
}
