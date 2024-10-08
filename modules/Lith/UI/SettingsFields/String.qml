import QtQuick
import QtQuick.Layouts

import Lith.Style

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
    signal textEdited

    rowComponent: TextField {
        id: textField
        horizontalAlignment: TextField.AlignRight
        Layout.fillWidth: true
        Layout.maximumWidth: root.halfWidth
        Layout.preferredWidth: root.halfWidth
        passwordCharacter: "*"
        onTextEdited: root.textEdited()
    }
}
