import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 480
    height: 800
    title: "Lith"

    Drawer {
        id: bufferDrawer
        width: 0.66 * parent.width
        height: parent.height

        Rectangle {
            anchors.fill: parent
            color: "red"
        }

        ListView {
            anchors.fill: parent
            model: stuff
            delegate: Text {
                text: buffer.name
                font.family: "monospace"
                font.pointSize: 18
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        stuff.selectedIndex = index
                        bufferDrawer.visible = false
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        Frame {
            Layout.fillWidth: true
            RowLayout {
                width: parent.width
                Button {
                    Layout.preferredWidth: height
                    font.pointSize: 20
                    text: "☰"
                    onClicked: bufferDrawer.visible = !bufferDrawer.visible
                }
                Text {
                    clip: true
                    height: 1
                    Layout.fillWidth: true
                    horizontalAlignment: Label.AlignHCenter
                    font.pointSize: 16
                    text: stuff.selected ? stuff.selected.name : ""
                }
                Button {
                    Layout.preferredWidth: height
                    font.pointSize: 20
                    text: "⚙"
                }
                Button {
                    Layout.preferredWidth: height
                    font.pointSize: 20
                    text: "👨"
                    onClicked: nickDrawer.visible = !nickDrawer.visible
                }
            }
        }
        ListView {
            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: stuff.selected ? stuff.selected.lines : null
            rotation: 180
            delegate: ColumnLayout {
                width: ListView.view.width
                rotation: 180
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.alignment: Qt.AlignTop
                        font.bold: true
                        text: sender
                        font.family: "monospace"
                        font.pointSize: 16
                    }

                    Repeater {
                        model: line.segments
                        RowLayout {
                            Layout.fillWidth: true
                            Text {
                                Layout.fillHeight: true
                                Layout.alignment: Qt.AlignTop
                                verticalAlignment: Text.AlignTop
                                text: modelData.type == 0 ? modelData.plainText : "🔗"
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                font.family: "monospace"
                                font.pointSize: 16
                            }
                            Button {
                                text: "⤶"
                                rotation: 180
                                visible: modelData.type != 0
                                font.pointSize: 20
                                Layout.preferredWidth: height
                            }
                            Button {
                                text: "🎨"
                                visible: modelData.type == 1
                                font.family: "monospace"
                                font.pointSize: 18
                                Layout.preferredWidth: height
                                onClicked: {
                                    if (!delegateImageWrapper.visible) {
                                        delegateImage.source = modelData.plainText
                                        delegateImageWrapper.visible = true
                                    }
                                    else
                                        delegateImageWrapper.visible = false
                                }
                            }
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }
                Item {
                    id: delegateImageWrapper
                    Layout.fillWidth: true
                    height: childrenRect.height
                    Layout.preferredHeight: childrenRect.height
                    visible: false
                    Image {
                        fillMode: Image.PreserveAspectFit
                        width: parent.width
                        id: delegateImage
                    }
                }
            }
                /*Text {
                width: ListView.view.width
                rotation: 180
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "monospace"
                font.pointSize: 16
                text: line.date.toLocaleTimeString(Qt.locale(), Locale.ShortFormat) + " " + sender + ": " + line.message
                */
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 0
            Button {
                Layout.preferredWidth: height
                text: "⇥"
                font.pointSize: 14
            }
            TextField {
                Layout.fillWidth: true
                font.pointSize: 16
                onAccepted: {
                    if (text.length > 0) {
                        stuff.selected.input(text)
                        text = ""
                    }
                }
            }
            Button {
                Layout.preferredWidth: height
                text: "📷"
                font.pointSize: 15
            }
        }
    }

    Drawer {
        id: nickDrawer
        edge: Qt.RightEdge
        width: 0.66 * parent.width
        height: parent.height

        Rectangle {
            anchors.fill: parent
            color: "blue"
        }

        ListView {
            anchors.fill: parent
            model: stuff.selected ? stuff.selected.nicks : null
            delegate: Text {
                text: modelData.name
            }
        }
    }


}
