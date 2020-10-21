import QtQuick 2.0
import QtQuick.Controls 2.4

Drawer {
    SystemPalette {
        id: palette
    }

    Rectangle {
        anchors.fill: parent
        color: palette.window
    }

    ListView {
        anchors.fill: parent
        model: stuff.selected ? stuff.selected.nicks : null
        delegate: Text {
            text: modelData.name
            visible: modelData.visible && modelData.level === 0
            height: visible ? implicitHeight : 0
            color: palette.windowText
            font.family: "Menlo"
            font.pointSize: 16
        }
    }
}
