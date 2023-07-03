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

    property bool networkSettingsChanged: {
        if (passphraseField.text.length > 0)
            return true
        if (settings.host !== hostField.text)
            return true
        if (settings.port !== parseInt(portField.text))
            return true
        if (settings.encrypted !== encryptedCheckbox.checked)
            return true
        if (settings.allowSelfSignedCertificates !== selfSignedCertificateCheckbox.checked)
            return true
        if (settings.handshakeAuth !== handshakeAuthCheckbox.checked)
            return true
        if (settings.connectionCompression !== connectionCompressionCheckbox.checked)
            return true
        if (typeof settings.useWebsockets !== "undefined") {
            if (settings.useWebsockets !== useWebsocketsCheckbox.checked)
                return true
        }
        if (typeof settings.websocketsEndpoint !== "undefined") {
            if (settings.websocketsEndpoint !== websocketsEndpointInput.text)
                return true
        }
        return false
    }

    function onAccepted() {
        save()
    }
    function save() {
        // Network-resetting settings need to be changed at once to not reset the connection for each changed property
        settings.saveNetworkSettings(
            hostField.text, portField.text, encryptedCheckbox.checked, selfSignedCertificateCheckbox.checked,
            passphraseField.text, handshakeAuthCheckbox.checked, connectionCompressionCheckbox.checked,
            (typeof settings.useWebsockets !== "undefined" ? useWebsocketsCheckbox.checked : false),
            (typeof settings.websocketsEndpoint !== "undefined" ? websocketsEndpointInput.text : "")
        )
        settings.enableLogging = enableLoggingCheckbox.checked
    }
    function onRejected() {
        restore()
    }
    function restore() {
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
        enableLoggingCheckbox.checked = settings.enableLogging
    }

    Item {
        width: parent.width
        implicitHeight: settingsPaneLayout.implicitHeight + 12

        ColumnLayout {
            id: settingsPaneLayout
            anchors.horizontalCenter: parent.horizontalCenter
            width: window.landscapeMode ? Math.min(Math.min(420, 1.33 * implicitWidth), parent.width) : parent.width
            spacing: -1

            Fields.Header {
                text: "Weechat connection"
            }
            Fields.Base {
                summary: qsTr("Current status: %1").arg(lith.statusString)
                details: lith.status == Lith.ERROR ? lith.errorString : ""
                enabled: lith.status == Lith.CONNECTED || lith.status == Lith.CONNECTING
                rowComponent: ColumnLayout {
                    spacing: 1
                    Button {
                        Layout.alignment: Qt.AlignRight
                        text: "Reconnect"
                        onClicked: lith.reconnect()
                    }
                    Label {
                        Layout.alignment: Qt.AlignRight
                        visible: text.length > 0
                        text: root.networkSettingsChanged ? qsTr("Recent changes were not saved yet") : ""
                        font.pixelSize: 11
                    }
                }
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
            Fields.Header {
                text: "Logging"
            }
            Fields.Button {
                summary: qsTr("Debug log")
                text: "Open"
                onClicked: debugDialog.visible = true
            }
            Fields.Boolean {
                id: enableLoggingCheckbox
                checked: settings.enableLogging
                summary: qsTr("Enable logging")
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }
}
