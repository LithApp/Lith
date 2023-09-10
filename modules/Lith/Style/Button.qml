import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import Lith.Core

T.Button {
    id: control

    property real minimumWidth: text.length > 0 ? 100 : 0
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            minimumWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 3
    spacing: 6

    icon.width: 28
    icon.height: 28

    font.pointSize: Lith.settings.baseFontSize

    contentItem: Item {
        opacity: control.enabled ? 1.0 : 0.5

        implicitHeight: {
            const iconHeight = control.icon.height + control.topPadding + control.bottomPadding
            const labelHeight = buttonLabel.implicitHeight + control.topPadding + control.bottomPadding
            return Math.max(iconHeight, labelHeight)
        }
        implicitWidth: (control.icon.source.length > 0 ? control.icon.width + control.spacing + control.leftPadding + control.rightPadding : 0) + buttonLabel.implicitWidth

        Image {
            id: buttonIcon
            source: control.icon.source

            x: control.leftPadding
            height: parent.height - control.topPadding - control.bottomPadding
            width: height
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            id: buttonLabel

            padding: 3
            x: control.icon.source.length > 0 ? buttonIcon.width + control.spacing + 12 : 0
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - x
            horizontalAlignment: Label.AlignHCenter

            text: control.text
            font: control.font
            color: control.checked || control.highlighted ? control.LithPalette.regular.brightText :
                   control.flat && !control.down ? (control.visualFocus ? control.LithPalette.regular.highlight : control.LithPalette.regular.windowText) : control.LithPalette.regular.buttonText
        }
    }

    background: Rectangle {
        color: !control.flat || (control.hovered && control.enabled) || control.pressed ? LithPalette.regular.button : LithPalette.regular.window
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
            color: LithPalette.regular.text
            radius: parent.radius
            opacity: control.pressed ? 0.3 : control.hovered ? 0.2 : 0.0
            Behavior on opacity { NumberAnimation { duration: 100 } }
        }
    }
}
