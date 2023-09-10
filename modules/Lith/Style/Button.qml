import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

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

            x: control.text.length > 0 ? control.leftPadding : (parent.width - width) / 2
            height: control.icon.height
            width: control.icon.width
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
                    return LithPalette.disabled.text
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
        border.color: ColorUtils.mixColors(LithPalette.regular.button, LithPalette.regular.text, 0.9)
        border.width:  control.flat ? 0 : 0.5
        readonly property real themeQuotient: WindowHelper.lightTheme ? 0.0 : WindowHelper.useBlack ? 0.5 : 0.5
        color: {
            if (control.flat)
                return LithPalette.regular.window
            if (!control.enabled) {
                if (control.down) {
                    return ColorUtils.darken(LithPalette.disabled.button, 1.1 + backgroundRect.themeQuotient)
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
                return ColorUtils.darken(LithPalette.regular.button, 1.4 + backgroundRect.themeQuotient)
            if (control.hovered || !control.flat)
                return ColorUtils.lighten(LithPalette.regular.button, 1.15 + 0.2 * backgroundRect.themeQuotient)
            return backgroundRect.color
        }
        property color endColor: {
            if (!control.enabled)
                return backgroundRect.color
            if (control.pressed)
                return ColorUtils.darken(LithPalette.regular.button, 1.2 + backgroundRect.themeQuotient)
            if (control.hovered)
                return ColorUtils.lighten(LithPalette.regular.button, 1.2 + 0.2 * backgroundRect.themeQuotient)
            return backgroundRect.color
        }
        Behavior on startColor { ColorAnimation { duration: 100 } }
        Behavior on endColor { ColorAnimation { duration: 100 } }
        gradient: Gradient {
            GradientStop { position: 0.0; color: backgroundRect.startColor }
            GradientStop { position: 0.2; color: backgroundRect.endColor }
        }
    }
}
