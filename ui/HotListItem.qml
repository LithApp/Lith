import QtQuick

import lith 1.0

Rectangle {
    id: root

    property alias text: theLabel.text

    property bool hot: false

    property real layoutSpacing
    readonly property real itemSize: lith.settings.hotlistCompact ? 18 : 32
    readonly property real textSize: lith.settings.hotlistCompact ? 9 : 16

    signal clicked

    implicitWidth: visible ? Math.max(itemLayout.width + 12, itemSize) : -layoutSpacing
    implicitHeight: itemSize
    radius: 3
    color: hot > 0 ? "#bb6666" : palette.text
    Row {
        id: itemLayout
        x: Math.max(6, (itemSize - width) / 2)
        anchors.verticalCenter: parent.verticalCenter
        spacing: lith.settings.hotlistCompact ? 1 : 3
        Text {
            id: theLabel
            font.pointSize: textSize
            font.bold: true
            color: palette.base
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked
    }
}
