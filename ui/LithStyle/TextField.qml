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

    property color borderColor: "transparent"
    property color backgroundColor: "transparent"
    property real animationSpeed: 300

    cursorDelegate: Item {
        width: 3
        height: control.font.pixelSize + 2
        Rectangle {
            visible: control.cursorVisible
            color: control.color
            anchors {
                fill: parent
                leftMargin: 2
            }
            SequentialAnimation on opacity {
                loops: Animation.Infinite
                running: true
                NumberAnimation {
                    from: 0
                    to: 1
                    duration: control.animationSpeed
                    easing.type: Easing.OutQuad
                }
                PauseAnimation {
                    duration: control.animationSpeed * 2
                }
                NumberAnimation {
                    from: 1
                    to: 0
                    duration: control.animationSpeed
                    easing.type: Easing.OutQuad
                }
                PauseAnimation {
                    duration: control.animationSpeed * 2
                }
            }
        }
    }

    background: Item {
        implicitWidth: 200
        implicitHeight: 40

        Rectangle {
            border {
                color: control.borderColor
                width: 1
            }

            color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.06)
            anchors.fill: parent
        }

        Text {
            visible: control.text.length === 0
            anchors {
                fill: parent
                margins: 6
                leftMargin: 7
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: control.horizontalAlignment
            font.pointSize: control.font.pointSize
            color: control.placeholderTextColor

            text: control.placeholderText
        }
    }
}
