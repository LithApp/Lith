import QtQuick 2.11

ListView {
    TextMetrics {
        id: timeMetrics
        text: Qt.formatTime(new Date(), Locale.LongFormat)
        font.family: "Menlo"
        font.pointSize: 16
    }

    model: stuff.selected ? stuff.selected.lines : null
    rotation: 180
    delegate: ChannelMessage {}
    MouseArea {
        z: -1
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAxis
        drag.maximumX: timeMetrics.width + 6
        drag.minimumX: 0
    }
    /*
    onContentYChanged: {
        if (contentY + height - contentHeight < 50)
            stuff.selected.fetchMoreLines()
        else
            console.warn(contentY + height - contentHeight)
        console.warn(contentY + " " + contentHeight + " " +height)
    }
    */
    property real yPosition: visibleArea.yPosition
    onYPositionChanged: {
        if (yPosition + visibleArea.heightRatio > 0.65)
            stuff.selected.fetchMoreLines()
        console.log(visibleArea.yPosition + " " + visibleArea.heightRatio)
    }

    /*
    onVerticalOvershootChanged: {
        if (verticalOvershoot > 30)
            stuff.selected.fetchMoreLines()
        console.warn(verticalOvershoot)
    }
    */
}
