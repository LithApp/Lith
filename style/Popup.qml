import QtQuick
import QtQuick.Templates as T

T.Popup {
    id: control
    height: 430

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 200
        border.color: "#444"
    }
    contentItem: Column {}
}
