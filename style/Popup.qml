import QtQuick 2
import QtQuick.Templates 2 as T

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
