import QtQuick 2.0

Text {
    visible: model.visible
    text: model.name
    color: model.color
}
