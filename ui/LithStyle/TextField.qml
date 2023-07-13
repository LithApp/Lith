import QtQuick 2.12
import QtQuick.Templates 2.12 as T

T.TextField {
    id: control

    implicitWidth: implicitBackgroundWidth + leftInset + rightInset
                   || contentWidth + leftPadding + rightPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    padding: 6
    color: palette.text
    selectionColor: palette.highlight
    placeholderTextColor: colorUtils.setAlpha(disabledPalette.text, 0.5)
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

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 40

        property color actualBorderColor: {
            if (control.borderColor != Qt.color("transparent"))
                return control.borderColor
            if (control.activeFocus)
                return palette.midlight
            return palette.button
        }

        border {
            color: actualBorderColor
            width: 1
        }

        color: palette.alternateBase
        anchors.fill: parent

        Label {
            visible: control.text.length === 0 && !control.activeFocus
            width: parent.width
            height: parent.height
            elide: Text.ElideRight
            verticalAlignment: control.verticalAlignment
            horizontalAlignment: control.horizontalAlignment
            leftPadding: control.leftPadding
            rightPadding: control.rightPadding
            font.pointSize: control.font.pointSize
            color: control.placeholderTextColor
            text: control.placeholderText
        }
    }
}
