import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    id: root
    color: palette.window
    implicitHeight: layout.implicitHeight + layout.y

    property Item additionalButton
    onAdditionalButtonChanged: {
        additionalButton.parent = layout
        additionalButton.Layout.fillHeight = true
    }

    property string acceptText: qsTr("Ok")
    property string rejectText: qsTr("Cancel")
    property bool acceptable: true
    property var dialog
    signal accepted
    signal rejected

    Rectangle {
        width: parent.width
        height: 1
        color: palette.midlight
    }

    RowLayout {
        id: layout
        height: parent.height
        y: 1
        spacing: 18
        anchors.horizontalCenter: parent.horizontalCenter
        // This item is laid out in the other direction to allow adding more buttons on the left side
        layoutDirection: Qt.RightToLeft

        Button {
            text: root.rejectText
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
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

        Button {
            enabled: acceptable
            Layout.minimumHeight: 40
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            text: root.acceptText
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
    }
}
