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

        property var doAction: function(operation) {
            console.log("Doing: " + operation)

            lith.selectedBuffer.input("/" + operation + " " + nickname)
            nickListActionMenuDialog.close()
            nickDrawer.close()
        }

        ListElement {
            name: "Open query"
            property var operation: function() {
                // this obv doesn't switch to query buffe yet, soon(TM)
                nicklistActionMenuModel.doAction("query")
            }
        }
        ListElement {
            name: "Op"
            property var operation: function() {
                nicklistActionMenuModel.doAction("op")
            }
        }
        ListElement {
            name: "Deop"
            property var operation: function() {
                nicklistActionMenuModel.doAction("deop")
            }
        }
        ListElement {
            name: "Kick"
            property var operation: function() {
                nicklistActionMenuModel.doAction("kick")
            }
        }
        ListElement {
            name: "Kickban"
            property var operation: function() {
                nicklistActionMenuModel.doAction("kickban")
            }
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
                    onClicked: operation()
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
