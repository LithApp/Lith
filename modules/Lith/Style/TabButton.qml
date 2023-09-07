import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import Lith.Style

T.TabButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    horizontalPadding: 24
    verticalPadding: 12
    spacing: 6

    icon.width: 28
    icon.height: 28

    property color backgroundColor: "transparent"

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.checked ? control.palette.windowText : control.palette.brightText
    }

    background: Rectangle {
        color: control.flat ? LithPalette.regular.button : "transparent"
        radius: 2

        Rectangle {
            anchors.fill: parent
            color: LithPalette.regular.text
            radius: 2
            opacity: control.pressed ? 0.3 : control.hovered ? 0.2 : 0.0
            Behavior on opacity { NumberAnimation { duration: 100 } }
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 4
            color: control.checked ? LithPalette.regular.highlight : LithPalette.regular.button
        }
    }
}
