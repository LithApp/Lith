// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
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
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    modal: true
    borderless: true

    header: TabBar {
        id: tabBar
        width: parent.width

        Repeater {
            model: window.platform.mobile ? [qsTr("Connection"), qsTr("Interface")]
                                          : [qsTr("Connection"), qsTr("Interface"), qsTr("Shortcuts")]

            delegate: TabButton {
                text: modelData
            }
        }
    }

    function save() {
        settingsNetwork.save()
        settingsInterface.save()
        settingsShortcuts.save()
    }
    function restore() {
        settingsNetwork.restore()
        settingsInterface.restore()
        settingsShortcuts.restore()
    }

    onAccepted: {
        settingsNetwork.onAccepted()
        settingsInterface.onAccepted()
        settingsShortcuts.onAccepted()
    }
    onRejected: {
        settingsNetwork.onRejected()
        settingsInterface.onRejected()
        settingsShortcuts.onRejected()
    }


    StackLayout {
        id: stackLayout
        anchors.fill: parent

        currentIndex: tabBar.currentIndex

        SettingsNetwork {
            id: settingsNetwork
        }

        SettingsInterface {
            id: settingsInterface
        }

        SettingsShortcuts {
            id: settingsShortcuts
            enabled: !window.platform.mobile
        }
    }

    Text {
        z: 9999999999
        anchors.centerIn: parent
        color: "red"
        font.pointSize: 14
        text: {
            let keyboardTopBoundary = Window.height - Qt.inputMethod.keyboardRectangle.height
            let focusItemBottomBoundary = Window.activeFocusItem ? Window.activeFocusItem.mapToGlobal(0, Window.activeFocusItem.height).y : "N/A"
            return "Window.height" + Window.height + "\n" +
              "Qt.inputMethod.keyboardRectangle.height" + Qt.inputMethod.keyboardRectangle.height + "\n" +
              "Window.activeFocusItem.y" + (Window.activeFocusItem ? Window.activeFocusItem.mapToGlobal(0, Window.activeFocusItem.height).y : "N/A") + "\n" +
              "keyboardTopBoundary" + keyboardTopBoundary + "\n" +
              "focusItemBottomBoundary" + focusItemBottomBoundary + "\n"
        }
        Rectangle {
            z: -1
            anchors.fill: parent
            opacity: 0.5
            anchors.margins: -3
        }
    }

    footer: DialogButtons {
        id: dialogButtons
        dialog: root
        acceptText: qsTr("Save and close")
        additionalButton: Button {
            text: qsTr("Save")
            onClicked: root.save()
        }
    }
}
