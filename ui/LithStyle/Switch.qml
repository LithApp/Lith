import QtQuick 2.12
import QtQuick.Templates 2.12 as T

T.Switch {
    id: control
    implicitWidth: 58
    implicitHeight: 40

    indicator: Item {
        implicitWidth: 48
        implicitHeight: 28
        x: 6
        y: 6
        Rectangle {
            anchors.fill: parent

            radius: height / 2
            //color: palette.text
            color: control.checked ? "#2ed058" : palette.text
            opacity: control.checked ? 0.90 : 0.06
        }

        Rectangle {
            x: control.checked ? parent.width - width : 0
            Behavior on x { NumberAnimation { duration: 100 } }
            width: 28
            height: 28
            radius: 13
            color: palette.button
            border.color: palette.buttonText
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.down ? "#17a81a" : "#21be2b"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
