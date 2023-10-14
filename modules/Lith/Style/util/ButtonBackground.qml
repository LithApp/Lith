import QtQuick

import Lith.Style
import Lith.Core
import Lith.UI

Rectangle {
    id: backgroundRect

    property var control: null

    radius: ControlProperties.buttonRadius

    property bool flat: backgroundRect.control?.flat || false
    property bool down: backgroundRect.control?.down || false
    property bool pressed: backgroundRect.control?.pressed || false
    property bool hovered: backgroundRect.control?.hovered || false

    readonly property real themeQuotient: WindowHelper.lightTheme ? 0.0 : WindowHelper.useBlack ? 0.5 : 0.5

    border.color: {
        if (backgroundRect.control?.activeFocus || backgroundRect.activeFocus)
            return ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.highlight, 0.5)
        ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.text, 0.9)
    }
    border.width: backgroundRect.flat ? 0 : 0.5

    color: {
        if (backgroundRect.flat)
            return LithPalette.regular.window
        if (!backgroundRect.enabled) {
            if (backgroundRect.down) {
                return ColorUtils.darken(LithPalette.disabled.button, 1.1 + backgroundRect.themeQuotient)
            }
            else {
                return LithPalette.disabled.button
            }
        }
        return LithPalette.regular.button
    }
    property color startColor: {
        if (!backgroundRect.enabled)
            return backgroundRect.color
        if (backgroundRect.pressed)
            return ColorUtils.darken(LithPalette.regular.button, 1.4 + backgroundRect.themeQuotient)
        if (backgroundRect.hovered || !backgroundRect.flat)
            return ColorUtils.lighten(LithPalette.regular.button, 1.15 + 0.2 * backgroundRect.themeQuotient)
        return backgroundRect.color
    }
    property color endColor: {
        if (!backgroundRect.enabled)
            return backgroundRect.color
        if (backgroundRect.pressed)
            return ColorUtils.darken(LithPalette.regular.button, 1.2 + backgroundRect.themeQuotient)
        if (backgroundRect.hovered)
            return ColorUtils.lighten(LithPalette.regular.button, 1.2 + 0.2 * backgroundRect.themeQuotient)
        return backgroundRect.color
    }

    Behavior on startColor { ColorAnimation { duration: 100 } }
    Behavior on endColor { ColorAnimation { duration: 100 } }
    gradient: Gradient {
        GradientStop { position: 0.0; color: backgroundRect.startColor }
        GradientStop { position: 0.2; color: backgroundRect.endColor }
    }
}
