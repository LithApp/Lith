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
                placeholderText: "Set all margins to"
                implicitWidth: 192
                validator: IntValidator {

                }
                onAccepted: {
                    WindowHelper.safeAreaMargins.left = text
                    WindowHelper.safeAreaMargins.right = text
                    WindowHelper.safeAreaMargins.top = text
                    WindowHelper.safeAreaMargins.bottom = text
                    text = ""
                }
            }
            TextField {
                placeholderText: "Set error string"
                implicitWidth: 192
                onAccepted: {
                    Lith.errorString = text
                    text = ""
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Safe Area:"
            }
            GridLayout {
                Layout.fillWidth: true
                columns: 4
                Repeater {
                    model: ["left", "right", "top", "bottom"]
                    Label {
                        text: modelData
                    }
                }
                Repeater {
                    model: ["left", "right", "top", "bottom"]
                    SpinBox {
                        value: WindowHelper.safeAreaMargins[modelData]
                        onValueChanged: WindowHelper.safeAreaMargins[modelData] = value
                    }
                }
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
