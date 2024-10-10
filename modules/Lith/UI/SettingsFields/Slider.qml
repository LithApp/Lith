import QtQuick
import QtQuick.Layouts

import Lith.Style as LithStyle

Base {
    id: root
    property alias value: slider.value
    property alias from: slider.from
    property alias to: slider.to
    property alias stepSize: slider.stepSize

    property int readingPrecision: 0
    signal valueEdited

    rowComponent: LithStyle.Slider {
        id: slider
        Layout.fillWidth: true
        Layout.maximumWidth: root.halfWidth
        Layout.preferredWidth: root.halfWidth
        onMoved: {
            root.valueEdited()
        }
        topPadding: 0
        bottomPadding: 16
        leftPadding: 0
        rightPadding: 0
        LithStyle.Label {
            text: root.from.toFixed(readingPrecision)
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            font.pixelSize:  LithStyle.FontSizes.small
        }
        LithStyle.Label {
            text: value.toFixed(readingPrecision)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            font.pixelSize:  LithStyle.FontSizes.regular
        }
        LithStyle.Label {
            text: root.to.toFixed(readingPrecision)
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            font.pixelSize:  LithStyle.FontSizes.small
        }
    }
}
