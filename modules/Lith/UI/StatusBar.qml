import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core
import Lith.Style

Rectangle {
    id: root
    color: LithPalette.regular.base
    implicitHeight: replayInfoLayout.visible ? replayInfoLayout.implicitHeight + replayInfoLayout.anchors.margins * 2 : 0
    implicitWidth: replayInfoLayout.visible ? replayInfoLayout.implicitWidth + replayInfoLayout.anchors.margins * 2 : 0

    RowLayout {
        id: replayInfoLayout
        visible: Lith.networkProxy.recording || Lith.status === Lith.REPLAY
        anchors.fill: parent
        anchors.margins: 3
        Rectangle {
            color: "red"
            Layout.alignment: Qt.AlignVCenter
            visible: Lith.networkProxy.recording

            width: Lith.settings.baseFontSize
            height: width
            radius: width / 2
        }
        Label {
            font.pointSize: FontSizes.tiny
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            elide: Label.ElideRight
            text: {
                if (Lith.networkProxy.recording)
                    return ("Recording into slot %1").arg(Lith.networkProxy.slot)
                else if (Lith.networkProxy.
                         replaying)
                    return ("Replaying event %1/%2").arg(Lith.networkProxy.currentEvent).arg(Lith.networkProxy.totalEvents)
                else if (Lith.status === Lith.REPLAY)
                    return ("Replayed %1 events").arg(Lith.networkProxy.totalEvents)
                else
                    return ""
            }
            SequentialAnimation on color {
                id: breathingAnimation
                running: Lith.networkProxy.recording
                loops: Animation.Infinite
                property color firstColor: LithPalette.regular.text
                onFirstColorChanged: breathingAnimation.restart()
                property color secondColor: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.highlight, 0.5)
                onSecondColorChanged: breathingAnimation.restart()
                property real animationDuration: 3000
                ColorAnimation { from: breathingAnimation.firstColor; to: breathingAnimation.secondColor; duration: breathingAnimation.animationDuration }
                ColorAnimation { from: breathingAnimation.secondColor; to: breathingAnimation.firstColor; duration: breathingAnimation.animationDuration }
            }
        }
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 1
        color: ColorUtils.mixColors(LithPalette.regular.window, LithPalette.regular.text, 0.5)

    }
}
