// Lith
// Copyright (C) 2020 Martin Bříza
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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform
import QtQuick.Dialogs

import Lith.Core
import Lith.UI
import Lith.Style

Item {
    id: root

    implicitWidth: channelViewLayout.implicitWidth
    implicitHeight: channelViewLayout.implicitHeight

    property bool inputBarHasFocus: inputBar.hasFocus
    property alias textInput: inputBar.textInput
    property alias searchTextInput: searchBar.textInput
    property alias messageArea: messageArea
    property alias scrollToBottomButtonPosition: channelMessageList.scrollToBottomButtonPosition

    readonly property real headerImplicitContentHeight: channelHeader.implicitContentHeight
    readonly property real inputBarImplicitContentHeight: inputBar.implicitContentHeight

    function showSearchBar() {
        searchBar.visible = true
    }

    Rectangle {
        anchors.fill: parent
        visible: true
        color: LithPalette.regular.base
    }

    ColumnLayout {
        id: channelViewLayout
        anchors.fill: parent
        spacing: 0

        ChannelHeader {
            id: channelHeader
            Layout.fillWidth: true
            z: 1
        }

        Rectangle {
            id: messageArea
            Layout.fillHeight: true
            Layout.fillWidth: true

            // This is a bit of a hack, it would be nice to use the same color from selected palette
            //color: WindowHelper.useBlack ? "black" : ColorUtils.mixColors(LithPalette.regular.base, LithPalette.regular.window, 0.5)
            color: "transparent"

            ColumnLayout {
                visible: !Lith.selectedBuffer
                anchors.fill: parent
                anchors.margins: 6
                Label {
                    Layout.fillWidth: true
                    text: "Version %1".arg(Lith.gitVersion)
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    horizontalAlignment: Label.AlignHCenter
                }

                Label {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: ""
                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
                    textFormat: Text.RichText
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
                    textFormat: Text.RichText
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: qsTr("Icons made by <a href=\"https://www.flaticon.com/authors/pixel-perfect\" title=\"Pixel perfect\">Pixel perfect</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\"> www.flaticon.com</a>")
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }
            }

            ChannelMessageList {
                id: channelMessageList
                x: Lith.settings.uiMargins
                width: parent.width - 2 * x
                height: parent.height - 2
                visible: Lith.selectedBuffer
                clip: true
            }

            DropHandler {
                id: dropHandler
                anchors.fill: parent
                onTextEntered: textInput.text += t
                onUrlEntered: Uploader.upload(u)
            }
        }

        HotList {
            id: hotlist
            z: 1
            visible: Lith.settings.hotlistEnabled
            Layout.fillWidth: true
            Layout.preferredHeight: visible ? implicitHeight : 0
        }

        SearchBar {
            id: searchBar
            visible: false
            Layout.fillWidth: true
            onCurrentMessageIndexChanged: {
                if (searchBar.currentMessageIndex >= 0)
                    channelMessageList.positionViewAtIndex(searchBar.currentMessageIndex, ListView.Contain)
            }
        }

        ChannelInputBar {
            id: inputBar
            visible: !searchBar.visible
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
            onRequestSearchBar: searchBar.visible = true
        }

        FileDialog {
            id: fileDialog
            fileMode: FileDialog.OpenFile
            currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation).slice(-1)[0]
            nameFilters: [ "Image files (*.jpg *.jpeg *.png)" ]

            onAccepted: {
                //inputField.text += " " + fileUrl
                //imageButton.isBusy = false
                Uploader.upload(fileDialog.selectedFile)
                Qt.inputMethod.hide()
                inputBar.textInput.forceActiveFocus()
            }
            onRejected: {
                Qt.inputMethod.hide()
                inputBar.textInput.forceActiveFocus()
            }
        }
    }
}
