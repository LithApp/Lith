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
        color: control.checked ? LithPalette.regular.highlightedText : LithPalette.regular.text
        elide: Label.ElideRight
    }

    background: Rectangle {
        // border.color: control.highlighted ? ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.5) : "transparent"
        color: control.pressed     ? ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.1) :
               control.hovered     ? ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.2) :
                                      ColorUtils.mixColors(LithPalette.regular.base, LithPalette.regular.window, 0.5)
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 1
            anchors.bottomMargin: anchors.topMargin
            anchors.leftMargin: 3
            anchors.rightMargin: anchors.leftMargin
            color: control.checked     ? ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.window, 0.7) : "transparent"
            border.color: control.highlighted ? control.checked ? ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.text, 0.7) : ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.window, 0.5) : "transparent"
            border.width: 1.5
            radius: 4
        }
    }
}
