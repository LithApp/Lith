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
                text: qsTr("Hostname")
            }
            TextField {
                id: hostField
                text: settings.host
                inputMethodHints: Qt.ImhNoPredictiveText
            }
            Text {
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
            Text {
                text: "SSL"
            }
            CheckBox {
                id: encryptedCheckbox
                checked: settings.encrypted
                Layout.alignment: Qt.AlignLeft
            }
            Text {
                text: qsTr("Password")
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
