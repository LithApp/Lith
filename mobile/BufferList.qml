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

            RowLayout {
                Layout.fillWidth: true

                Item {
                    width: 6
                }
                Text {
                    font.pointSize: 26
                    text: "Lith"
                }
                Item {
                    Layout.fillWidth: true
                }

                Button {
                    font.pointSize: 20
                    text: "⚙"
                    onClicked: settingsDialog.visible = true
                }
                Item {
                    width: 6
                }
            }

            Item {
                height: 1
            }
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: "Open Buffers"
                font.family: "Menlo"
                font.pointSize: 16
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

                    RowLayout {
                        x: 3
                        y: 6
                        width: parent.width - 6
                        Text {
                            id: bufferName
                            Layout.fillWidth: true
                            clip: true
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
                        Rectangle {
                            visible: modelData.hotMessages > 0 || modelData.unreadMessages > 0
                            color: modelData.hotMessages ? "red" : palette.alternateBase
                            border.color: palette.text
                            border.width: 1
                            height: bufferName.height + 6
                            width: height
                            radius: 2
                            Text {
                                text: modelData.hotMessages > 0 ? modelData.hotMessages : modelData.unreadMessages
                                font.family: "Menlo"
                                font.pointSize: 18
                                anchors.centerIn: parent
                                color: palette.windowText
                            }
                        }
                    }
                }
            }
        }
    }
}
