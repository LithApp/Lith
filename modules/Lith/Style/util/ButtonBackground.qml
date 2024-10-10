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

    color: "transparent"

    Rectangle {
        anchors.fill: parent
        color: {
            if (control.pressed)
                return ColorUtils.mixColors(LithPalette.regular.button, "black", 0.5)
            if (control.hovered)
                return ColorUtils.mixColors(LithPalette.regular.base, LithPalette.regular.button, 0.6)
            return LithPalette.regular.button
        }
        border.color: hovered ? ColorUtils.mixColors(LithPalette.regular.window, "white", 0.9)
                              : flat ? "transparent"
                                     : ColorUtils.mixColors(LithPalette.regular.window, LithPalette.regular.text, 0.9)
        border.width: 0.5
        radius: parent.radius
        opacity: control.hovered || !flat ? 1 : 0
        Behavior on color {
            ColorAnimation {
                duration: 100
            }
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }
    }
}
