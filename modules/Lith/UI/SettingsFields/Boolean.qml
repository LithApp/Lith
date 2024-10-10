import QtQuick
import Lith.Style

Base {
    property alias checked: checkBox.checked

    rowComponent: CheckBox {
        id: checkBox
        padding: 0
    }
}
