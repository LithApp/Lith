import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: root
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
            Text {
                text: "Enable readline shortcuts (^W, ^D, etc.)"
            }
            CheckBox {
                enabled: false
                checked: lith.settings.enableReadlineShortcuts
            }

            Text {
                text: "Start buffer search and filtering"
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutSearchBuffer.join(", ")
                Layout.preferredWidth: 176
            }

            Text {
                text: "Autocomplete"
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutAutocomplete.join(", ")
                Layout.preferredWidth: 176
            }

            Text {
                text: "Switch to next buffer"
            }
            TextField {
                enabled: false
                text: lith.settings.shortcutSwitchToNextBuffer.join(", ")
                Layout.preferredWidth: 176
            }

            Text {
                text: "Switch to previous buffer"
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
