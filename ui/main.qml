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
        clip: true
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        color: palette.window
        height: Qt.inputMethod &&
                Qt.inputMethod.keyboardRectangle &&
                Qt.inputMethod.visible &&
                Qt.platform.os !== "android" &&
                channelView.inputBarHasFocus ? Qt.inputMethod.keyboardRectangle.height :
                                               0
        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
        Text {
            text: "Top Area\nIf you see this, report a bug, please"
            anchors.centerIn: parent
        }
    }
    Rectangle {
        id: bottomKeyboardArea
        clip: true
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        color: palette.window
        height: Qt.inputMethod &&
                Qt.inputMethod.keyboardRectangle &&
                Qt.inputMethod.visible &&
                Qt.platform.os !== "android" &&
                !channelView.inputBarHasFocus ? Qt.inputMethod.keyboardRectangle.height - bottomSafeAreaHeight :
                                                0
        Behavior on height {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
        Text {
            text: "Bottom Area\nIf you see this, report a bug, please"
            anchors.centerIn: parent
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

    DataBrowser {
        id: dataBrowser
        visible: false
        width: parent.width
        height: parent.height
    }
}
