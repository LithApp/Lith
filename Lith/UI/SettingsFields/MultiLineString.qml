import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Base {
    id: root
    property alias text: textField.text
    property alias placeholderText: textField.placeholderText
    property alias inputMethodHints: textField.inputMethodHints
    property alias horizontalAlignment: textField.horizontalAlignment
    property alias fieldImplicitHeight: textField.implicitHeight
    property alias font: textField.font

    columnComponent: TextArea {
        id: textField
        implicitHeight: 200
        Layout.fillWidth: true
        Layout.bottomMargin: 9
        verticalAlignment: TextArea.Top
    }
}
