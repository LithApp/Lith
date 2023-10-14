import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

import "util"

T.RadioButton {
    id: control
    checked: true
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Math.max(implicitContentWidth, indicator.width) + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    spacing: 0
    padding: 6

    background: Item {}

    indicator: ButtonBackground {
        id: backgroundRect
        control: control

        implicitWidth: FontSizes.regular + 12
        implicitHeight: implicitWidth
        anchors.verticalCenter: parent.verticalCenter

        radius: width / 2

        Rectangle {
            anchors.centerIn: parent
            width: parent.width / 3
            height: width
            radius: width / 2
            color: ColorUtils.mixColors(LithPalette.regular.windowText, LithPalette.regular.button, control.enabled ? 1.0 : 0.4)
            visible: control.checked
        }
    }

    contentItem: Label {
        leftPadding: visible && control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: visible && control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0
        visible: text.length > 0
        text: control.text
        font: control.font
        color: ColorUtils.mixColors(LithPalette.regular.windowText, LithPalette.regular.window, enabled ? 1.0 : 0.3)
        verticalAlignment: Text.AlignVCenter
    }
}
