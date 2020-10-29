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
    }

    background: Rectangle {
        color: "#eeeeee"
    }

    onVisibleChanged: {
        console.log(nickListActionMenuDialog.x, nickListActionMenuDialog.y)
    }

    ColumnLayout {
        /*Layout.alignment: Qt.AlignHCenter*/

        anchors.centerIn: parent
        GridLayout {
            columns: 1
            rowSpacing: 12
            Button {
                width: parent.fillWidth
                text: "Open query"
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1.125
                onClicked: {
                    /*console.log("kicking", nickname)
                    lith.selectedBuffer.input("/query " + nickname)
                    bufferDrawer.visible = false
                    nickListActionMenuDialog.close()*/

                    console.log("Doing fuck all.")
                }
            }
            Button {
                width: parent.fillWidth
                text: "Kick user"
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1.125
                onClicked: {
                    console.log("kicking", nickname)
                    lith.selectedBuffer.input("/say /kick " + nickname)
                    nickListActionMenuDialog.close()
                }
            }

            Button {
                width: parent.fillWidth
                text: "Kickban user"
                font.family: "Menlo"
                font.pointSize: settings.baseFontSize * 1.125
                onClicked: {
                    console.log("kicking", nickname)
                    lith.selectedBuffer.input("/say /kick " + nickname)
                    nickListActionMenuDialog.close()
                }
            }

        }
        Item {
            Layout.fillHeight: true
        }
    }

}
