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

    Item {
        width: parent.width
        implicitHeight: settingsPaneLayout.implicitHeight

        GridLayout {
            id: settingsPaneLayout
            // shouldn't be hardcoded
            columns: parent.width > 420 ? 2 : 1
            width: columns === 2 ? Math.min(parent.width, implicitWidth) : parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            ColumnLayout {
                Layout.alignment: Qt.AlignLeft
                spacing: 0
                Label {
                    text: qsTr("Enable readline shortcuts")
                }
                Label {
                    text: "(^W, ^D, etc.)"
                    font.pointSize: lith.settings.baseFontSize * 0.50
                }
            }
            CheckBox {
                enabled: false
                checked: lith.settings.enableReadlineShortcuts
                Layout.alignment: Qt.AlignRight
            }

            Label {
                text: qsTr("Open buffer search")
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutSearchBuffer.join(", ")
                Layout.preferredWidth: 176
                Layout.alignment: Qt.AlignRight
            }

            Label {
                text: qsTr("Autocomplete")
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutAutocomplete.join(", ")
                Layout.preferredWidth: 176
                Layout.alignment: Qt.AlignRight
            }

            Label {
                text: qsTr("Switch to next buffer")
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutSwitchToNextBuffer.join(", ")
                Layout.preferredWidth: 176
                Layout.alignment: Qt.AlignRight
            }

            Label {
                text: qsTr("Switch to previous buffer")
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutSwitchToPreviousBuffer.join(", ")
                Layout.preferredWidth: 176
                Layout.alignment: Qt.AlignRight
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
