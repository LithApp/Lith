import QtQuick
import QtQuick.Templates as T

import Lith.UI
import Lith.Style
import Lith.Core

import "util"

T.SpinBox {
    id: control

    editable: true

    // Note: the width of the indicators are calculated into the padding
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             up.implicitIndicatorHeight, down.implicitIndicatorHeight)

    leftPadding: padding + (control.mirrored ? (up.indicator ? up.indicator.width : 0) : (down.indicator ? down.indicator.width : 0))
    rightPadding: padding + (control.mirrored ? (down.indicator ? down.indicator.width : 0) : (up.indicator ? up.indicator.width : 0))

    font.pixelSize: FontSizes.regular

    validator: IntValidator {
        locale: control.locale.name
        bottom: Math.min(control.from, control.to)
        top: Math.max(control.from, control.to)
    }

    contentItem: TextField {
        text: control.displayText
        clip: width < implicitWidth
        padding: 6

        borderColor: backgroundColor
        font: control.font
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
    }

    up.indicator: Button {
        z: 2
        x: control.mirrored ? 0 : control.width - width
        height: control.height
        implicitWidth: 40
        implicitHeight: 40
        text: "+"
        onClicked: control.increase()
    }

    down.indicator: Button {
        z: 2
        x: control.mirrored ? parent.width - width : 0
        height: control.height
        implicitWidth: 40
        implicitHeight: 40
        text: "-"
        onClicked: control.decrease()
    }

    background: InputBackground {
        control: control
        implicitWidth: 140
        z: 1
        color: "transparent"
        radius: ControlProperties.buttonRadius
    }
}
