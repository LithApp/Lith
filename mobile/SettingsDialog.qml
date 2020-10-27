import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Dialog {
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok | Dialog.Cancel

    width: parent.width
    height: parent.height

    SystemPalette {
        id: palette
    }

    header: TabBar {
        id: tabBar
        width: parent.width
        TabButton {
            text: "Connection"
        }
        TabButton {
            text: "Interface"
        }
    }

    background: Rectangle {
        color: "#eeeeee"
    }

    onAccepted: {
        var newHost = hostField.text
        var newPort = portField.text
        var newEncrypted = encryptedCheckbox.checked
        var newPassphrase = passphraseField.text
        lith.host = newHost
        lith.port = newPort
        lith.encrypted = newEncrypted
        if (newPassphrase.length > 0)
            lith.setPassphrase(newPassphrase)
        newPassphrase = ""

        var newShowFullLink = showFullLinkCheckbox.checked
        settings.showFullLink = newShowFullLink

        settings.showSendButton = showSendButtonCheckbox.checked
    }
    onRejected: {
        hostField.text = lith.host
        portField.text = lith.port
        encryptedCheckbox.checked = lith.encrypted
        passphraseField.text = ""

        showFullLinkCheckbox.checked = settings.showFullLink
        showSendButtonCheckbox.checked = settings.showSendButton
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            GridLayout {
                columns: 2
                Text {
                    text: "Hostname"
                }
                TextField {
                    id: hostField
                    text: lith.host
                    inputMethodHints: Qt.ImhNoPredictiveText
                }
                Text {
                    text: "Port"
                }
                TextField {
                    id: portField
                    text: lith.port
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
                    checked: lith.encrypted
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
        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            columns: 2
            Text {
                text: "Show full URLs"
            }
            CheckBox {
                id: showFullLinkCheckbox
                checked: settings.showFullLink
                Layout.alignment: Qt.AlignLeft
            }
            Text {
                text: "Show send button"
            }
            CheckBox {
                id: showSendButtonCheckbox
                checked: settings.showSendButton
                Layout.alignment: Qt.AlignLeft
            }
            Text {
                text: "Font size"
            }
            SpinBox {
                value: settings.baseFontSize
                onValueChanged: settings.baseFontSize = value
                from: 6
                to: 32
            }
        }

    }
}
