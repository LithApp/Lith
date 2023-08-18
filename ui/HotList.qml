import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Lith.Core

ListView {
    id: root
    model: Lith.buffers

    // TODO this should really be calculated from the size of the listview and the items but it isn't
    // I don't really want to deal with this now though
    implicitWidth: contentItem.childrenRect.width
    implicitHeight: contentItem.childrenRect.height
    spacing: Lith.settings.hotlistCompact ? 3 : 6

    Reflection {
        id: reflection
        className: "Buffer"
    }

    orientation: ListView.Horizontal
    delegate: HotListItem {
        id: delegateBody

        onClicked: {
            Lith.selectedBufferIndex = index
        }

        required property var modelData
        required property int index
        layoutSpacing: root.spacing
        hot: modelData.hotMessages > 0
        visible: modelData.hotMessages > 0 || modelData.unreadMessages > 0

        text: formatSplitter.formattedData

        FormatStringSplitter {
            id: formatSplitter
            allowedPropertyNames: reflection.stringProperties
            dataSource: delegateBody.modelData
        }
        Connections {
            target: Lith.settings
            function onHotlistFormatChanged() {
                formatSplitter.fromStringList(Lith.settings.hotlistFormat)
            }
        }
        Component.onCompleted: {
            formatSplitter.fromStringList(Lith.settings.hotlistFormat)
        }
    }
}
