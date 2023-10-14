import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

import "util"

T.TextArea {
    id: control

    implicitWidth: implicitBackgroundWidth + leftInset + rightInset
                   || contentWidth + leftPadding + rightPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    padding: 6
    color: LithPalette.regular.text
    selectionColor: LithPalette.regular.highlight
    placeholderTextColor: LithPalette.disabled.text
    verticalAlignment: Text.AlignVCenter

    font.pixelSize: FontSizes.regular

    property color borderColor: "transparent"
    property color backgroundColor: "transparent"
    property real animationSpeed: 300

    background: InputBackground {
        control: control

        Label {
            visible: control.text.length === 0 && !control.activeFocus
            width: parent.width
            height: parent.height
            elide: Text.ElideRight
            verticalAlignment: control.verticalAlignment
            horizontalAlignment: control.horizontalAlignment
            leftPadding: control.leftPadding
            rightPadding: control.rightPadding
            font.pixelSize: control.font.pixelSize
            color: control.placeholderTextColor
            text: control.placeholderText
        }
    }
}
