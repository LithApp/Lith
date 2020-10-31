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


    ListModel {
        id: nicklistActionMenuModel

        ListElement {
            name: "Open query"
            operation: "query"
        }
        ListElement {
            name: "Op"
            operation: "op"
        }
        ListElement {
            name: "Deop"
            operation: "deop"
        }
        ListElement {
            name: "Kick"
            operation:"kick"
        }
        ListElement {
            name: "Kickban"
            operation: "kickban"

        }
    }

    ColumnLayout {
        /*Layout.alignment: Qt.AlignHCenter*/

        width: parent.width
        anchors.centerIn: parent
        GridLayout {
            width: parent.fillWidth
            columns: 1
            rowSpacing: 12

            Repeater {
                model: nicklistActionMenuModel
                delegate: Button {
                    text: name
                    onClicked: {
                        lith.selectedBuffer.input("/" + operation + " " + nickname)
                        nickListActionMenuDialog.close()
                        nickDrawer.close()
                    }
                    width: parent.fillWidth
                    Layout.fillWidth: true
                    font.family: "Menlo"
                    font.pointSize: settings.baseFontSize * 1.125
                }
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }

}
