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
    delegate: Message {}
    MouseArea {
        z: -1
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAxis
        drag.maximumX: timeMetrics.width + 6
        drag.minimumX: 0
    }
}
