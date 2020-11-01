import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2

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
        border.color: Qt.darker(palette.button)
        color: palette.button

        Rectangle {
            width: 12
            height: 12
            anchors.centerIn: parent
            radius: 2
            color: palette.windowText
            visible: control.checked
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
