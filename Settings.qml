import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

Dialog {
    title: "Configuration"

    onAccepted: {
        weechat.host = hostField.text
        weechat.port = portField.text
        weechat.encrypted = encryptedCheckbox.checked
        if (passphraseField.text.length > 0)
            weechat.setPassphrase(passphraseField.text)
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
        }
        Text {
            text: "Port"
        }
        TextField {
            id: portField
            text: weechat.port
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
            echoMode: TextInput.Password
        }
    }
}
