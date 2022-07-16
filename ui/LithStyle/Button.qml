import QtQuick 2.12
import QtQuick.Templates 2.15 as T

T.Button {
    id: control

    implicitWidth: 100
    implicitHeight: 40

    icon.width: 28
    icon.height: 28

    font.pointSize: lith.settings.baseFontSize

    flat: true

    SystemPalette {
        id: palette
    }

    contentItem: Item {
        Rectangle {
            anchors.fill: parent
            color: palette.button
            radius: 2
            visible: !control.flat
        }
        Rectangle {
            anchors.fill: parent
            color: palette.text
            radius: 2
            opacity: control.pressed ? 0.3 : control.hovered ? 0.2 : 0.0
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
            anchors.centerIn: parent
            anchors.margins: 8
            source: control.icon.source
            width: control.icon.width
            height:control.icon.height
        }
    }

    background: Item {
    }
}
