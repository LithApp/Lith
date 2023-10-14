import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style
import Lith.UI

T.Switch {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    property color onColor: LithPalette.regular.highlight
    property color offColor: LithPalette.regular.window

    font.pixelSize: FontSizes.regular

    indicator: ButtonBackground {
        id: backgroundRect
        control: control

        implicitWidth: 44
        implicitHeight: 24

        radius: height / 2

        Rectangle {
            x: control.checked ? parent.width - width : 0
            Behavior on x { NumberAnimation { duration: 100 } }
            width: parent.height
            height: parent.height
            radius: height / 2
            color: ColorUtils.mixColors((control.checked ? control.onColor : control.offColor), LithPalette.regular.window, control.hovered ? 0.9 : 1.0)
            Behavior on color { ColorAnimation { duration: 100 } }
            border.color: ColorUtils.mixColors(LithPalette.regular.midlight, LithPalette.regular.window, control.enabled ? 1.0 : 0.5)
            border.width: 1
        }
    }

    contentItem: Label {
        text: control.text
        font: control.font
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
