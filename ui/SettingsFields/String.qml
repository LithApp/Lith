import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Base {
    id: root
    property alias text: textField.text
    property alias placeholderText: textField.placeholderText
    property alias validator: textField.validator
    property alias inputMethodHints: textField.inputMethodHints
    property alias horizontalAlignment: textField.horizontalAlignment
    property alias echoMode: textField.echoMode
    property alias passwordCharacter: textField.passwordCharacter
    property alias readOnly: textField.readOnly

    rowComponent: TextField {
        id: textField
        horizontalAlignment: TextField.AlignRight
        Layout.fillWidth: true
        Layout.maximumWidth: root.width / 2
        passwordCharacter: "*"
    }
}
