import QtQuick 2

Item {
    id: root

    x: leftMargin
    y: topMargin
    width: parent.width - leftMargin - rightMargin
    height: parent.height - topMargin - bottomMargin

    // TODO safe area handling for all sides of the screen
    // (it doesn't really do anything as of now anyway)
    property real bottomSafeAreaHeight: 0.0
    Component.onCompleted: {
        bottomSafeAreaHeight = lith.windowHelper.getBottomSafeAreaSize()
    }
    onWidthChanged: {
        bottomSafeAreaHeight = lith.windowHelper.getBottomSafeAreaSize()
    }

    property real topMargin: {
        if (Qt.inputMethod && Qt.inputMethod.keyboardRectangle && Qt.inputMethod.visible) {
            if (channelView.inputBarHasFocus) {
                if (Qt.platform.os === "ios") {
                    return Qt.inputMethod.keyboardRectangle.height
                }
            }
        }
        return 0
    }
    property real bottomMargin:{
        if (Qt.inputMethod && Qt.inputMethod.keyboardRectangle && Qt.inputMethod.visible) {
            if (!channelView.inputBarHasFocus) {
                if (Qt.platform.os === "ios") {
                    return Qt.inputMethod.keyboardRectangle.height - bottomSafeAreaHeight
                }
            }
        }
        return 0
    }
    property real leftMargin: 0
    property real rightMargin: 0

    Behavior on topMargin { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on bottomMargin { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on leftMargin { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on rightMargin { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }

    ErrorMessage {
        id: errorMessage
        anchors {
            top: parent.top
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
            bottom: parent.bottom
        }
    }

    BufferList {
        id: bufferDrawer
        leftPadding: root.leftMargin
        y: root.y
        width: 0.66 * root.width + root.leftMargin
        height: root.height
    }

    NickList {
        id: nickDrawer
        edge: Qt.RightEdge
        rightPadding: root.rightMargin
        y: root.y
        width: 0.66 * root.width + root.rightMargin
        height: root.height
    }

    NickListActionMenu {
        id: nickListActionMenu
    }

    SettingsDialog {
        id: settingsDialog
        width: parent.width
        height: parent.height
    }

    PreviewPopup {
        id: previewPopup
        topMargin: root.topMargin
        bottomMargin: root.bottomMargin

        onVisibleChanged: {
            if (visible) {
                Qt.inputMethod.hide()
            }
            else {
                channelView.textInput.forceActiveFocus()
                Qt.inputMethod.hide()
            }
        }
    }
}
