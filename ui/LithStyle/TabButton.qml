import QtQuick
import QtQuick.Templates as T

T.TabButton {
    id: control
    implicitWidth: 200
    implicitHeight: 40

    padding: 6

    property color backgroundColor: "transparent"

    SystemPalette {
        id: palette
    }

    contentItem: Item {
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
        Rectangle {
            anchors.fill: parent
            color: palette.window
            radius: 2
            visible: !control.flat
        }
        Rectangle {
            anchors.fill: parent
            color: palette.text
            radius: 2
            opacity: control.pressed ? 0.4 : control.checked ? 0.0 : control.hovered ? 0.2 : 0.3
            Behavior on opacity { NumberAnimation { duration: 100 } }
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 3
            color: palette.highlight
            visible: control.checked
        }
    }
}
