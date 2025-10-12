import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Lith.Style
import Lith.Core

import ".."

Window {
    id: debugWindow

    title: "Lith Debug"

    visible: true
    width: 800
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
            TextField {
                placeholderText: "Set error string"
                implicitWidth: 192
                onAccepted: {
                    Lith.errorString = text
                    text = ""
                }
            }
            CheckBox {
                text: "Cover out of bounds areas"
                checked: mainView.safeAreaBoundaryCovers
                onCheckedChanged: mainView.safeAreaBoundaryCovers = checked
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
