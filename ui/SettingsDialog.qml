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
    focus: true
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
