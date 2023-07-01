import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property bool standalone: false
    property alias text: label.text
    property alias font: label.font

    Layout.fillWidth: true
    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    RowLayout {
        id: row
        width: parent.width
        Label {
            id: label
            Layout.topMargin: font.pixelSize
            Layout.bottomMargin: root.standalone ? font.pixelSize : 0.33 * font.pixelSize
            Layout.alignment: Qt.AlignHCenter
            font.bold: true
            font.capitalization: Font.AllUppercase
        }
    }
}
