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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ScrollView {
    id: root
    clip: true
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    function onAccepted() {
    }
    function onRejected() {
    }

    ColumnLayout {
        x: 6
        y: 6
        width: root.width - 12
        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            columns: 2
            Label {
                text: qsTr("Enable readline shortcuts (^W, ^D, etc.)")
            }
            CheckBox {
                enabled: false
                checked: lith.settings.enableReadlineShortcuts
            }

            Label {
                text: qsTr("Start buffer search and filtering")
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutSearchBuffer.join(", ")
                Layout.preferredWidth: 176
            }

            Label {
                text: qsTr("Autocomplete")
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutAutocomplete.join(", ")
                Layout.preferredWidth: 176
            }

            Label {
                text: qsTr("Switch to next buffer")
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutSwitchToNextBuffer.join(", ")
                Layout.preferredWidth: 176
            }

            Label {
                text: qsTr("Switch to previous buffer")
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutSwitchToPreviousBuffer.join(", ")
                Layout.preferredWidth: 176
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
