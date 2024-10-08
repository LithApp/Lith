import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

import "util"

T.Button {
    id: control

    property real minimumWidth: text.length > 0 ? 80 : 0
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            minimumWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 4
    spacing: 6

    icon.width: 26
    icon.height: 26

    font.pixelSize: FontSizes.regular
    property alias textHorizontalAlignment: buttonLabel.horizontalAlignment
    property alias textVerticalAlignment: buttonLabel.verticalAlignment

    contentItem: Item {

        implicitHeight: {
            const iconHeight = control.icon.height + control.topPadding + control.bottomPadding
            const labelHeight = buttonLabel.implicitHeight + control.topPadding + control.bottomPadding
            return Math.max(iconHeight, labelHeight)
        }
        implicitWidth: (control.icon.source.length > 0 ? control.icon.width + control.spacing + control.leftPadding + control.rightPadding : 0) + buttonLabel.implicitWidth

        IconImage {
            id: buttonIcon
            source: control.icon.source

            x: control.text.length > 0 ? control.leftPadding : (parent.width - width) / 2
            height: Math.min(control.height - 10, control.icon.height)
            width: Math.min(control.height - 10, control.icon.width)
            anchors.verticalCenter: parent.verticalCenter
            opacity: control.enabled ? 1.0 : 0.5
        }

        Label {
            id: buttonLabel

            padding: 3
            x: control.icon.source.length > 0 ? buttonIcon.width + control.spacing + 12 : 0
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - x
            horizontalAlignment: Label.AlignHCenter
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
            elide: Label.ElideRight
            maximumLineCount: 4

            text: control.text
            font: control.font
            color: {
                if (!control.enabled)
                    return LithPalette.disabled.text
                if (control.checked || control.highlighted)
                    return LithPalette.regular.highlightedText
                if (control.flat && !control.down) {
                    if (control.visualFocus)
                        return LithPalette.regular.highlight
                    else
                        return LithPalette.regular.windowText
                }
                return LithPalette.regular.buttonText
            }
        }
    }

    background: ButtonBackground {
        control: control
    }
}
