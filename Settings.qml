import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

Dialog {
    title: "Configuration"

    standardButtons: Dialog.Ok | Dialog.Cancel

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
        columns: 2
        Text {
            text: "Host"
        }
        TextField {
            id: hostField
            placeholderText: "URL"
            text: weechat.host
            inputMethodHints: Qt.ImnNoPredictiveText
        }
        Text {
            text: "Port"
        }
        TextField {
            id: portField
            text: weechat.port
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator {
                bottom: 1
                top: 65535
            }
        }
        Text {
            text: "Use encryption"
        }
        CheckBox {
            id: encryptedCheckbox
            checked: weechat.encrypted
        }
        Text {
            text: "Passphrase"
        }
        TextField {
            id: passphraseField
            placeholderText: weechat.hasPassphrase ? "*****" : "";
            echoMode: TextInput.Password
        }
    }
}
