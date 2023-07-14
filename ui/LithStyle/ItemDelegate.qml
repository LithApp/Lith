import QtQuick
import QtQuick.Templates as T

T.ItemDelegate {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 8
    spacing: 6

    icon.width: 24
    icon.height: 24
    icon.color: control.palette.text

    contentItem: Label {
        verticalAlignment: Label.AlignVCenter
        text: control.text
        font: control.font
        color: control.checked ? palette.highlightedText : palette.text
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        color: control.checked     ? colorUtils.setAlpha(palette.highlight, 0.7) :
               control.highlighted ? colorUtils.setAlpha(palette.highlight, 0.6) :
               control.pressed     ? colorUtils.setAlpha(palette.text, 0.1) :
               control.hovered     ? colorUtils.setAlpha(palette.text, 0.2) :
                                     palette.base
    }
}
