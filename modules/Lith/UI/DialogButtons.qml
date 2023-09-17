import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Lith.Style
import Lith.Core

Rectangle {
    id: root
    color: ColorUtils.mixColors(LithPalette.regular.window, LithPalette.regular.text, 0.9)
    implicitHeight: layout.implicitHeight + 2 * layout.y

    property Item additionalButton
    onAdditionalButtonChanged: {
        additionalButton.parent = layout
        additionalButton.Layout.fillHeight = true
    }

    property string acceptText: qsTr("Ok")
    property string rejectText: qsTr("Cancel")
    property bool acceptable: true
    property var dialog: null
    signal accepted
    signal rejected

    RowLayout {
        id: layout
        y: 5
        height: parent.height - 2 * y
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
