import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC2

QQC2.AbstractButton {
    id: control
    checked: true
    implicitWidth: 40
    implicitHeight: 40

    indicator: Rectangle {
        implicitWidth: 26
        implicitHeight: 26
        anchors.verticalCenter: parent.verticalCenter
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 2
        color: palette.button

        Item {
            anchors.centerIn: parent
            rotation: 45

            Rectangle {
                width: 18
                height: 4.5
                radius: 2
                anchors.centerIn: parent
                color: palette.windowText
                visible: control.checked
            }

            Rectangle {
                width: 4.5
                height: 18
                radius: 2
                anchors.centerIn: parent
                color: palette.windowText
                visible: control.checked
            }

        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: palette.windowText
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
