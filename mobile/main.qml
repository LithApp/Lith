import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 480
    height: 800
    title: "Lith"

    SystemPalette {
        id: palette
    }

    BufferList {
        id: bufferDrawer
        width: 0.66 * parent.width
        height: parent.height
    }

    NickList {
        id: nickDrawer
        edge: Qt.RightEdge
        width: 0.66 * parent.width
        height: parent.height
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }

    FrontPage {
        anchors.fill: parent
    }

    SettingsDialog {
        id: settingsDialog
        anchors.centerIn: parent
    }
}
