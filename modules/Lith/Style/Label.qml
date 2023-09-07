import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

T.Label {
    enum Size {
        Tiny,
        Small,
        Regular,
        Medium,
        Large,
        Header
    }
    property int size: Label.Size.Regular

    color: LithPalette.regular.windowText

    font.pointSize: {
        switch (size) {
        case Label.Tiny:
            return Lith.settings.baseFontSize * 0.75
        case Label.Small:
            return Lith.settings.baseFontSize * 0.875
        case Label.Regular:
            return Lith.settings.baseFontSize
        case Label.Medium:
            return Lith.settings.baseFontSize * 1.125
        case Label.Large:
            return Lith.settings.baseFontSize * 1.25
        case Label.Header:
            return Lith.settings.baseFontSize * 1.375
        }
        return Lith.settings.baseFontSize
    }
}
