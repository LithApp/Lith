import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import lith 1.0

ListView {
    id: root
    model: lith.buffers

    property int itemSize: lith.settings.hotlistCompact ? 18 : 32
    property int textSize: lith.settings.hotlistCompact ? 9 : 16

    height: itemSize
    Layout.preferredHeight: itemSize
    spacing: lith.settings.hotlistCompact ? 3 : 6

    orientation: ListView.Horizontal
    delegate: Rectangle {
        width: visible ? Math.max(itemLayout.width + 12, itemSize) : -root.spacing
        height: itemSize
        radius: 3
        color: modelData.hotMessages > 0 ? "red" : palette.text
        visible: modelData.hotMessages > 0 || modelData.unreadMessages > 0
        Row {
            id: itemLayout
            x: Math.max(6, (itemSize - width) / 2)
            anchors.verticalCenter: parent.verticalCenter
            spacing: lith.settings.hotlistCompact ? 1 : 3
            Text {
                font.pointSize: textSize
                font.bold: true
                text: modelData.number + (lith.settings.hotlistShowUnreadCount ? ":" : "")
                color: palette.base
            }
            Text {
                visible: false
                font.pointSize: textSize
                //visible: !lith.settings.hotlistShowUnreadCount && !lith.settings.hotlistCompact
                text: modelData.short_name
                elide: Text.ElideRight
            }
            Text {
                font.pointSize: textSize
                text: modelData.hotMessages > 0 ? modelData.hotMessages + "/" + modelData.unreadMessages : modelData.unreadMessages
                color: palette.base
                visible: lith.settings.hotlistShowUnreadCount
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: lith.selectedBufferIndex = index
        }
    }
}
