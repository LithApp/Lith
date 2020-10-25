import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0


RowLayout {
    spacing: 0
    Button {
        Layout.preferredWidth: height
        text: "⇥"
        font.pointSize: 20
        focusPolicy: Qt.NoFocus
    }
    ChannelTextInput {
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
        font.pointSize: 16
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
