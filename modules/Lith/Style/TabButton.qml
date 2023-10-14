import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import Lith.Core
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

    font.pixelSize: FontSizes.regular

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.checked ? LithPalette.regular.windowText : ColorUtils.mixColors(LithPalette.regular.windowText, LithPalette.regular.window, 0.75)

    }

    background: Rectangle {
        color: {
            if (control.flat)
                return "transparent"
            return ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.buttonText, control.pressed ? 0.75 : control.hovered ? 0.9 : 1.0)
        }
        radius: 2

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 4
            color: control.checked ? LithPalette.regular.highlight : ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.text, 0.75)
        }
    }
}
