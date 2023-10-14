import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style
import Lith.UI

T.Switch {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    property color onColor: LithPalette.regular.highlight
    property color offColor: LithPalette.regular.window

    font.pixelSize: FontSizes.regular

    indicator: Rectangle {
        id: backgroundRect
        implicitWidth: 44
        implicitHeight: 24

        radius: height / 2

        border.color: ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.text, 0.9)
        border.width:  control.flat ? 0 : 0.5
        readonly property real themeQuotient: WindowHelper.lightTheme ? 0.0 : WindowHelper.useBlack ? 0.5 : 0.5
        color: {
            if (control.flat)
                return LithPalette.regular.window
            if (!control.enabled) {
                if (control.down) {
                    return ColorUtils.darken(LithPalette.disabled.button, 1.1 + backgroundRect.themeQuotient)
                }
                else {
                    return LithPalette.disabled.button
                }
            }
            return LithPalette.regular.button
        }
        property color startColor: {
            if (!control.enabled)
                return backgroundRect.color
            if (control.pressed)
                return ColorUtils.darken(LithPalette.regular.button, 1.4 + backgroundRect.themeQuotient)
            if (control.hovered || !control.flat)
                return ColorUtils.lighten(LithPalette.regular.button, 1.15 + 0.2 * backgroundRect.themeQuotient)
            return backgroundRect.color
        }
        property color endColor: {
            if (!control.enabled)
                return backgroundRect.color
            if (control.pressed)
                return ColorUtils.darken(LithPalette.regular.button, 1.2 + backgroundRect.themeQuotient)
            if (control.hovered)
                return ColorUtils.lighten(LithPalette.regular.button, 1.2 + 0.2 * backgroundRect.themeQuotient)
            return backgroundRect.color
        }
        Behavior on startColor { ColorAnimation { duration: 100 } }
        Behavior on endColor { ColorAnimation { duration: 100 } }
        gradient: Gradient {
            GradientStop { position: 0.0; color: backgroundRect.startColor }
            GradientStop { position: 0.6; color: backgroundRect.endColor }
        }

        Rectangle {
            x: control.checked ? parent.width - width : 0
            Behavior on x { NumberAnimation { duration: 100 } }
            width: parent.height
            height: parent.height
            radius: height / 2
            color: ColorUtils.mixColors((control.checked ? control.onColor : control.offColor), LithPalette.regular.window, control.hovered ? 0.9 : 1.0)
            Behavior on color { ColorAnimation { duration: 100 } }
            border.color: ColorUtils.mixColors(LithPalette.regular.midlight, LithPalette.regular.window, control.enabled ? 1.0 : 0.5)
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
