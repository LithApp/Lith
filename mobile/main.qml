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

    header: Rectangle {
        id: keyboardArea
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        color: palette.window
        height: Qt.inputMethod &&
                Qt.inputMethod.keyboardRectangle &&
                Qt.inputMethod.visible &&
                channelView.inputBarHasFocus ? Qt.inputMethod.keyboardRectangle.height + errorMessage.height :
                                               errorMessage.height
        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
        ErrorMessage {
            id: errorMessage
            anchors.bottom: parent.bottom
        }
    }

    Rectangle {
        id: windowBackground
        anchors.fill: parent
        color: palette.base
    }

    BufferList {
        id: bufferDrawer
        width: 0.66 * parent.width
        height: parent.height
        topMargin: keyboardArea.height
    }

    NickList {
        id: nickDrawer
        edge: Qt.RightEdge
        width: 0.66 * parent.width
        height: parent.height
        topMargin: keyboardArea.height
    }

    ChannelView {
        id: channelView
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            top: keyboardArea.bottom
        }
    }

    SettingsDialog {
        id: settingsDialog
        topMargin: keyboardArea.height
    }

    PreviewPopup {
        id: previewPopup
        topMargin: keyboardArea.height
    }
}
