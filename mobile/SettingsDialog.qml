import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Dialog {
    title: "Settings"
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok | Dialog.Cancel

    header: Rectangle {
        color: palette.base
        height: 48
        width: settingsDialog.width
        Text {
            anchors.centerIn: parent
            text: "Settings"
            color: palette.text
        }
    }

    background: Rectangle {
        color: palette.base
    }

    onAccepted: {
        weechat.host = hostField.text
        weechat.port = portField.text
        weechat.encrypted = encryptedCheckbox.checked
        if (passphraseField.text.length > 0)
            weechat.setPassphrase(passphraseField.text)
        passphraseField.text = ""
    }
    onRejected: {
        hostField.text = weechat.host
        portField.text = weechat.port
        encryptedCheckbox.checked = weechat.encrypted
        passphraseField.text = ""
    }
    GridLayout {
        anchors.fill: parent
        columns: 2
        Text {
            text: "Hostname"
            color: palette.text
        }
        TextField {
            id: hostField
            text: weechat.host
            inputMethodHints: Qt.ImhNoPredictiveText
        }
        Text {
            text: "Port"
            color: palette.text
        }
        TextField {
            id: portField
            text: weechat.port
            inputMethodHints: Qt.ImhPreferNumbers
            validator: IntValidator {
                bottom: 0
                top: 65535
            }
        }
        Text {
            text: "SSL"
            color: palette.text
        }
        CheckBox {
            id: encryptedCheckbox
            checked: weechat.encrypted
            Layout.alignment: Qt.AlignLeft
        }
        Text {
            text: "Password"
            color: palette.text
        }
        TextField {
            id: passphraseField
            placeholderText: weechat.hasPassphrase ? "*****" : ""
            echoMode: TextInput.Password
            passwordCharacter: "*"
        }
    }
}
