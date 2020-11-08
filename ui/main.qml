// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Jakub Mach
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

import QtQuick 2.12
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
                channelView.textInput.forceActiveFocus()
                Qt.inputMethod.hide()
            }
        }
    }

    Dialog {
        id: linkHandler
        width: parent.width * 0.78
        height: linkHandlerLayout.height + 40
        visible: false
        anchors.centerIn: parent

        property string currentLink
        property bool containsImage: currentLink.endsWith("png") ||
                                     currentLink.endsWith("jpg") ||
                                     currentLink.endsWith("gif")
        property bool containsVideo: currentLink.endsWith("avi") ||
                                     currentLink.endsWith("mov") ||
                                     currentLink.endsWith("mp4") ||
                                     currentLink.endsWith("webm")

        function show(link, newparent) {
            visible = true
            currentLink = link
            console.warn("AHOJ " + link)
        }

        ColumnLayout {
            id: linkHandlerLayout
            width: parent.width
            spacing: 9
            Text {
                id: linkText
                Layout.fillWidth: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.family: "Menlo"
                font.pointSize: lith.settings.baseFontSize
                textFormat: Text.RichText
                text: "<a href=\""+linkHandler.currentLink+"\">"+linkHandler.currentLink+"</a>"
            }
            RowLayout {
                Layout.alignment: Qt.AlignCenter
                Button {
                    focusPolicy: Qt.NoFocus
                    font.pointSize: settings.baseFontSize
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: height
                    text: "📋"
                    onClicked: {
                        clipboard.setText(linkHandler.currentLink)
                        linkHandler.visible = false
                    }
                }
                Button {
                    focusPolicy: Qt.NoFocus
                    font.pointSize: settings.baseFontSize
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: height
                    onClicked: {
                        Qt.openUrlExternally(linkHandler.currentLink)
                        linkHandler.visible = false
                    }
                    Text {
                        text: "⤶"
                        rotation: 180
                        anchors.centerIn: parent
                    }
                }
                Button {
                    visible: linkHandler.containsImage || linkHandler.containsVideo
                    focusPolicy: Qt.NoFocus
                    text: "🖼️"
                    font.pointSize: settings.baseFontSize
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: height
                    onClicked: {
                        if (linkHandler.containsImage)
                            previewPopup.showImage(linkHandler.currentLink)
                        if (linkHandler.containsVideo)
                            previewPopup.showVideo(linkHandler.currentLink)
                        linkHandler.visible = false
                    }
                }
            }
        }
    }

    DataBrowser {
        id: dataBrowser
        visible: false
        width: parent.width
        height: parent.height
    }
}
