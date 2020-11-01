import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

Controls.AbstractButton {
    id: control

    SystemPalette {
        id: palette
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: palette.windowText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.color: Qt.darker(palette.button)
        color: palette.button
        border.width: 1
        radius: 2
    }
}
