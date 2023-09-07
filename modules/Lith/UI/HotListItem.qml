import QtQuick
import QtQuick.Controls

import Lith.Core
import Lith.Style

Rectangle {
    id: root

    property alias text: theLabel.text

    property bool hot: false

    property real layoutSpacing
    readonly property real textSize: theLabel.font.pointSize
    readonly property int labelType: Lith.settings.hotlistCompact ? Label.Small : Label.Regular
    readonly property real padding: Lith.settings.hotlistCompact ? 8 : 16

    signal clicked

    implicitWidth: visible ? Math.max(itemLayout.width + root.padding, implicitHeight) : -layoutSpacing
    implicitHeight: textSize + root.padding
    radius: 3
    color: hot > 0 ? LithPalette.regular.highlight : LithPalette.regular.text
    Row {
        id: itemLayout
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: Lith.settings.hotlistCompact ? 1 : 3
        Label {
            id: theLabel
            size: root.labelType
            font.bold: true
            color: hot ? LithPalette.regular.highlightedText : LithPalette.regular.window
        }
    }
    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            root.clicked()
        }
    }
}
