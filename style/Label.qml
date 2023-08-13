import QtQuick
import QtQuick.Templates as T

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

    color: palette.windowText

    font.pointSize: {
        switch (size) {
        case Label.Tiny:
            return lith.settings.baseFontSize * 0.75
        case Label.Small:
            return lith.settings.baseFontSize * 0.875
        case Label.Regular:
            return lith.settings.baseFontSize
        case Label.Medium:
            return lith.settings.baseFontSize * 1.125
        case Label.Large:
            return lith.settings.baseFontSize * 1.25
        case Label.Header:
            return lith.settings.baseFontSize * 1.375
        }
    }
}
