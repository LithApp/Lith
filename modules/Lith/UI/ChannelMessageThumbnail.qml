import QtQuick
import QtQuick.Controls

import Lith.Core
import Lith.Style

Rectangle {
    id: root

    readonly property real maximumHeightHorizontal: 100
    readonly property real maximumHeightVertical: 3 * maximumHeightHorizontal
    property real maximumWidth: 100
    property string thumbnailUrl
    readonly property real aspectRatio: thumbnailImage.sourceSize.width === 0 ? 1.0 : thumbnailImage.sourceSize.height / thumbnailImage.sourceSize.width

    width: aspectRatio > 1.0 ? height / aspectRatio : Math.min(height / aspectRatio, maximumWidth)
    height: aspectRatio <= 1.0 ? maximumHeightHorizontal : maximumHeightVertical
    radius: 3
    color: "transparent"
    border.width: 0.5
    border.color: LithPalette.regular.text

    Image {
        id: thumbnailImage
        z: -1
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: root.thumbnailUrl.endsWith(".jpg") || root.thumbnailUrl.endsWith(".png") ? root.thumbnailUrl : ""
        asynchronous: true
    }
    Label {
        id: thumbnailCross
        text: "❌"
        visible: thumbnailImage.status === Image.Error || thumbnailImage.status === Image.Null
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

    MouseArea {
        anchors.fill: parent
        onClicked: linkHandler.show(root.thumbnailUrl, root)
    }
}
