import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core
import Lith.Style

Item {
    id: mainView

    x: leftMargin
    y: topMargin
    width: parent.width - leftMargin - rightMargin
    height: parent.height - topMargin - bottomMargin

    implicitWidth: LithPlatform.mobile ? 480 : 1024
    implicitHeight: 800

    Component.onCompleted: {
        WindowHelper.updateSafeAreaMargins(window)
    }
    onWidthChanged: {
        WindowHelper.updateSafeAreaMargins(window)
    }
    onHeightChanged: {
        WindowHelper.updateSafeAreaMargins(window)
    }

    property real topMargin: {
        if (LithPlatform.ios) {
            if (Qt.inputMethod && Qt.inputMethod.keyboardRectangle && Qt.inputMethod.visible) {
                let keyboardTopBoundary = Window.height - Qt.inputMethod.keyboardRectangle.height
                let focusItemBottomBoundary = Window.activeFocusItem.mapToGlobal(0, Window.activeFocusItem.height).y
                if (focusItemBottomBoundary > keyboardTopBoundary) {
                    return Qt.inputMethod.keyboardRectangle.height + WindowHelper.safeAreaMargins.top
                }
            }
        }
        return WindowHelper.safeAreaMargins.top
    }
    property real bottomMargin:{
        if (LithPlatform.ios) {
            let keyboardTopBoundary = Window.height - Qt.inputMethod.keyboardRectangle.height
            let focusItemBottomBoundary = Window.activeFocusItem.mapToGlobal(0, Window.activeFocusItem.height).y
            if (Qt.inputMethod && Qt.inputMethod.keyboardRectangle && Qt.inputMethod.visible) {
                if (focusItemBottomBoundary <= keyboardTopBoundary)
                    return Qt.inputMethod.keyboardRectangle.height
                else
                    return 0
            }
        }
        return WindowHelper.safeAreaMargins.bottom
    }
    property real leftMargin: WindowHelper.safeAreaMargins.left
    property real rightMargin: WindowHelper.safeAreaMargins.right

    Behavior on topMargin { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on bottomMargin { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on leftMargin { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    Behavior on rightMargin { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }

    Rectangle {
        id: windowBackground
        anchors.fill: parent
        color: palette.base
    }

    Rectangle {
        z: 9999
        y: -topMargin
        width: parent.width
        height: topMargin
        color: LithPalette.regular.base
    }

    Rectangle {
        z: 9999
        y: parent.height
        width: parent.width
        height: topMargin
        color: LithPalette.regular.base
    }

    Rectangle {
        z: 9999
        x: -leftMargin
        width: leftMargin
        height: parent.height
        color: LithPalette.regular.base
    }

    Rectangle {
        z: 9999
        x: parent.width
        width: leftMargin
        height: parent.height
        color: LithPalette.regular.base
    }

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
        enabled: {
            if (!nickDrawer.isClosed)
                return false
            if (!window.WindowHelper.landscapeMode && !bufferDrawer.isClosed)
                return false
            return true
        }
        anchors {
            left: WindowHelper.landscapeMode ? bufferDrawer.right : parent.left
            right: parent.right
            top: errorMessage.bottom
            bottom: parent.bottom
        }
    }

    DynamicDrawer {
        id: bufferDrawer
        y: errorMessage.height
        height: mainView.height - y
        dragOutHandleTopMargin: channelView.messageArea.y
        dragOutHandleBottomMargin: height - channelView.messageArea.y - channelView.messageArea.height
        Component.onCompleted: {
            if (Lith.settings.showBufferListOnStartup)
                lastState = DynamicDrawer.State.Open
            else
                lastState = DynamicDrawer.State.Closed
        }

        onIsClosedChanged: {
            bufferList.currentIndex = Lith.selectedBufferIndex

            if (!isClosed) {
                bufferList.clear()
            }
        }

        BufferList {
            id: bufferList
            anchors.fill: parent
            onClose: {
                if (!WindowHelper.landscapeMode)
                    bufferDrawer.hide()
                if (channelView.textInput.visible)
                    channelView.textInput.forceActiveFocus()
                else if (channelView.searchTextInput.visible)
                    channelView.searchTextInput.forceActiveFocus()
            }
        }
    }

    NickList {
        id: nickDrawer
        edge: Qt.RightEdge
        rightPadding: mainView.rightMargin
        property bool isClosed: position === 0.0
        dragMargin: 64
        y: isClosed ? mainView.y + channelView.messageArea.y : mainView.y
        height: isClosed ? channelView.scrollToBottomButtonPosition : mainView.height
        width: Math.min(0.66 * mainView.width, 400) + mainView.rightMargin
        onRequestSearchBar: {
            channelView.showSearchBar()
        }
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
        topMargin: mainView.topMargin
        bottomMargin: mainView.bottomMargin

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


    Dialog {
        id: linkHandler
        z: 99999999
        width: mainView.width
        height: linkHandlerLayout.height + 12
        // Dialog doesn't allow anchors.verticalCenter, so we need to position it manually
        x: -parent.mapToItem(mainView, 0, 0).x
        y: parent.height / 2 - height / 2
        visible: false
        padding: 0
        topPadding: 0

        property string currentLink
        property string currentExtension: Lith.getLinkFileExtension(currentLink)
        property bool containsImage: currentExtension.endsWith("png") ||
                                     currentExtension.endsWith("jpg") ||
                                     currentExtension.endsWith("jpeg")||
                                     currentExtension.endsWith("webp")||
                                     currentExtension.endsWith("bmp") ||
                                     currentExtension.endsWith("svg")
        property bool containsVideo: currentExtension.endsWith("avi") ||
                                     currentExtension.endsWith("mov") ||
                                     currentExtension.endsWith("mp4") ||
                                     currentExtension.endsWith("webm") ||
                                     currentExtension.endsWith("gif")

        onVisibleChanged: {
            if (!visible)
                parent = mainView
        }

        function show(link, item) {
            if (Lith.settings.openLinksDirectly) {
                currentLink = link
                openCurrentLink(!Lith.settings.openLinksDirectlyInBrowser)
            }
            else {
                if (item === null)
                    parent = mainView
                else
                    parent = item
                currentLink = link
                visible = true
            }
        }

        function openCurrentLink(openPreview) {
            if (linkHandler.containsImage && openPreview)
                previewPopup.showImage(linkHandler.currentLink)
            else if (linkHandler.containsVideo && openPreview)
                previewPopup.showVideo(linkHandler.currentLink)
            else
                Qt.openUrlExternally(linkHandler.currentLink)
        }

        RowLayout {
            id: linkHandlerLayout
            y: 6
            x: 6
            width: parent.width - 12
            spacing: 9
            Label {
                id: linkText
                Layout.fillWidth: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                textFormat: Text.RichText
                text: "<a href=\""+linkHandler.currentLink+"\">"+linkHandler.currentLink+"</a>"
            }
            Button {
                focusPolicy: Qt.NoFocus
                Layout.preferredHeight: 36
                Layout.preferredWidth: height
                icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/copy.png"
                onClicked: {
                    ClipboardProxy.setText(linkHandler.currentLink)
                    linkHandler.visible = false
                }
            }
            Button {
                focusPolicy: Qt.NoFocus
                Layout.preferredHeight: 36
                Layout.preferredWidth: height
                onClicked: {
                    linkHandler.openCurrentLink(false)
                    linkHandler.visible = false
                }
                icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/resize.png"
            }
            Button {
                visible: linkHandler.containsImage || linkHandler.containsVideo
                focusPolicy: Qt.NoFocus
                icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/image.png"
                Layout.preferredHeight: 36
                Layout.preferredWidth: height
                onClicked: {
                    linkHandler.openCurrentLink(true)
                    linkHandler.visible = false
                }
            }
        }
    }
}
