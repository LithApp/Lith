import QtQuick 2
import QtQuick.Controls 2.12
import QtQuick.Layouts 1

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
        return 300
        if (Qt.inputMethod && Qt.inputMethod.keyboardRectangle && Qt.inputMethod.visible) {
            if (!channelView.inputBarHasFocus) {
                if (Qt.platform.os === "ios") {
                    return Qt.inputMethod.keyboardRectangle.height - bottomSafeAreaHeight
                }
            }
        }
        return 300
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
        property bool isClosed: position === 0.0
        dragMargin: 64
        y: isClosed ? root.y + channelView.messageArea.y : root.y
        height: isClosed ? channelView.messageArea.height : root.height
        width: 0.66 * root.width + root.leftMargin
    }

    NickList {
        id: nickDrawer
        edge: Qt.RightEdge
        rightPadding: root.rightMargin
        property bool isClosed: position === 0.0
        dragMargin: 64
        y: isClosed ? root.y + channelView.messageArea.y : root.y
        height: isClosed ? channelView.scrollToBottomButtonPosition : root.height
        width: 0.66 * root.width + root.rightMargin
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


    Dialog {
        id: linkHandler
        z: 99999999
        width: root.width
        height: linkHandlerLayout.height + 12
        // Dialog doesn't allow anchors.verticalCenter, so we need to position it manually
        x: -parent.mapToItem(root, 0, 0).x
        y: parent.height / 2 - height / 2
        visible: false
        padding: 0
        topPadding: 0

        property string currentLink
        property string currentExtension: lith.getLinkFileExtension(currentLink)
        property bool containsImage: currentExtension.endsWith("png") ||
                                     currentExtension.endsWith("jpg") ||
                                     currentExtension.endsWith("jpeg")
        property bool containsVideo: currentExtension.endsWith("avi") ||
                                     currentExtension.endsWith("mov") ||
                                     currentExtension.endsWith("mp4") ||
                                     currentExtension.endsWith("webm") ||
                                     currentExtension.endsWith("gif")

        onVisibleChanged: {
            if (!visible)
                parent = root
        }

        function show(link, item) {
            if (lith.settings.openLinksDirectly) {
                currentLink = link
                openCurrentLink(!lith.settings.openLinksDirectlyInBrowser)
            }
            else {
                if (item === null)
                    parent = root
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
            Text {
                id: linkText
                Layout.fillWidth: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: lith.settings.baseFontSize
                textFormat: Text.RichText
                text: "<a href=\""+linkHandler.currentLink+"\">"+linkHandler.currentLink+"</a>"
            }
            Button {
                focusPolicy: Qt.NoFocus
                font.pointSize: settings.baseFontSize
                Layout.preferredHeight: 36
                Layout.preferredWidth: height
                icon.source: "qrc:/navigation/"+currentTheme+"/copy.png"
                onClicked: {
                    clipboardProxy.setText(linkHandler.currentLink)
                    linkHandler.visible = false
                }
            }
            Button {
                focusPolicy: Qt.NoFocus
                font.pointSize: settings.baseFontSize
                Layout.preferredHeight: 36
                Layout.preferredWidth: height
                onClicked: {
                    linkHandler.openCurrentLink(false)
                    linkHandler.visible = false
                }
                icon.source: "qrc:/navigation/"+currentTheme+"/resize.png"
            }
            Button {
                visible: linkHandler.containsImage || linkHandler.containsVideo
                focusPolicy: Qt.NoFocus
                icon.source: "qrc:/navigation/"+currentTheme+"/image.png"
                font.pointSize: settings.baseFontSize
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
