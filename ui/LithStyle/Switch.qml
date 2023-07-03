import QtQuick 2.12
import QtQuick.Templates 2.12 as T

T.Switch {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    property color onColor: palette.highlight
    property color offColor: palette.window

    indicator: Rectangle {
        implicitWidth: 44
        implicitHeight: 24

        radius: height / 2
        color: palette.button

        Rectangle {
            id: shadow
            visible: control.enabled
            anchors.fill: parent
            radius: parent.radius
            opacity: control.pressed ? 0.12 : control.hovered ? 0.08 : 0.06
            property color startColor: control.pressed ? "black" : "white"
            Behavior on startColor { ColorAnimation { duration: 100 } }
            property color endColor: control.hovered && !control.pressed ? "white" : "transparent"
            Behavior on endColor { ColorAnimation { duration: 100 } }
            gradient: Gradient {
                GradientStop { position: 0.0; color: shadow.startColor }
                GradientStop { position: 0.6; color: shadow.endColor }
            }
        }

        Rectangle {
            x: control.checked ? parent.width - width : 0
            Behavior on x { NumberAnimation { duration: 100 } }
            width: parent.height
            height: parent.height
            radius: height / 2
            color: control.checked ? control.onColor : control.offColor
            Behavior on color { ColorAnimation { duration: 100 } }
            border.color: palette.midlight
            border.width: 1
        }
    }

    contentItem: Label {
        text: control.text
        font: control.font
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
