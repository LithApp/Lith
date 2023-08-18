import QtQuick
import QtQuick.Templates as T

T.BusyIndicator {
    id: control
    contentItem: Item {
        id: container
        implicitHeight: 40
        implicitWidth: 40
        Item {
            anchors.centerIn: parent
            Repeater {
                model: 8
                delegate: Rectangle {
                    color: palette.text
                    width: control.width * 0.22
                    height: width
                    radius: width / 2
                    x: Math.sin(index * Math.PI / 4) * width * 1.6 - width / 2
                    y: Math.cos(index * Math.PI / 4) * width * 1.6 - width / 2
                }
            }
            NumberAnimation on rotation {
                from: 0
                to: 360
                duration: 1000
                loops: NumberAnimation.Infinite
                easing {
                    type: Easing.InOutQuad
                    amplitude: 1.0
                }
            }
        }
    }
}
