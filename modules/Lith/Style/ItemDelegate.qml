import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

T.ItemDelegate {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 10
    spacing: 6

    icon.width: 24
    icon.height: 24
    icon.color: control.LithPalette.regular.text

    font.pointSize: Lith.settings.baseFontSize

    property alias textFormat: label.textFormat

    contentItem: Label {
        id: label
        verticalAlignment: Label.AlignVCenter
        text: control.text
        font: control.font
        color: control.checked ? LithPalette.regular.highlightedText : LithPalette.regular.text
        size: Label.Large
    }

    background: Rectangle {
        implicitWidth: 64
        implicitHeight: 32
        color: control.checked     ? ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.window, 0.7) :
               control.highlighted ? ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.window, 0.4) :
               control.pressed     ? ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.1) :
               control.hovered     ? ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.2) :
                                      ColorUtils.mixColors(LithPalette.regular.base, LithPalette.regular.window, 0.5)
    }
}
