import QtQuick
import QtQuick.Controls

import lith 1.0

Rectangle {
    id: root

    property alias text: theLabel.text

    property bool hot: false

    property real layoutSpacing
    readonly property real textSize: lith.settings.hotlistCompact ? 9 : 16
    readonly property real padding: lith.settings.hotlistCompact ? 8 : 16

    signal clicked

    implicitWidth: visible ? Math.max(itemLayout.width + root.padding, implicitHeight) : -layoutSpacing
    implicitHeight: textSize + root.padding
    radius: 3
    color: hot > 0 ? palette.highlight : palette.text
    Row {
        id: itemLayout
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: lith.settings.hotlistCompact ? 1 : 3
        Text {
            id: theLabel
            font.pointSize: textSize
            font.bold: true
            color: hot ? palette.highlightedText : palette.window
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.clicked()
        }
    }
}
