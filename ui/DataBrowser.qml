import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    ScrollView {
        anchors.fill: parent
        ListView {
            id: bufferListView
            spacing: 0
            model: lith.unfilteredBuffers
            header: Item {
                width: bufferListView.width
                height: childrenRect.height
                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 1
                        left: parent.left
                        right: parent.right
                    }
                    height: 1
                    color: "black"
                }

                Text {
                    text: "Buffer"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    width: 256
                }
                Text {
                    x: 256
                    text: "Nick"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    width: 256
                }
                Text {
                    x: 512
                    text: "Message"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width - 512
                }
            }
            delegate: Item {
                height: Math.max(messageListView.height, nickListView.height)
                width: 256 + 256 + messageListView.contentWidth
                Text {
                    width: 256
                    text: modelData.name
                    font.family: "monospace"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: viewer.obj = modelData
                    }
                }
                ListView {
                    id: nickListView
                    spacing: 0
                    x: 256
                    width: 256
                    height: contentHeight
                    model: modelData.nicks
                    delegate: Text {
                        Layout.fillWidth: true
                        font.family: "monospace"
                        text: modelData.name
                        Rectangle {
                            z: -1
                            anchors {
                                left: parent.left
                                leftMargin: 1
                                top: parent.top
                                bottom: parent.bottom
                            }
                            width: 1
                            color: "light gray"
                        }
                        Rectangle {
                            z: -1
                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }
                            x: 256
                            width: 1
                            color: "light gray"
                        }
                        Rectangle {
                            z: -1
                            anchors {
                                bottom: parent.bottom
                                bottomMargin: 1
                            }
                            width: 10000
                            height: 1
                            color: "light gray"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: viewer.obj = modelData
                        }
                    }
                }
                ListView {
                    spacing: 0
                    id: messageListView
                    x: 256 + 256
                    height: contentHeight
                    width: contentWidth
                    model: modelData.lines
                    delegate: Text {
                        Layout.fillWidth: true
                        font.family: "monospace"
                        text: modelData.message
                        Rectangle {
                            z: -1
                            anchors {
                                left: parent.left
                                leftMargin: 1
                                top: parent.top
                                bottom: parent.bottom
                            }
                            width: 1
                            color: "light gray"
                        }
                        Rectangle {
                            z: -1
                            anchors {
                                bottom: parent.bottom
                                bottomMargin: 1
                            }
                            x: -256
                            width: 10000
                            height: 1
                            color: "light gray"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: viewer.obj = modelData
                        }
                    }
                }
                Rectangle {
                    z: -1
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 1
                        left: parent.left
                        right: parent.right
                    }
                    height: 1
                    color: "gray"
                }
            }
        }
    }
    Dialog {
        id: viewer
        property var obj: null
        visible: obj !== null
        onVisibleChanged: if (!visible) obj = null
        width: parent.width
        height: parent.height
        onObjChanged: {
            if (obj) {
                title.text = obj.toString()
                description.text = JSON.stringify(obj, null, 4)
            }
        }
        ScrollView {
            anchors.fill: parent
            ColumnLayout {
                TextEdit {
                    id: title
                    readOnly: true
                    font.family: "monospace"
                    Layout.fillWidth: true
                }
                TextEdit {
                    id: description
                    readOnly: true
                    font.family: "monospace"
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    Layout.fillWidth: true
                }
            }
        }
    }
}
