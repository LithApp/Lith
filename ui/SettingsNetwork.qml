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

import "SettingsFields" as Fields

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
        settings.allowSelfSignedCertificates = selfSignedCertificateCheckbox.checked
        settings.handshakeAuth = handshakeAuthCheckbox.checked
        settings.connectionCompression = connectionCompressionCheckbox.checked
        if (typeof settings.useWebsockets !== "undefined") {
            settings.useWebsockets = useWebsocketsCheckbox.checked
        }
        if (typeof settings.websocketsEndpoint !== "undefined") {
            settings.websocketsEndpoint = websocketsEndpointInput.text
        }
    }
    function onRejected() {
        passphraseField.text = ""
        hostField.text = settings.host
        portField.text = settings.port
        encryptedCheckbox.checked = settings.encrypted
        selfSignedCertificateCheckbox.checked = settings.allowSelfSignedCertificates
        handshakeAuthCheckbox.checked = settings.handshakeAuth
        connectionCompressionCheckbox.checked = settings.connectionCompression
        if (typeof settings.useWebsockets !== "undefined") {
            useWebsocketsCheckbox.checked = settings.useWebsockets
        }
        if (typeof settings.websocketsEndpoint !== "undefined") {
            websocketsEndpointInput.text = settings.websocketsEndpoint
        }
    }

    Item {
        width: parent.width
        implicitHeight: settingsPaneLayout.implicitHeight

        ColumnLayout {
            id: settingsPaneLayout
            anchors.horizontalCenter: parent.horizontalCenter
            width: window.landscapeMode ? Math.min(Math.min(420, 1.33 * implicitWidth), parent.width) : parent.width
            spacing: -1

            Fields.Header {
                text: "Weechat connection"
            }
            Fields.String {
                id: hostField
                text: settings.host

                summary: qsTr("Hostname")
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhSensitiveData | Qt.ImhNoAutoUppercase
            }
            Fields.Integer {
                id: portField
                text: settings.port

                summary: qsTr("Port")
                validator: IntValidator {
                    bottom: 0
                    top: 65535
                }
            }
            Fields.Boolean {
                id: encryptedCheckbox
                checked: settings.encrypted

                summary: qsTr("SSL")
            }
            Fields.Boolean {
                id: selfSignedCertificateCheckbox
                checked: settings.allowSelfSignedCertificates

                summary: qsTr("Allow self-signed certificates")
                details: "(Less secure, not recommended)"
            }
            Fields.String {
                id: passphraseField
                summary: qsTr("Password")
                placeholderText: lith.hasPassphrase ? "**********" : ""
                echoMode: TextInput.Password
            }
            Fields.Boolean {
                id: handshakeAuthCheckbox
                checked: settings.handshakeAuth

                summary: qsTr("Use Handshake")
                details: "(More secure, available since WeeChat 2.9)"
            }
            Fields.Boolean {
                id: connectionCompressionCheckbox
                checked: settings.connectionCompression

                summary: qsTr("Use WeeChat compression")
            }
            Fields.Header {
                text: "Websockets"
            }
            Fields.Boolean {
                id: useWebsocketsCheckbox
                visible: typeof settings.useWebsockets !== "undefined"
                checked: settings.useWebsockets

                summary: qsTr("Use WebSockets to connect")
            }
            Fields.String {
                id: websocketsEndpointInput
                visible: typeof settings.useWebsockets !== "undefined"
                enabled: useWebsocketsCheckbox.checked
                text: settings.websocketsEndpoint

                summary: qsTr("Websockets endpoint")
            }
            Button {
                Layout.topMargin: 24
                Layout.alignment: Qt.AlignHCenter
                text: "Reconnect"
                enabled: lith.status == Lith.CONNECTED || lith.status == Lith.CONNECTING
                onClicked: lith.reconnect()
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }
}
