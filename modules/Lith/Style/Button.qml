import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import Lith.Core

T.Button {
    id: control

    property real minimumWidth: text.length > 0 ? 100 : 0
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            minimumWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 3
    spacing: 6

    icon.width: 28
    icon.height: 28

    font.pointSize: Lith.settings.baseFontSize

    contentItem: Item {

        implicitHeight: {
            const iconHeight = control.icon.height + control.topPadding + control.bottomPadding
            const labelHeight = buttonLabel.implicitHeight + control.topPadding + control.bottomPadding
            return Math.max(iconHeight, labelHeight)
        }
        implicitWidth: (control.icon.source.length > 0 ? control.icon.width + control.spacing + control.leftPadding + control.rightPadding : 0) + buttonLabel.implicitWidth

        Image {
            id: buttonIcon
            source: control.icon.source
            sourceSize: Qt.size(control.icon.width, control.icon.height)

            x: control.leftPadding
            height: parent.height - control.topPadding - control.bottomPadding
            width: height
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

            text: control.text
            font: control.font
            color: {
                if (!control.enabled)
                    return LithPalette.disabled.buttonText
                if (control.checked || control.highlighted)
                    return control.LithPalette.regular.brightText
                if (control.flat && !control.down) {
                    if (control.visualFocus)
                        return control.LithPalette.regular.highlight
                    else
                        return control.LithPalette.regular.windowText
                }
                return LithPalette.regular.buttonText
            }
        }
    }

    background: Rectangle {
        id: backgroundRect
        radius: 3
        color: {
            if (control.flat)
                return LithPalette.regular.window
            if (!control.enabled) {
                if (control.down) {
                    return ColorUtils.darken(LithPalette.disabled.button, 1.1)
                }
                else {
                    return LithPalette.disabled.button
                }
            }
            return LithPalette.regular.button
        }
        property color startColor: {
            if (!control.enabled)
                return backgroundRect.color
            if (control.pressed)
                return ColorUtils.darken(LithPalette.regular.button, 1.1)
            if (control.hovered || !control.flat)
                return ColorUtils.lighten(LithPalette.regular.button, 1.5)
            return backgroundRect.color
        }
        property color endColor: {
            if (!control.enabled)
                return backgroundRect.color
            if (control.hovered && !control.pressed)
                return ColorUtils.lighten(LithPalette.regular.button, 1.5)
            return backgroundRect.color
        }
        Behavior on startColor { ColorAnimation { duration: 100 } }
        Behavior on endColor { ColorAnimation { duration: 100 } }
        gradient: Gradient {
            GradientStop { position: 0.0; color: backgroundRect.startColor }
            GradientStop { position: 0.6; color: backgroundRect.endColor }
        }
    }
}
