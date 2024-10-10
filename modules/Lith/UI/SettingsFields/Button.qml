import QtQuick
import QtQuick.Layouts

import Lith.Style as LithStyle
Base {
    id: root
    property alias text: button.text
    property alias font: button.font
    signal clicked

    rowComponent: LithStyle.Button {
        id: button
        onClicked: root.clicked()
    }
}
