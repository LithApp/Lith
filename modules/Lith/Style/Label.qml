import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

T.Label {
    color: enabled ? LithPalette.regular.windowText : LithPalette.disabled.windowText
    font.pixelSize: FontSizes.regular
}
