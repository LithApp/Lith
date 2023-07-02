import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import lith 1.0

ListView {
    id: root
    model: lith.buffers

    // TODO this should really be calculated from the size of the listview and the items but it isn't
    // I don't really want to deal with this now though
    implicitHeight: lith.settings.hotlistCompact ? 18 : 32
    spacing: lith.settings.hotlistCompact ? 3 : 6

    Reflection {
        id: reflection
        className: "Buffer"
    }

    orientation: ListView.Horizontal
    delegate: HotListItem {
        id: delegateBody

        onClicked: lith.selectedBufferIndex = index

        required property var modelData
        layoutSpacing: root.spacing
        hot: modelData.hotMessages > 0
        visible: modelData.hotMessages > 0 || modelData.unreadMessages > 0

        text: modelData.number + (lith.settings.hotlistShowUnreadCount ? (":" + (modelData.hotMessages > 0 ? modelData.hotMessages + "/" + modelData.unreadMessages : modelData.unreadMessages)) : "")
    }
}
