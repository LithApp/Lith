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

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 9
        visible: lith && lith.selectedBuffer

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: lith.selectedBuffer ? lith.selectedBuffer.full_name : ""
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize * 1.125
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "0 users, 0 voice, 0 ops"
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize * 0.75
        }

        Item {
            Layout.fillWidth: true
            height: nickFilter.height
            TextField {
                id: nickFilter
                focus: false
                placeholderText: "Filter nicks"
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 3
                }
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1
            }
        }

        ListView {
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: lith.selectedBuffer ? lith.selectedBuffer.nicks : null

            delegate: Rectangle {
                width: ListView.view.width
                visible: modelData.visible && modelData.level === 0
                height: visible ? nickTextItem.height + 12 : 0
                color: nickItemMouse.pressed ? "gray" : palette.base

                MouseArea {
                    id: nickItemMouse
                    anchors.fill: parent
                    onClicked: {
                        console.warn("something, soon?");
                        nickListActionMenu.visible = true
                        nickListActionMenu.nickname = modelData.name
                    }
                }

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


                    }
                }
            }
        }
    }
}
