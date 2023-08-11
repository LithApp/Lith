import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LithStyle

import ".."

Window {
    id: debugWindow

    visible: true
    width: 600
    height: 800

    color: palette.window

    ColumnLayout {
        id: misc
        width: parent.width
        RowLayout {
            Layout.fillWidth: true
            PaletteSwitch {
                text: "Switch theme"
            }
        }
    }
    SettingsDialogContents {
        id: settings
        anchors {
            top: misc.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
