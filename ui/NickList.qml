import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Drawer {
    SystemPalette {
        id: palette
    }

    Rectangle {
        anchors.fill: parent
        color: palette.window
    }

    ListView {
        anchors.fill: parent
        clip: true
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: stuff.selected ? stuff.selected.nicks : null

        delegate: Rectangle {
            width: ListView.view.width
            visible: modelData.visible && modelData.level === 0
            height: visible ? nickTextItem.height + 12 : 0
            color: nickItemMouse.pressed ? "gray" : palette.base

            Behavior on color {
                ColorAnimation {

                }
            }

            RowLayout {
                x: 3
                y: 6
                width: parent.width - 6
                Text {
                    id: nickTextItem
                    clip: true
                    text: (modelData.prefix === " " ? "" : modelData.prefix) + modelData.name
                    font.family: "Menlo"
                    font.pointSize: settings.baseFontSize * 1.125
                    color: palette.windowText

                    MouseArea {
                        id: nickItemMouse
                        anchors.fill: parent
                        onClicked: {
                            console.warn("something, soon?");
                            // TODO: open buffer with query? menu? whois? kick? ban?
                        }
                    }
                }
            }

        }
    }
}
