import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Base {
    id: root
    property alias model: repeater.model
    property int currentIndex: -1
    onCurrentIndexChanged: {
        if (currentIndex < 0) {
            for (const button in buttonGroup.buttons) {
                button.checked = false
            }
        }
        else if (currentIndex < buttonGroup.buttons.length) {
            buttonGroup.buttons[currentIndex].checked = true
        }
    }
    Component.onCompleted: {
        if (currentIndex >= 0 && currentIndex < buttonGroup.buttons.length) {
            buttonGroup.buttons[currentIndex].checked = true
        }
    }

    ButtonGroup {
        id: buttonGroup
        onCheckedButtonChanged: root.currentIndex = checkedButton.index
    }

    rowComponent: ColumnLayout {
        Layout.topMargin: 6
        Layout.bottomMargin: 6
        spacing: 6
        Repeater {
            id: repeater
            RowLayout {
                id: radioDelegateLayout
                required property var modelData
                required property int index

                Layout.alignment: Qt.AlignRight

                Label {
                    text: modelData
                }

                RadioButton {
                    ButtonGroup.group: buttonGroup
                    checked: false
                    readonly property int index: radioDelegateLayout.index
                    padding: 0
                }
            }
        }
    }
}
