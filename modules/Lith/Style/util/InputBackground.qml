import QtQuick

import Lith.Style
import Lith.UI
import Lith.Core

Rectangle {
    id: inputBackground

    property var control: null

    property color backgroundColor: inputBackground.control?.backgroundColor || ColorUtils.mixColors(LithPalette.regular.window, LithPalette.regular.base, enabled ? 0.5 : 0.95)
    property color borderColor: inputBackground.control?.borderColor || "transparent"

    readonly property color actualBorderColor: {
        if (inputBackground.borderColor != Qt.color("transparent"))
            return control.borderColor
        if (inputBackground.control?.activeFocus || inputBackground.activeFocus)
            return ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.highlight, 0.5)
        return ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.text, 0.9)
    }

    border {
        color: inputBackground.actualBorderColor
        width: 1
    }

    color: inputBackground.backgroundColor
}
