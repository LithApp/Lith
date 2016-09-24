import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

import "Default"

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
            BufferList {
                Layout.fillWidth: true
                Layout.fillHeight: true
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

                delegate: Line {}
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
