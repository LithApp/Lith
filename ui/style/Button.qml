import QtQuick 2.12
import QtQuick.Controls 2.12 as Controls

Controls.AbstractButton {
    id: control

    implicitWidth: 100
    implicitHeight: 40

    font.pointSize: lith.settings.baseFontSize

    SystemPalette {
        id: palette
    }

    contentItem: Item {
        Rectangle {
            anchors.fill: parent
            color: palette.text
            radius: 2
            opacity: control.pressed ? 0.3 : control.hovered ? 0.1 : 0.0
            Behavior on opacity { NumberAnimation { duration: 100 } }
        }
        Text {
            text: control.text
            font: control.font
            anchors.centerIn: parent
            opacity: enabled ? 1.0 : 0.3
            color: palette.windowText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        Image {
            mipmap: true
            antialiasing: true
            anchors.fill: parent
            anchors.margins: 8
            source: control.icon.source
        }
    }

    background: Item {
    }
}
