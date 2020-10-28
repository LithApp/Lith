import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import lith 1.0

Dialog {
    id: channelMessageActionMenuDialog
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    anchors.centerIn: parent

    width: 400

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
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }
            }

            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: parent.width
                maximumLineCount: 2
                id: messageText
                visible: nickname == "" ? false : true
                text: "<" + nickname + "> " + message;

                clip: true
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
                font.pointSize: settings.baseFontSize

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }

            }

            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: parent.width
                maximumLineCount: 2
                id: messageTextWithNickname
                text: message;

                clip: true
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                font.family: "Menlo"
                font.pointSize: settings.baseFontSize

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        channelMessageActionMenuDialog.close()
                        clipboard.setText(parent.text)
                    }
                }
            }
        }
    }
}


