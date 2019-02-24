import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Drawer {
    SystemPalette {
        id: palette
    }

    Rectangle {
        anchors.fill: parent
        color: palette.window

        ColumnLayout {
            anchors.fill: parent

            Item {
                height: 1
            }
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: "Open Buffers"
                font.family: "Menlo"
                font.pointSize: 24
                color: palette.windowText
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        stuff.selectedIndex = -1
                        bufferDrawer.close()
                    }
                }
            }
            Rectangle {
                height: 1
                Layout.fillWidth: true
                color: "dark gray"
            }


            ListView {
                clip: true
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: stuff
                delegate: Rectangle {
                    width: ListView.view.width
                    height: childrenRect.height + 12
                    color: bufferMouse.pressed ? "gray" : palette.base
                    Behavior on color {
                        ColorAnimation {

                        }
                    }

                    Text {
                        x: 3
                        y: 6
                        text: buffer.name
                        font.family: "Menlo"
                        font.pointSize: 20
                        color: palette.windowText
                        MouseArea {
                            id: bufferMouse
                            anchors.fill: parent
                            onClicked: {
                                stuff.selectedIndex = index
                                bufferDrawer.visible = false
                            }
                        }
                    }
                }
            }
        }
    }
}
