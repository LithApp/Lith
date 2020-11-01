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

    width: parent.width / 1.25

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

    ListModel {
        id: channelMessageActionMenuModel

        ListElement {
            textVal: "%3 <%2> %1"
        }
        ListElement {
            textVal: "<%2> %1"
        }
        ListElement {
            textVal: "%1"
        }
    }

    ColumnLayout {

        width: parent.width
        GridLayout
        {
            rowSpacing: 32
            columns: 1
            width: parent.width

            Repeater {
                model: channelMessageActionMenuModel
                delegate: Text {
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: parent.width
                    maximumLineCount: 2
                    visible: nickname == "" && index != 2 ? false : true
                    text: index == 0 ? textVal.arg(message).arg(nickname).arg(timestamp) :
                          index == 1 ? textVal.arg(message).arg(nickname) :
                                       textVal.arg(message)

                    clip: true
                    elide: Text.ElideRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    font.family: "Menlo"
                    font.pointSize: settings.baseFontSize

                    MouseArea {
                        id: mouseActionMenuItem
                        anchors.fill: parent
                        onClicked: {
                            channelMessageActionMenuDialog.close()
                            clipboard.setText(parent.text)
                        }
                    }

                    Rectangle
                    {
                        color: mouseActionMenuItem.pressed ? "gray" : "#eeeeee"
                        width: parent.width
                        height: parent.height
                        z: -1
                        Behavior on color {
                            ColorAnimation {

                            }
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


