import QtQuick 2.0

ListView {
    model: stuff.selected ? stuff.selected.lines : null
    clip: true
    rotation: 180
    delegate: Message {}
}
