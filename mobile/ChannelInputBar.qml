import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0


RowLayout {
    spacing: 0
    // TODO
    property alias textInput: channelTextInput
    property bool hasFocus: channelTextInput.activeFocus
    Button {
        Layout.preferredWidth: height
        text: "⇥"
        font.pointSize: settings.baseFontSize * 1.25
        focusPolicy: Qt.NoFocus
    }
    ChannelTextInput {
        id: channelTextInput
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
    }

    Button {
        focusPolicy: Qt.NoFocus
        id: imageButton
        Layout.preferredWidth: height
        property bool isBusy: uploader.working
        text: isBusy ? "" : "📷"
        enabled: !isBusy
        font.pointSize: settings.baseFontSize
        onClicked: {
            fileDialog.open()
        }
        BusyIndicator {
            id: busy
            visible: parent.isBusy
            anchors.fill: parent
            scale: 0.7
        }
        Connections {
            target: uploader
            function onSuccess(url) {
                console.warn("FINISHED")
                console.warn(url)
                if (channelTextInput.length !== 0 && !channelTextInput.text.endsWith(" "))
                    channelTextInput.text += " "
                channelTextInput.text += url
                channelTextInput.text += " "
            }
            function onError(message) {
                console.warn("ERROR")
                console.warn(message)
            }
        }

    }

    Button {
        focusPolicy: Qt.NoFocus
        id: sendButton
        Layout.preferredWidth: height
        text: "↳"
        visible: settings.showSendButton
        font.pointSize: settings.baseFontSize
        onClicked: {
            if (channelTextInput.inputFieldAlias.text.length > 0) {
                stuff.selected.input(channelTextInput.inputFieldAlias.text)
                channelTextInput.inputFieldAlias.text = ""
            }
        }
    }
}
