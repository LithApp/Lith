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

    Rectangle {
        id: windowBackground
        anchors.fill: parent
        color: palette.base
    }

    Rectangle {
        id: topKeyboardArea
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        color: palette.window
        height: Qt.inputMethod &&
                Qt.inputMethod.keyboardRectangle &&
                Qt.inputMethod.visible &&
                channelView.inputBarHasFocus ? Qt.inputMethod.keyboardRectangle.height :
                                               0
        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }
    Rectangle {
        id: bottomKeyboardArea
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        color: palette.window
        height: Qt.inputMethod &&
                Qt.inputMethod.keyboardRectangle &&
                Qt.inputMethod.visible &&
                !channelView.inputBarHasFocus ? Qt.inputMethod.keyboardRectangle.height :
                                                0
        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }

    ErrorMessage {
        id: errorMessage
        anchors {
            top: topKeyboardArea.bottom
            left: parent.left
            right: parent.right
        }
    }

    ChannelView {
        id: channelView
        anchors {
            left: parent.left
            right: parent.right
            top: errorMessage.bottom
            bottom: bottomKeyboardArea.top
        }
    }

    BufferList {
        id: bufferDrawer
        width: 0.66 * parent.width
        height: parent.height - bottomKeyboardArea.height
    }

    NickList {
        id: nickDrawer
        edge: Qt.RightEdge
        width: 0.66 * parent.width
        height: parent.height - bottomKeyboardArea.height
    }

    NickListActionMenu {
        id: nickListActionMenu
    }

    SettingsDialog {
        id: settingsDialog
        width: parent.width
        height: parent.height - bottomKeyboardArea.height
    }

    PreviewPopup {
        id: previewPopup
        topMargin: topKeyboardArea.height
        bottomMargin: bottomKeyboardArea.height

        onVisibleChanged: {
            if (visible) {
                Qt.inputMethod.hide()
            }
            else {
                Qt.inputMethod.show()
                channelView.textInput.forceActiveFocus()
            }
        }
    }
}
