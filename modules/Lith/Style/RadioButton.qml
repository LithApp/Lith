import QtQuick
import QtQuick.Templates as T

import Lith.Core

T.RadioButton {
    id: control
    checked: true
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Math.max(implicitContentWidth, indicator.width) + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    spacing: 0
    padding: 6

    background: Item {}

    indicator: Rectangle {
        id: backgroundRect
        implicitWidth: 26 * Math.max(1.0, Lith.settings.baseFontSize / 20)
        implicitHeight: implicitWidth
        anchors.verticalCenter: parent.verticalCenter

        radius: width / 2
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
            anchors.centerIn: parent
            width: parent.width / 3
            height: width
            radius: width / 2
            color: ColorUtils.mixColors(LithPalette.regular.windowText, LithPalette.regular.button, control.enabled ? 1.0 : 0.4)
            visible: control.checked
        }
    }

    contentItem: Label {
        leftPadding: visible && control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: visible && control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0
        visible: text.length > 0
        text: control.text
        font: control.font
        color: ColorUtils.mixColors(LithPalette.regular.windowText, LithPalette.regular.window, enabled ? 1.0 : 0.3)
        verticalAlignment: Text.AlignVCenter
    }
}
