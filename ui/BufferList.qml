import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Drawer {
    id: root
    SystemPalette {
        id: palette
    }
    onVisibleChanged: {
        if (visible)
            filterField.focus = true
    }

    Rectangle {
        anchors.fill: parent
        color: palette.window

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 3

            RowLayout {
                Layout.fillWidth: true

                Item {
                    width: 6
                }
                Text {
                    font.pointSize: settings.baseFontSize * 1.5
                    text: "Lith"
                    color: palette.windowText
                }
                Item {
                    Layout.fillWidth: true
                }

                Button {
                    font.pointSize: settings.baseFontSize * 1.25
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
            TextField {
                id: filterField
                Layout.fillWidth: true
                placeholderTextColor: palette.alternateBase
                placeholderText: "Filter buffers"
                text: lith.buffers.filterWord
                onTextChanged: lith.buffers.filterWord = text
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize
                color: palette.windowText

                Keys.onPressed: {
                    if (event.key === Qt.Key_Up) {
                        bufferList.currentIndex--;
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Down) {
                        bufferList.currentIndex++;
                        event.accepted = true
                    }
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        lith.selectedBuffer = bufferList.currentItem.buffer
                        filterField.text = ""
                        root.close()
                    }
                }
                /*
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        lith.selectedBufferIndex = -1
                        bufferDrawer.close()
                    }
                }
                */
            }
            Rectangle {
                height: 1
                Layout.fillWidth: true
                color: "dark gray"
            }


            ListView {
                id: bufferList
                clip: true
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: lith.buffers
                currentIndex: 0
                reuseItems: true

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                    hoverEnabled: true
                    active: hovered || pressed
                    orientation: Qt.Vertical
                }

                delegate: Rectangle {
                    width: ListView.view.width
                    height: childrenRect.height + 12
                    property var buffer: modelData
                    color: index == bufferList.currentIndex ? "#bb6666" : bufferMouse.pressed ? "gray" : palette.base

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
                            text: buffer.short_name === "" ? buffer.name : buffer.short_name
                            font.family: "Menlo"
                            font.pointSize: settings.baseFontSize * 1.125
                            color: palette.windowText
                            MouseArea {
                                id: bufferMouse
                                anchors.fill: parent
                                onClicked: {
                                    lith.selectedBuffer = buffer
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
                                font.pointSize: settings.baseFontSize
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
