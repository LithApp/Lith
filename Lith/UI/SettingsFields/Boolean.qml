import QtQuick
import QtQuick.Controls

Base {
    property alias checked: checkBox.checked

    rowComponent: CheckBox {
        id: checkBox
        padding: 0
    }
}
