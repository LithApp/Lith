import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

import "."
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
        Item {
            clip: true
            Layout.fillWidth: true

            Item {
                id: chatTitle
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height
                RowLayout {
                    width: parent.width
                    Button {
                        text: "Gimme backlog"
                        onClicked: stuff.selected.fetchMoreLines()
                    }
                    Text {
                        font.family: "Consolas"
                        text: stuff.selected ? stuff.selected.name + ": " + stuff.selected.title : "(No buffer selected)"
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    Button {
                        text: nickList.open ? ">>" : "<< Nicklist"
                        onClicked: nickList.open = !nickList.open
                    }
                }
            }

            Rectangle {
                anchors.fill: messageScrollView
                color: "white"
            }

            ScrollView {
                id: messageScrollView
                anchors {
                    top: chatTitle.bottom
                    bottom: chatInput.top
                    left: parent.left
                    right: parent.right
                }
                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

                ListView {
                    id: messageListView
                    rotation: 0
                    width: parent.width
                    interactive: false

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

                    model: stuff.selected ? stuff.selected.lines : null
                    section.property: "sender"
                    section.delegate: Text {
                        text: section
                        height: 0
                        color: "gray"
                        font.family: "Consolas"
                    }

                    delegate: Line { }
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
                property string lookingFor: ""
                property string lastTried: ""
                property bool ignoreThisOne: false
                Keys.onTabPressed: {
                    event.accepted = true
                    var i = 0
                    var arr = stuff.selected.nicks
                    if (lastTried.length > 0) {
                        for (i = 0; i < arr.length; i++) {
                            if (arr[i].name === lastTried) {
                                break
                            }
                        }
                        i++
                    }
                    for (; i < arr.length; i++) {
                        if (arr[i].name.toLowerCase().startsWith(lookingFor.toLowerCase())) {
                            ignoreThisOne = true
                            text = arr[i].name + ": "
                            lastTried = arr[i].name
                            return
                        }
                    }

                    for (i = 0; i < arr.length; i++) {
                        if (arr[i].name.toLowerCase().startsWith(lookingFor.toLowerCase())) {
                            ignoreThisOne = true
                            text = arr[i].name + ": "
                            lastTried = arr[i].name
                            return
                        }
                    }
                }
                Keys.onPressed: {
                    if (event.key === Qt.Key_V && event.modifiers & Qt.ControlModifier) {
                        var leftPart = text.substr(0, selectionStart)
                        var insertedPart
                        var rightPart = text.substr(selectionEnd)
                        if (clipboard.hasImage()) {
                            insertedPart = "<unsupported>"
                        }
                        else {
                            insertedPart = clipboard.text()
                        }
                        text = leftPart + insertedPart + rightPart
                        cursorPosition = (leftPart + insertedPart).length
                        event.accepted = true
                    }
                    else {
                        event.accepted = false
                    }
                }
                onTextChanged: {
                    if (ignoreThisOne)
                        ignoreThisOne = false
                    else {
                        lookingFor = text
                        lastTried = ""
                    }
                }
            }
        }
        NickList {
            id: nickList
            clip: true
            model: stuff.selected ? stuff.selected.nicks : null
            implicitWidth: 200
        }
    }

    Settings {
        id: settings
    }
}
