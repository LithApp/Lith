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

    verticalPadding: 12
    horizontalPadding: 12
    spacing: 9

    icon.width: 24
    icon.height: 24
    icon.color: control.LithPalette.regular.text

    font.pixelSize: FontSizes.regular

    property alias textFormat: label.textFormat

    contentItem: Label {
        id: label
        verticalAlignment: Label.AlignVCenter
        text: control.text
        font: control.font
        color: LithPalette.regular.text
        elide: Label.ElideRight
    }

    background: Rectangle {
        implicitWidth: 64
        implicitHeight: 32
        radius: Math.sqrt(Lith.settings.uiMargins, 0.9)
        border.color: control.highlighted || control.hovered ? ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.5) :
                                                               "transparent"
        Behavior on border.color {
            ColorAnimation {
                duration: 60
            }
        }
        color: control.pressed                       ? ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.base, 0.35) :
               control.checked                       ? ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.base, 0.25) :
                                                       LithPalette.regular.base
        Behavior on color {
            ColorAnimation {
                duration: 100
                easing.type: Easing.OutExpo
            }
        }
    }
}
