import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0

Frame {
    background: Rectangle {
        color: palette.window
    }

    RowLayout {
        width: parent.width
        Button {
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: height
            font.pointSize: 20
            text: "☰"
            onClicked: bufferDrawer.visible = !bufferDrawer.visible
        }
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                height: 1
                font.bold: true
                font.family: "Menlo"
                font.pointSize: stuff.selected && stuff.selected.title.length > 0 ? 14 : 18
                color: palette.windowText
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: stuff.selected ? stuff.selected.name : ""
            }
            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: stuff.selected && stuff.selected.title.length > 0
                clip: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: stuff.selected ? stuff.selected.title : ""
                elide: Text.ElideRight
                maximumLineCount: 2
                font.family: "Menlo"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: palette.windowText
            }
        }
        Item { width: 1 }
        Button {
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: height
            font.pointSize: 20
            visible: weechat.status !== Weechat.UNCONFIGURED
            enabled: weechat.status === Weechat.CONNECTED
            text: weechat.status === Weechat.CONNECTING ? "🤔" :
                  weechat.status === Weechat.CONNECTED ? "🙂" :
                  weechat.status === Weechat.DISCONNECTED ? "😴" :
                  weechat.status === Weechat.ERROR ? "☠" :
                                                     "😱"
            onClicked: nickDrawer.visible = !nickDrawer.visible
        }
    }
}
