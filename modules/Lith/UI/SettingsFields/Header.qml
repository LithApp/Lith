import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property bool standalone: false
    property alias text: label.text
    property alias font: label.font
    property alias labelItem: label

    Layout.fillWidth: true
    implicitWidth: label.implicitWidth
    implicitHeight: label.implicitHeight

    Label {
        id: label

        x: (root.width - width) / 2

        topPadding: font.pixelSize
        bottomPadding: root.standalone ? font.pixelSize : 0.33 * font.pixelSize
        font.bold: true
        font.capitalization: Font.AllUppercase
    }
}
