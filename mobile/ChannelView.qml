import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2 as Dialogs

import lith 1.0

ColumnLayout {
    spacing: 0
    SystemPalette {
        id: palette
    }

    property bool inputBarHasFocus: inputBar.hasFocus
    property alias textInput: inputBar.textInput

    ChannelHeader {
        id: channelHeader
        Layout.fillWidth: true
    }

    Text {
        Layout.fillHeight: true
        Layout.fillWidth: true
        visible: !stuff.selected
        text: "Welcome to Lith\n" +
              "Weechat status: " + weechat.status + "\n" +
              "Current error status: " + (weechat.errorString.length > 0 ? weechat.errorString : "None")
        color: palette.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        ChannelMessageList {
            id: channelMessageList
            width: parent.width
            height: parent.height
            visible: stuff.selected
        }
    }

    ChannelInputBar {
        id: inputBar
        Layout.fillWidth: true
    }

    Dialogs.FileDialog {
        id: fileDialog
        folder: shortcuts.pictures
        nameFilters: [ "Image files (*.jpg *.png)" ]
        onAccepted: {
            //inputField.text += " " + fileUrl
            //imageButton.isBusy = false
            uploader.upload(fileUrl)
            Qt.inputMethod.hide()
            inputBar.textInput.forceActiveFocus()
        }
        onRejected: {
            Qt.inputMethod.hide()
            inputBar.textInput.forceActiveFocus()
        }
    }
}
