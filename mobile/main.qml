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
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: stuff.selected ? stuff.selected.lines : null
            delegate: Text {
                width: ListView.view.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "monospace"
                font.pointSize: 16
                text: sender + ": " + line.message
            }
        }
        TextField {
            Layout.fillWidth: true
            onAccepted: {
                if (text.length > 0) {
                    stuff.selected.input(text)
                    text = ""
                }
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
