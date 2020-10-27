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
            font.pointSize: settings.baseFontSize * 1.25
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
                font.pointSize: stuff.selected && stuff.selected.title.length > 0 ? settings.baseFontSize * 0.875 :
                                                                                    settings.baseFontSize * 1.125
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
                font.pointSize: settings.baseFontSize * 0.75
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: palette.windowText
            }
        }
        Item { width: 1 }
        Button {
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: height
            font.pointSize: settings.baseFontSize * 1.25
            visible: lith.status !== Lith.UNCONFIGURED
            enabled: lith.status === Lith.CONNECTED
            text: lith.status === Lith.CONNECTING   ? "🤔" :
                  lith.status === Lith.CONNECTED    ? "🙂" :
                  lith.status === Lith.DISCONNECTED ? "😴" :
                  lith.status === Lith.ERROR        ? "☠" :
                                                      "😱"
            onClicked: nickDrawer.visible = !nickDrawer.visible
        }
    }
}
