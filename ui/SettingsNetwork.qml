// Lith
// Copyright (C) 2020 Martin Bříza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import lith 1.0

ScrollView {
    id: root
    clip: true
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    function onAccepted() {
        var newPassphrase = passphraseField.text
        if (newPassphrase.length > 0)
            settings.passphrase = newPassphrase
        settings.host = hostField.text
        settings.port = portField.text
        settings.encrypted = encryptedCheckbox.checked
        settings.handshakeAuth = handshakeAuthCheckbox.checked
        settings.connectionCompression = connectionCompressionCheckbox.checked
        if (typeof settings.useWebsockets !== "undefined") {
            settings.useWebsockets = useWebsocketsCheckbox.checked
        }
    }
    function onRejected() {
        passphraseField.text = ""
        hostField.text = settings.host
        portField.text = settings.port
        encryptedCheckbox.checked = settings.encrypted
        handshakeAuthCheckbox.checked = settings.handshakeAuth
        connectionCompressionCheckbox.checked = settings.connectionCompression
        if (typeof settings.useWebsockets !== "undefined") {
            useWebsocketsCheckbox.checked = settings.useWebsockets
        }
    }

    ColumnLayout {
        x: 6
        y: 6
        width: root.width - 12
        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            columns: 2
            Label {
                text: qsTr("Hostname")
            }
            TextField {
                id: hostField
                text: settings.host
                inputMethodHints: Qt.ImhNoPredictiveText
            }
            Label {
                text: qsTr("Port")
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
            Label {
                text: "SSL"
            }
            CheckBox {
                id: encryptedCheckbox
                checked: settings.encrypted
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                text: qsTr("Password")
            }
            TextField {
                id: passphraseField
                color: palette.text
                placeholderText: lith.hasPassphrase ? "**********" : ""
                echoMode: TextInput.Password
                passwordCharacter: "*"
            }
            ColumnLayout {
                spacing: 0
                Label {
                    text: "Use Handshake"
                }
                Label {
                    text: "(More secure, available since WeeChat 2.9)"
                    font.pointSize: font.pointSize * 0.75
                }
            }
            CheckBox {
                id: handshakeAuthCheckbox
                checked: settings.handshakeAuth
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                text: "Use WeeChat compression"
            }
            CheckBox {
                id: connectionCompressionCheckbox
                checked: settings.connectionCompression
                Layout.alignment: Qt.AlignLeft
            }
            Label {
                visible: typeof settings.useWebsockets !== "undefined"
                text: "Use WebSockets to connect"
            }
            CheckBox {
                visible: typeof settings.useWebsockets !== "undefined"
                id: useWebsocketsCheckbox
                checked: settings.useWebsockets
                Layout.alignment: Qt.AlignLeft
            }
            Button {
                Layout.alignment: Qt.AlignHCenter
                ColumnLayout.columnSpan: 2
                text: "Reconnect"
                enabled: lith.status == Lith.CONNECTED || lith.status == Lith.CONNECTING
                onClicked: lith.reconnect()
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
