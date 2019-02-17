import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtMultimedia 5.9 as Multimedia

import lith 1.0

ApplicationWindow {
    visible: true
    width: 480
    height: 800
    title: "Lith"

    SystemPalette {
        id: palette
    }

    Drawer {
        id: bufferDrawer
        width: 0.66 * parent.width
        height: parent.height

        Rectangle {
            anchors.fill: parent
            color: palette.base

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
                    color: palette.text
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
                            color: palette.text
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

    Rectangle {
        anchors.fill: parent
        color: palette.base
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
                    color: palette.text
                    Layout.fillWidth: true
                    horizontalAlignment: Label.AlignHCenter
                    font.pointSize: 16
                    text: stuff.selected ? stuff.selected.name : ""
                }
                Button {
                    Layout.preferredWidth: height
                    font.pointSize: 20
                    text: "⚙"
                    onClicked: settingsDialog.visible = true
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
                        font.family: "Menlo"
                        font.pointSize: 16
                        color: palette.text
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Repeater {
                            model: line.segments
                            RowLayout {
                                Layout.fillWidth: true
                                Text {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.alignment: Qt.AlignTop
                                    verticalAlignment: Text.AlignTop
                                    text: modelData.type === LineSegment.PLAIN ? modelData.plainText : modelData.summary
                                    color: modelData.type === LineSegment.PLAIN ? palette.text : palette.highlight
                                    font.underline: modelData.type !== LineSegment.PLAIN
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    font.family: "Menlo"
                                    font.pointSize: 16
                                }
                                Button {
                                    text: "⤶"
                                    rotation: 180
                                    visible: modelData.type !== LineSegment.PLAIN
                                    font.pointSize: 20
                                    Layout.preferredWidth: height
                                    onClicked: Qt.openUrlExternally(modelData.plainText)
                                }
                                Button {
                                    text: "🎨"
                                    Component.onCompleted: console.log(modelData.type)
                                    visible: modelData.type === LineSegment.VIDEO || modelData.type === LineSegment.IMAGE
                                    font.family: "Menlo"
                                    font.pointSize: 18
                                    Layout.preferredWidth: height
                                    onClicked: {
                                        if (!delegateImageWrapper.visible) {
                                            delegateImage.source = modelData.plainText
                                            delegateVideo.source = modelData.plainText
                                            delegateImageWrapper.visible = true
                                        }
                                        else
                                            delegateImageWrapper.visible = false
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: delegateImageWrapper
                            color: palette.text
                            Layout.fillWidth: true
                            height: childrenRect.height + 2
                            Layout.preferredHeight: childrenRect.height
                            visible: false
                            Image {
                                x: 1
                                y: 1
                                fillMode: Image.PreserveAspectFit
                                width: parent.width - 2
                                id: delegateImage
                            }
                            Multimedia.Video {
                                x: 1
                                y: 1
                                autoPlay: true
                                fillMode: Image.PreserveAspectFit
                                width: parent.width - 2
                                height: width
                                id: delegateVideo
                            }
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }
                /*Text {
                width: ListView.view.width
                rotation: 180
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
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
                background: Rectangle {
                    color: palette.base
                }
                color: palette.text
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

    Dialog {
        id: settingsDialog
        anchors.centerIn: parent
        title: "Settings"
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape
        standardButtons: Dialog.Ok | Dialog.Cancel

        header: Rectangle {
            color: palette.base
            height: 48
            width: settingsDialog.width
            Text {
                anchors.centerIn: parent
                text: "Settings"
                color: palette.text
            }
        }

        background: Rectangle {
            color: palette.base
        }

        onAccepted: {
            weechat.host = hostField.text
            weechat.port = portField.text
            weechat.encrypted = encryptedCheckbox.checked
            if (passphraseField.text.length > 0)
                weechat.setPassphrase(passphraseField.text)
            passphraseField.text = ""
        }
        onRejected: {
            hostField.text = weechat.host
            portField.text = weechat.port
            encryptedCheckbox.checked = weechat.encrypted
            passphraseField.text = ""
        }
        GridLayout {
            anchors.fill: parent
            columns: 2
            Text {
                text: "Hostname"
                color: palette.text
            }
            TextField {
                id: hostField
                text: weechat.host
                inputMethodHints: Qt.ImhNoPredictiveText
            }
            Text {
                text: "Port"
                color: palette.text
            }
            TextField {
                id: portField
                text: weechat.port
                inputMethodHints: Qt.ImhPreferNumbers
                validator: IntValidator {
                    bottom: 0
                    top: 65535
                }
            }
            Text {
                text: "SSL"
                color: palette.text
            }
            CheckBox {
                id: encryptedCheckbox
                checked: weechat.encrypted
                Layout.alignment: Qt.AlignLeft
            }
            Text {
                text: "Password"
                color: palette.text
            }
            TextField {
                id: passphraseField
                placeholderText: weechat.hasPassphrase ? "*****" : ""
                echoMode: TextInput.Password
                passwordCharacter: "*"
            }
        }
    }
}
