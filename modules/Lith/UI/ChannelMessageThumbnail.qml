import QtQuick
import QtQuick.Controls

import Lith.Core

Rectangle {
    id: root

    readonly property real maximumHeightHorizontal: 100
    readonly property real maximumHeightVertical: 3 * maximumHeightHorizontal
    property real maximumWidth: 100
    property string thumbnailUrl
    readonly property real aspectRatio: thumbnailImage.sourceSize.width === 0 ? 1.0 : thumbnailImage.sourceSize.height / thumbnailImage.sourceSize.width

    width: aspectRatio > 1.0 ? height / aspectRatio : Math.min(height / aspectRatio, maximumWidth)
    height: aspectRatio <= 1.0 ? maximumHeightHorizontal : maximumHeightVertical
    radius: 6
    color: "transparent"
    border.width: 1
    border.color: LithPalette.regular.text
    visible: root.thumbnailUrl.endsWith(".jpg") || root.thumbnailUrl.endsWith(".png")

    Image {
        id: thumbnailImage
        z: -1
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: root.thumbnailUrl.endsWith(".jpg") || root.thumbnailUrl.endsWith(".png") ? root.thumbnailUrl : ""

        Label {
            id: thumbnailCross
            text: "❌"
            visible: parent.status === Image.Error
            anchors.centerIn: parent
            color: "red"
            Label {
                anchors {
                    top: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Error"
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: linkHandler.show(root.thumbnailUrl, root)
    }
}
