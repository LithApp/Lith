import QtQuick 2.0

Text {
    visible: modelData.visible && modelData.level === 0
    text: modelData.name
    color: modelData.color
    font.family: "Consolas"
    height: visible ? implicitHeight : 0
}
