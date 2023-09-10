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
import QtQuick.Controls
import QtQuick.Layouts

import "SettingsFields" as Fields
import Lith.Core

ScrollView {
    id: root
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    function save() {
    }
    function restore() {
    }

    Item {
        width: parent.width
        implicitHeight: settingsPaneLayout.implicitHeight + 12

        ColumnLayout {
            id: settingsPaneLayout
            anchors.horizontalCenter: parent.horizontalCenter
            width: window.WindowHelper.landscapeMode ? Math.min(Math.min(420, 1.33 * implicitWidth), parent.width) : parent.width
            spacing: -1

            // BIG TODO, this whole layout is disabled
            enabled: false

            Fields.Header {
                text: qsTr("Shortcuts")
            }

            Fields.Boolean {
                summary: qsTr("Enable readline shortcuts")
                details: "(^W, ^D, etc.)"
                checked: Lith.settings.enableReadlineShortcuts
            }

            Fields.String {
                summary: qsTr("Open buffer search")
                text: Lith.settings.shortcutSearchBuffer.join(", ")
            }

            Fields.String {
                summary: qsTr("Autocomplete")
                text: Lith.settings.shortcutAutocomplete.join(", ")
            }

            Fields.String {
                summary: qsTr("Switch to next buffer")
                text: Lith.settings.shortcutSwitchToNextBuffer.join(", ")
            }

            Fields.String {
                summary: qsTr("Switch to previous buffer")
                text: Lith.settings.shortcutSwitchToPreviousBuffer.join(", ")
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
