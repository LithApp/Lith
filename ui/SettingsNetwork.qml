import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: root
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    function onAccepted() {
        var newHost = hostField.text
        var newPort = portField.text
        var newEncrypted = encryptedCheckbox.checked
        var newPassphrase = passphraseField.text
        settings.host = newHost
        settings.port = newPort
        settings.encrypted = newEncrypted
        if (newPassphrase.length > 0)
            settings.passphrase = newPassphrase
        newPassphrase = ""
    }
    function onRejected() {
        hostField.text = settings.host
        portField.text = settings.port
        encryptedCheckbox.checked = settings.encrypted
        passphraseField.text = ""
    }

    ColumnLayout {
        x: 6
        y: 6
        width: root.width - 12
        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            columns: 2
            Text {
                text: "Hostname"
            }
            TextField {
                id: hostField
                text: settings.host
                inputMethodHints: Qt.ImhNoPredictiveText
            }
            Text {
                text: "Port"
            }
            TextField {
                id: portField
                text: settings.port
                inputMethodHints: Qt.ImhPreferNumbers
                validator: IntValidator {
                    bottom: 0
                    top: 65535
                }
            }
            Text {
                text: "SSL"
            }
            CheckBox {
                id: encryptedCheckbox
                checked: settings.encrypted
                Layout.alignment: Qt.AlignLeft
            }
            Text {
                text: "Password"
            }
            TextField {
                id: passphraseField
                color: palette.text
                placeholderText: lith.hasPassphrase ? "**********" : ""
                echoMode: TextInput.Password
                passwordCharacter: "*"
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
