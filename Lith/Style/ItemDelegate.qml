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
    icon.color: control.palette.text

    font.pointSize: Lith.settings.baseFontSize

    contentItem: Label {
        verticalAlignment: Label.AlignVCenter
        text: control.text
        font: control.font
        color: control.checked ? palette.highlightedText : palette.text
        size: Label.Large
    }

    background: Rectangle {
        implicitWidth: 64
        implicitHeight: 32
        color: control.checked     ? colorUtils.setAlpha(palette.highlight, 0.7) :
               control.highlighted ? colorUtils.setAlpha(palette.highlight, 0.6) :
               control.pressed     ? colorUtils.setAlpha(palette.text, 0.1) :
               control.hovered     ? colorUtils.setAlpha(palette.text, 0.2) :
                                     palette.base
    }
}
