import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 480
    height: 800
    title: "Lith"

    SystemPalette {
        id: palette
    }

    header: ErrorMessage { }

    Rectangle {
        id: windowBackground
        anchors.fill: parent
        color: palette.base
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

    ChannelView {
        anchors.fill: parent
    }

    SettingsDialog {
        id: settingsDialog
    }

    PreviewPopup {
        id: previewPopup
    }
}
