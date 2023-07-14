import QtQuick 2.12
import QtQuick.Controls.impl
import QtQuick.Templates 2.15 as T

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            100)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    horizontalPadding: text.length > 0 ? 12 : 6
    verticalPadding: text.length > 0 ? 12 : 6
    spacing: 6

    icon.width: 28
    icon.height: 28

    font.pointSize: lith.settings.baseFontSize

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        opacity: control.enabled ? 1.0 : 0.5

        icon: control.icon
        text: control.text
        font: control.font
        color: control.checked || control.highlighted ? control.palette.brightText :
               control.flat && !control.down ? (control.visualFocus ? control.palette.highlight : control.palette.windowText) : control.palette.buttonText
    }

    background: Rectangle {
        clip: true
        color: !control.flat || control.hovered || control.pressed ? palette.button : palette.window
        Behavior on color { ColorAnimation { duration: 100 } }
        radius: 3

        Rectangle {
            id: shadow
            visible: control.enabled
            anchors.fill: parent
            radius: parent.radius
            opacity: control.pressed ? 0.15 : control.hovered ? 0.15 : control.flat ? 0.0 : 0.06
            Behavior on opacity { NumberAnimation { duration: 100 } }
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
            visible: false
            anchors.fill: parent
            color: palette.text
            radius: parent.radius
            opacity: control.pressed ? 0.3 : control.hovered ? 0.2 : 0.0
            Behavior on opacity { NumberAnimation { duration: 100 } }
        }
    }
}
