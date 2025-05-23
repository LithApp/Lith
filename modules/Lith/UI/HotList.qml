import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core
import Lith.Style

ListView {
    id: root
    model: Lith.buffers
    clip: true

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
            Lith.selectedBuffer = modelData
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
    Rectangle {
        id: background
        anchors.fill: parent
        z: -1
        color: LithPalette.regular.window
    }
}
