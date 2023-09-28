import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

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

    font.pointSize: Lith.settings.baseFontSize

    property color borderColor: "transparent"
    property color backgroundColor: "transparent"
    property real animationSpeed: 300

    background: Rectangle {
        property color actualBorderColor: {
            if (control.borderColor != Qt.color("transparent"))
                return control.borderColor
            if (control.borderColor == control.backgroundColor)
                return control.borderColor
            if (control.activeFocus)
                return LithPalette.regular.midlight
            return LithPalette.regular.button
        }

        border {
            color: actualBorderColor
            width: 1
        }

        color: control.backgroundColor
        anchors.fill: parent

        Label {
            visible: control.text.length === 0 && !control.activeFocus
            width: parent.width
            height: parent.height
            elide: Text.ElideRight
            verticalAlignment: control.verticalAlignment
            horizontalAlignment: control.horizontalAlignment
            leftPadding: control.leftPadding
            rightPadding: control.rightPadding
            font.pointSize: control.font.pointSize
            color: control.placeholderTextColor
            text: control.placeholderText
        }
    }
}
