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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "SettingsFields" as Fields

import Lith.Core
import Lith.Style

ScrollView {
    id: root
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    property bool networkSettingsChanged: {
        if (passphraseField.text.length > 0)
            return true
        if (Lith.settings.host !== hostField.text)
            return true
        if (Lith.settings.port !== parseInt(portField.text))
            return true
        if (Lith.settings.encrypted !== encryptedCheckbox.checked)
            return true
        if (Lith.settings.allowSelfSignedCertificates !== selfSignedCertificateCheckbox.checked)
            return true
        if (Lith.settings.handshakeAuth !== handshakeAuthCheckbox.checked)
            return true
        if (Lith.settings.connectionCompression !== connectionCompressionCheckbox.checked)
            return true
        if (typeof Lith.settings.useWebsockets !== "undefined") {
            if (Lith.settings.useWebsockets !== useWebsocketsCheckbox.checked)
                return true
        }
        if (typeof Lith.settings.websocketsEndpoint !== "undefined") {
            if (Lith.settings.websocketsEndpoint !== websocketsEndpointInput.text)
                return true
        }
        return false
    }

    function save() {
        // Network-resetting settings need to be changed at once to not reset the connection for each changed property
        Lith.settings.saveNetworkSettings(
            hostField.text, portField.text, encryptedCheckbox.checked, selfSignedCertificateCheckbox.checked,
            passphraseField.text, useEmptyPassphraseCheckbox.checked, handshakeAuthCheckbox.checked, connectionCompressionCheckbox.checked,
            (typeof Lith.settings.useWebsockets !== "undefined" ? useWebsocketsCheckbox.checked : false),
            (typeof Lith.settings.websocketsEndpoint !== "undefined" ? websocketsEndpointInput.text : "")
        )
        Lith.settings.showInternalData = showInternalDataCheckbox.checked
        Lith.settings.enableLogging = enableLoggingCheckbox.checked
        Lith.settings.enableReplayRecording = enableReplayRecordingCheckbox.checked
        Lith.settings.encryptedCredentials = encryptedCredentialsCheckbox.checked
        passphraseField.text = ""
    }
    function restore() {
        passphraseField.text = ""
        hostField.text = Lith.settings.host
        portField.text = Lith.settings.port
        encryptedCheckbox.checked = Lith.settings.encrypted
        selfSignedCertificateCheckbox.checked = Lith.settings.allowSelfSignedCertificates
        handshakeAuthCheckbox.checked = Lith.settings.handshakeAuth
        connectionCompressionCheckbox.checked = Lith.settings.connectionCompression
        if (typeof Lith.settings.useWebsockets !== "undefined") {
            useWebsocketsCheckbox.checked = Lith.settings.useWebsockets
        }
        if (typeof Lith.settings.websocketsEndpoint !== "undefined") {
            websocketsEndpointInput.text = Lith.settings.websocketsEndpoint
        }
        showInternalDataCheckbox.checked = Lith.settings.showInternalData
        enableLoggingCheckbox.checked = Lith.settings.enableLogging
        enableReplayRecordingCheckbox.checked = Lith.settings.enableReplayRecording
        encryptedCredentialsCheckbox.checked = Lith.settings.encryptedCredentials
    }

    Item {
        width: parent.width
        implicitHeight: settingsPaneLayout.implicitHeight + 12

        ColumnLayout {
            id: settingsPaneLayout
            anchors.horizontalCenter: parent.horizontalCenter
            width: window.WindowHelper.landscapeMode ? Math.min(Math.min(480, 1.33 * implicitWidth), parent.width) : parent.width
            spacing: -1

            Fields.Header {
                text: "Weechat connection"
            }
            Fields.Button {
                summary: qsTr("Current status: %1").arg(Lith.statusString)
                details: Lith.status == Lith.ERROR ? Lith.errorString : root.networkSettingsChanged ? qsTr("Recent changes were not saved yet") : ""
                rowComponent.enabled: Lith.status == Lith.CONNECTED || Lith.status == Lith.CONNECTING || Lith.status == Lith.ERROR || Lith.status == Lith.DISCONNECTED
                text: "Reconnect"
                onClicked: Lith.reconnect()
            }
            Fields.String {
                id: hostField
                text: Lith.settings.host

                summary: qsTr("Hostname")
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhSensitiveData | Qt.ImhNoAutoUppercase
            }
            Fields.Integer {
                id: portField
                text: Lith.settings.port

                summary: qsTr("Port")
                validator: IntValidator {
                    bottom: 0
                    top: 65535
                }
            }
            Fields.Boolean {
                id: encryptedCheckbox
                checked: Lith.settings.encrypted

                summary: qsTr("SSL")
            }
            Fields.Boolean {
                id: selfSignedCertificateCheckbox
                checked: Lith.settings.allowSelfSignedCertificates
                summary: qsTr("Allow self-signed certificates")
                details: checked ? qsTr("Lith will allow self-signed server and chain certificates and mismatching certificate hostnames.") : qsTr("(Less secure, not recommended)")
            }
            Fields.String {
                id: passphraseField
                summary: qsTr("Password")
                echoMode: TextInput.Password
                placeholderText: Lith.settings.hasPassphrase && !Lith.settings.useEmptyPassphrase && !useEmptyPassphraseCheckbox.checked ? "********" : useEmptyPassphraseCheckbox.checked ? "You use no passphrase" : "No password was entered yet"
                rowComponent.enabled: !useEmptyPassphraseCheckbox.checked
                columnComponent: RowLayout {
                    Layout.fillWidth: true
                    spacing: 3
                    ColumnLayout {
                        Layout.fillWidth: true
                        Label {
                            id: summary
                            Layout.topMargin: details.visible ? 6 : 0
                            Layout.fillWidth: true
                            elide: Label.ElideRight
                            color: ColorUtils.mixColors(LithPalette.regular.windowText, LithPalette.regular.window, enabled ? 1.0 : 0.5)
                            text: "Use an empty password"

                            MouseArea {
                                id: baseMouse
                                anchors.fill: parent
                                hoverEnabled: true

                                ToolTip.text: summary.text + (details.text.length > 0 ? "\n" + details.text : "")
                                ToolTip.visible: LithPlatform.mobile ? baseMouse.containsPress : baseMouse.containsMouse
                                ToolTip.delay: LithPlatform.mobile ? 0 : 800
                            }
                        }
                        Label {
                            id: details
                            Layout.fillWidth: true
                            Layout.bottomMargin: visible ? 6 : 0
                            Layout.rightMargin: visible ? 6 : 0
                            elide: Label.ElideRight
                            font.pixelSize: summary.font.pixelSize * 0.7
                            color: ColorUtils.mixColors(LithPalette.disabled.text, LithPalette.regular.window, enabled ? 1.0 : 0.5)
                            visible: text.length > 0
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                            maximumLineCount: 4
                            text: "Not recommended, low security.\nRequires to be enabled in WeeChat."
                        }
                    }
                    CheckBox {
                        padding: 0
                        id: useEmptyPassphraseCheckbox
                        checked: Lith.settings.useEmptyPassphrase
                    }
                }
            }
            Fields.Boolean {
                id: handshakeAuthCheckbox
                checked: Lith.settings.handshakeAuth

                summary: qsTr("Use Handshake")
                details: "(More secure, available since WeeChat 2.9)"
            }
            Fields.Boolean {
                id: connectionCompressionCheckbox
                checked: Lith.settings.connectionCompression

                summary: qsTr("Use WeeChat compression")
            }
            Fields.Header {
                text: "Websockets"
            }
            Fields.Boolean {
                id: useWebsocketsCheckbox
                visible: typeof Lith.settings.useWebsockets !== "undefined"
                checked: typeof Lith.settings.useWebsockets !== "undefined" ? Lith.settings.useWebsockets : false

                summary: qsTr("Use WebSockets to connect")
            }
            Fields.String {
                id: websocketsEndpointInput
                enabled: typeof Lith.settings.useWebsockets === "undefined" || useWebsocketsCheckbox.checked
                text: Lith.settings.websocketsEndpoint === "undefined" ? "" : Lith.settings.websocketsEndpoint

                summary: qsTr("Websockets endpoint")
            }

            ////////////////////////// STORAGE
            Fields.Header {
                text: "Storage"
            }
            Fields.Boolean {
                id: encryptedCredentialsCheckbox
                enabled: Lith.settings.credentialEncryptionAvailable
                checked: Lith.settings.encryptedCredentials && Lith.settings.credentialEncryptionAvailable
                summary: qsTr("Store connection credentials securely")
                details: Lith.settings.credentialEncryptionAvailable ? qsTr("Your host, port and passphrase settings will be stored in your %1").arg(LithPlatform.android ? "Keystore" : LithPlatform.ios || LithPlatform.macos ? "Keychain" : LithPlatform.windows ? "Credential store" : "Keyring/Wallet") : qsTr("Secure credential storage is not available on your system")
            }

            ////////////////////////// LOGGING AND DEBUGGING
            Fields.Header {
                text: "Logging and debugging"
            }
            Fields.Boolean {
                id: showInternalDataCheckbox
                checked: Lith.settings.showInternalData
                summary: qsTr("Show internal data")
                details: qsTr("Enable showing extra internal data in some portions of the UI.")
            }
            Fields.Boolean {
                id: enableLoggingCheckbox
                checked: Lith.settings.enableLogging
                summary: qsTr("Enable logging")
            }
            Fields.Boolean {
                id: enableReplayRecordingCheckbox
                checked: Lith.settings.enableReplayRecording
                summary: qsTr("Record network data")
                details: qsTr("(Developer only, restart required)\nLith will record all data coming from WeeChat for debugging purposes. Personal data such as message contents and encryption negotiation will be logged.")
                columnComponent: ListView {
                    id: recordingsListView
                    Layout.fillWidth: true
                    model: Lith.networkProxy.existingRecordings
                    implicitHeight: contentHeight
                    spacing: -1

                    Label {
                        color: LithPalette.disabled.text
                        text: qsTr("There were no present recordings on startup.")
                        visible: recordingsListView.count === 0
                        anchors.centerIn: parent
                    }

                    header: RowLayout {
                        visible: recordingsListView.count > 0
                        spacing: 0
                        Label {
                            Layout.leftMargin: 6
                            Layout.topMargin: 6
                            Layout.bottomMargin: 6
                            text: "Slot"
                            Layout.minimumWidth: font.pixelSize * 3
                            Layout.maximumWidth: Layout.minimumWidth
                            horizontalAlignment: Label.AlignHCenter
                        }
                        Label {
                            text: "Started"
                            Layout.minimumWidth: font.pixelSize * 9
                            Layout.maximumWidth: Layout.minimumWidth
                            horizontalAlignment: Label.AlignHCenter
                        }
                        Label {
                            text: "Size"
                            Layout.minimumWidth: font.pixelSize * 6
                            Layout.maximumWidth: Layout.minimumWidth
                            horizontalAlignment: Label.AlignHCenter
                        }
                        Label {
                            text: "Version"
                            Layout.minimumWidth: font.pixelSize * 3
                            Layout.maximumWidth: Layout.minimumWidth
                            horizontalAlignment: Label.AlignHCenter
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    delegate: Rectangle {
                        required property var modelData
                        required property int index
                        property bool beingRecorded: {
                            if (Lith.networkProxy.recording) {
                                if (Lith.networkProxy.slot === modelData.number)
                                    return true
                            }
                            return false
                        }

                        width: ListView.view.width
                        implicitHeight: recordingLayout.implicitHeight + recordingLayout.y * 2
                        color: index % 2 ? LithPalette.regular.alternateBase : LithPalette.regular.base
                        border.color: LithPalette.regular.light
                        border.width: 1
                        GridLayout {
                            id: recordingLayout
                            y: 6
                            x: 6
                            width: parent.width - x * 2
                            height: parent.height - y * 2
                            columnSpacing: 0
                            columns: {
                                const totalWidth = infoLayout.implicitWidth + actionsLayout.implicitWidth
                                if (totalWidth > recordingLayout.width)
                                    return 1
                                else
                                    return 2
                            }
                            RowLayout {
                                id: infoLayout
                                Label {
                                    id: numberLabel
                                    text: modelData.number
                                    font.weight: 99
                                    Layout.minimumWidth: font.pixelSize * 3
                                    Layout.maximumWidth: Layout.minimumWidth
                                    horizontalAlignment: Label.AlignHCenter
                                }
                                Label {
                                    id: overWrittenLabel
                                    visible: beingRecorded
                                    text: qsTr("This slot is currently being overwritten.")
                                    color: LithPalette.disabled.text
                                    Layout.minimumWidth: font.pixelSize * 15
                                    Layout.maximumWidth: Layout.minimumWidth
                                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                                }
                                Label {
                                    id: createdAtLabel
                                    visible: !beingRecorded
                                    text: Qt.formatDateTime(modelData.createdAt)
                                    Layout.minimumWidth: font.pixelSize * 9
                                    Layout.maximumWidth: Layout.minimumWidth
                                    horizontalAlignment: Label.AlignHCenter
                                }
                                Label {
                                    id: sizeLabel
                                    visible: !beingRecorded
                                    text: "%1 KiB".arg(Number(modelData.size / 1024.0).toLocaleString(Qt.locale(), "f", 0))
                                    Layout.minimumWidth: font.pixelSize * 6
                                    Layout.maximumWidth: Layout.minimumWidth
                                    horizontalAlignment: Label.AlignHCenter
                                }
                                Label {
                                    id: versionLabel
                                    opacity: (x + width + infoLayout.spacing) < infoLayout.width ? 1.0 : 0
                                    text: beingRecorded ? Lith.networkProxy.currentReplayVersion : modelData.version
                                    Layout.minimumWidth: font.pixelSize * 3
                                    Layout.maximumWidth: Layout.minimumWidth
                                    horizontalAlignment: Label.AlignHCenter
                                }
                                Item {
                                    Layout.fillWidth: true
                                }
                            }
                            RowLayout {
                                id: actionsLayout
                                Layout.alignment: Qt.AlignHCenter
                                Button {
                                    id: saveToDocumentsButton
                                    visible: !beingRecorded
                                    font.pixelSize: FontSizes.regular * 0.875
                                    horizontalPadding: 8
                                    verticalPadding: 2
                                    text: "Save to\nDocuments"
                                    onClicked: {
                                        var result = modelData.store()
                                        recordingResultPopupLabel.text = result
                                    }
                                }
                                Button {
                                    id: deleteRecordingButton
                                    visible: !beingRecorded
                                    Layout.leftMargin: 4
                                    Layout.preferredHeight: Math.max(deleteRecordingButton.implicitHeight, saveToDocumentsButton.implicitHeight)
                                    font.pixelSize: FontSizes.regular * 0.875
                                    horizontalPadding: 8
                                    verticalPadding: 2
                                    text: "Delete"
                                    onClicked: {
                                        var result = modelData.erase()
                                        recordingResultPopupLabel.text = result
                                    }
                                }
                            }
                        }
                    }

                    Popup {
                        id: recordingResultPopup
                        anchors.centerIn: parent
                        width: recordingResultPopupRectangle.width
                        height: recordingResultPopupRectangle.height
                        modal: true
                        Overlay.modal: Rectangle {
                            color: ColorUtils.setAlpha(LithPalette.regular.window, 0.7)
                        }
                        onVisibleChanged: {
                            if (visible)
                                recordingResultPopupTimer.start()
                            else
                                recordingResultPopupLabel.text = ""
                        }
                        Rectangle {
                            id: recordingResultPopupRectangle
                            color: LithPalette.regular.window
                            border.color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.3)
                            border.width: 1
                            implicitWidth: recordingResultPopupLabel.width + 2 * recordingResultPopupLabel.x
                            implicitHeight: recordingResultPopupLabel.implicitHeight + 2 * recordingResultPopupLabel.y
                            Label {
                                id: recordingResultPopupLabel
                                x: 18
                                y: 12
                                width: Math.min(root.width, font.pixelSize * 32)
                                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                                onTextChanged: (text) => {
                                    if (text.length > 0)
                                        recordingResultPopup.visible = true
                                }
                                onLinkActivated: (link) => {
                                    ClipboardProxy.setText(link)
                                    // oof, this is not nice
                                    text += "<br><font color=\"%1\">%2</font>".arg(ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.6)).arg(qsTr("Path copied to clipboard."))
                                }
                            }
                        }
                        Timer {
                            id: recordingResultPopupTimer
                            interval: 10000
                            running: false
                            onTriggered: {
                                recordingResultPopup.visible = false
                            }
                        }
                    }
                }
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }
}
