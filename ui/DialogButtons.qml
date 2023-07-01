import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

RowLayout {
    id: root

    property bool acceptable: true
    property var dialog
    signal accepted
    signal rejected

    Button {
        enabled: acceptable
        Layout.fillWidth: true
        text: qsTr("Ok")
        onClicked: function() {
            if (dialog) {
                dialog.accepted()
                dialog.visible = false
            }
            else {
                accepted()
            }
        }
    }
    Button {
        Layout.fillWidth: true
        text: qsTr("Cancel")
        onClicked: function() {
            if (dialog) {
                dialog.rejected()
                dialog.visible = false
            }
            else {
                rejected()
            }
        }
    }
}
