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

import "LithStyle" as LithStyle

Dialog {
    id: root
    modal: true
    focus: true

    header: TabBar {
        id: tabBar
        width: parent.width

        Repeater {
            model: window.platform.mobile ? [qsTr("Connection"), qsTr("Interface")]
                                          : [qsTr("Connection"), qsTr("Interface"), qsTr("Shortcuts")]

            // TODO this is definitely wrong, the module specification is (but actually not) redundant
            delegate: LithStyle.TabButton {
                text: modelData
            }
        }
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
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: dialogButtons.top
        }

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

    // Separating gradient
    Rectangle {
        anchors {
            top: stackLayout.top
            left: parent.left
            right: parent.right
        }
        height: 12
        gradient: Gradient {
            GradientStop { position: 0.1; color: palette.button }
            GradientStop { position: 1.0; color: "transparent" }
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                top: top.bottom
            }
            height: 1
            color: palette.shadow
        }
    }

    // Separating gradient
    Rectangle {
        anchors {
            bottom: dialogButtons.top
            left: parent.left
            right: parent.right
        }
        height: 12
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 0.9; color: palette.button }
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 1
            color: palette.shadow
        }
    }

    DialogButtons {
        id: dialogButtons
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        dialog: root
    }
}
