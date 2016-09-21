import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Lith"

    statusBar: StatusBar {
        RowLayout {
            anchors.fill: parent
            Text {
                text: qsTr(stuff.bufferCount + " buffers")
            }
            Item {
                height: 1
                Layout.fillWidth: true
            }
            Text {
                visible: weechat.fetchTo !== 0
                text: "Syncing"
            }
            ProgressBar {
                visible: weechat.fetchTo !== 0
                minimumValue: 0
                value: weechat.fetchFrom.toFixed(1) / weechat.fetchTo.toFixed(1)
            }
        }
    }
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        ColumnLayout {
            spacing: 0
            Button {
                implicitWidth: 200
                Layout.fillWidth: true
                text: "Lith"
                onClicked: settings.open()
            }
            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: stuff
                delegate: Rectangle {
                    color: mouse.containsPress ? "gray" : mouse.containsMouse ? Qt.lighter("gray") : index == stuff.selectedIndex ? Qt.darker("light gray") : "light gray"
                    Behavior on color { ColorAnimation { duration: 60 } }
                    width: parent.width
                    height: childrenRect.height
                    Text {
                        text: buffer.name.split(".")[buffer.name.split(".").length - 1]
                    }
                    MouseArea {
                        id: mouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: stuff.selectedIndex = index
                    }
                }
            }
        }
        Rectangle {
            StatusBar {
                id: chatTitle
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                RowLayout {
                    Button {
                        text: "Gimme backlog"
                        onClicked: stuff.selected.fetchMoreLines()
                    }
                    Text {
                        text: stuff.selected ? stuff.selected.name + ": " + stuff.selected.title : "(No buffer selected)"
                    }
                }
            }
            ListView {
                id: messageListView
                anchors {
                    top: chatTitle.bottom
                    bottom: chatInput.top
                    left: parent.left
                    right: parent.right
                }

                clip: true
                spacing: 3

                onCountChanged: {
                    currentIndex = count - 1
                }
                onWidthChanged: {
                    positionViewAtEnd()
                }
                onHeightChanged: {
                    positionViewAtEnd()
                }

                snapMode: ListView.SnapToItem
                highlightRangeMode: ListView.ApplyRange
                highlightMoveDuration: 100
                highlightFollowsCurrentItem: true
                highlightResizeDuration: 100
                preferredHighlightBegin: height - highlightItem.height
                preferredHighlightEnd: height - highlightItem.height
                highlight: Rectangle {
                    color: "#88ff0000"
                }

                model: stuff.selected.lines

                delegate: RowLayout {
                    width: parent.width
                    spacing: 0
                    Text {
                        font.family: "Consolas"
                        Layout.alignment: Qt.AlignTop | Qt.AlignRight
                        color: "gray"
                        text: Qt.formatTime(line.date, Qt.DefaultLocaleShortDate) + " " + line.prefix + " "
                    }
                    Text {
                        font.family: "Consolas"
                        Layout.fillWidth: true
                        text: line.message
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
            }

            TextField {
                id: chatInput
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
                onAccepted: {
                    stuff.selected.input(chatInput.text)
                    chatInput.text = ""
                }
            }
        }
    }

    Settings {
        id: settings
    }
}
