import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import lith 1.0

/* Hold-to-copy action menu */

Dialog {
    id: channelMessageActionMenuDialog
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    width: 400

    // Center the popup/dialog, whatever
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)


    property string message;
    property string nickname;
    property string timestamp;

    SystemPalette {
        id: palette
    }

    header: Label {
        topPadding: 20
        leftPadding: 20
        text: "Copy"
        font.bold: true
        font.pointSize: settings.baseFontSize * 1.125
    }

    background: Rectangle {
        color: "#eeeeee"
    }
    /*Overlay.modal: Rectangle {
        color: "#aacfdbe7"
    }*/

    ColumnLayout {

        width: parent.width
        GridLayout
        {
            rowSpacing: 32
            columns: 1
            width: parent.width


            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: parent.width
                maximumLineCount: 2
                id: messageTextWithNicknameTimestamp
                visible: nickname == "" ? false : true
                text: timestamp + " <" + nickname + "> " + message;

                clip: true
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
                font.pointSize: settings.baseFontSize

                MouseArea {
                    id: mouseNicknameTimestamp
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }

                Rectangle
                {
                    color: mouseNicknameTimestamp.pressed ? "gray" : "#eeeeee"
                    width: parent.width
                    height: parent.height
                    z: -1
                    Behavior on color {
                        ColorAnimation {

                        }
                    }
                }
            }

            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: parent.width
                maximumLineCount: 2
                id: messageTextWithNickname
                visible: nickname == "" ? false : true
                text: "<" + nickname + "> " + message;

                clip: true
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
                font.pointSize: settings.baseFontSize

                MouseArea {
                    id: mouseNickname
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }
                Rectangle
                {
                    color: mouseNickname.pressed ? "gray" : "#eeeeee"
                    width: parent.width
                    height: parent.height
                    z: -1
                    Behavior on color {
                        ColorAnimation {

                        }
                    }
                }
            }

            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: parent.width
                maximumLineCount: 2
                id: messageText
                text: message;

                clip: true
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
                font.pointSize: settings.baseFontSize

                MouseArea {
                    id: mouseMessage
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }
                Rectangle
                {
                    color: mouseMessage.pressed ? "gray" : "#eeeeee"
                    width: parent.width
                    height: parent.height
                    z: -1
                    Behavior on color {
                        ColorAnimation {

                        }
                    }
                }
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }
}


