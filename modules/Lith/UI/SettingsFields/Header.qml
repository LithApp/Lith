import QtQuick
import QtQuick.Layouts

import Lith.Style

Item {
    id: root

    property bool standalone: false
    property alias text: label.text
    property alias font: label.font
    property alias labelItem: label
    property real leftPadding: 0
    property real rightPadding: 0

    Layout.fillWidth: true
    implicitWidth: label.implicitWidth
    implicitHeight: label.implicitHeight

    Label {
        id: label

        x: Math.max(root.leftPadding, (root.width - width) / 2)
        width: Math.min(implicitWidth, root.width - root.leftPadding - root.rightPadding)

        topPadding: font.pixelSize
        bottomPadding: root.standalone ? font.pixelSize : 0.33 * font.pixelSize
        font.bold: true
        font.capitalization: Font.AllUppercase
    }
}
