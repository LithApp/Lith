import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import lith 1.0

Dialog {
    id: nickListActionMenuDialog
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    property string nickname;

    SystemPalette {
        id: palette
    }

    header: Label {
        topPadding: 20
        id: nickActionNicknameText
        text: nickname
        horizontalAlignment: Qt.AlignCenter
        font.pointSize: settings.baseFontSize * 1.125
        font.bold: true
    }

    background: Rectangle {
        color: "#eeeeee"
    }

    onVisibleChanged: {
        console.log(nickListActionMenuDialog.x, nickListActionMenuDialog.y)
    }

    ColumnLayout {
        /*Layout.alignment: Qt.AlignHCenter*/
        width: parent.width
        anchors.centerIn: parent
        GridLayout {
            width: parent.fillWidth
            columns: 1
            rowSpacing: 12
            Button {
                width: parent.fillWidth
                Layout.fillWidth: true
                text: "Open query"
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1.125
                onClicked: {
                    /*console.log("kicking", nickname)
                    lith.selectedBuffer.input("/query " + nickname)
                    nickListActionMenuDialog.close()*/

                    console.log("Doing fuck all, for now.")
                    nickListActionMenuDialog.close()
                    nickDrawer.close()
                }
            }
            Button {
                width: parent.fillWidth
                Layout.fillWidth: true
                text: "Op user"
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1.125
                onClicked: {
                    lith.selectedBuffer.input("/op " + nickname)
                    nickListActionMenuDialog.close()
                    nickDrawer.close()
                }
            }
            Button {
                width: parent.fillWidth
                Layout.fillWidth: true
                text: "Deop user"
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1.125
                onClicked: {
                    lith.selectedBuffer.input("/deop " + nickname)
                    nickListActionMenuDialog.close()
                    nickDrawer.close()
                }
            }
            Button {
                width: parent.fillWidth
                Layout.fillWidth: true
                text: "Kick user"
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1.125
                onClicked: {
                    lith.selectedBuffer.input("/kick " + nickname)
                    nickListActionMenuDialog.close()
                    nickDrawer.close()
                }
            }
            Button {
                // font.capitalization: Font.MixedCase
                width: parent.fillWidth
                Layout.fillWidth: true
                text: "Kickban user"
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1.125
                onClicked: {
                    lith.selectedBuffer.input("/kickban " + nickname)
                    nickListActionMenuDialog.close()
                    nickDrawer.close()
                }
            }

        }
        Item {
            Layout.fillHeight: true
        }
    }

}
