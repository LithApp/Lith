import QtQuick 2.12
import QtQuick.Templates 2.12 as T

T.TextField {
    id: control
    implicitWidth: 200
    implicitHeight: 40

    padding: 6
    color: palette.text
    selectionColor: palette.highlight
    placeholderTextColor: disabledPalette.text
    verticalAlignment: Text.AlignVCenter


    property color backgroundColor: "transparent"

    background: Item {
        implicitWidth: 200
        implicitHeight: 40

        Text {
            visible: control.text.length === 0
            anchors {
                fill: parent
                margins: 6
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: control.horizontalAlignment
            font.pointSize: control.font.pointSize
            color: control.placeholderTextColor

            text: control.placeholderText
        }

        Rectangle {
            color: palette.text
            opacity: 0.06
            anchors.fill: parent
        }
    }
}
