import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0


RowLayout {
    spacing: 0
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
        property bool isBusy: false
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
    }
}
