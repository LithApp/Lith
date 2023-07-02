import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: debugDialog
    anchors.centerIn: Overlay.overlay

    Rectangle {
        anchors.fill: parent
        color: palette.window
    }

    ColumnLayout {
        width: parent.width
        height: parent.height
        RowLayout {
            Layout.fillWidth: true
            TextField {
                placeholderText: "Context filter"
                text: lith.logger.contextFilter
                onTextChanged: lith.logger.contextFilter = text
            }
            Button {
                text: "Close"
                onClicked: debugDialog.visible = false
            }
            CheckBox {
                checked: lith.logger.showLineAdded
                onCheckedChanged: lith.logger.showLineAdded = checked
            }
            CheckBox {
                checked: lith.logger.showProtocol
                onCheckedChanged: lith.logger.showProtocol = checked
            }
            CheckBox {
                checked: lith.logger.showDetails
                onCheckedChanged: lith.logger.showDetails = checked
            }
        }
        ScrollView {
            id: listScrollView
            Layout.fillHeight: true
            Layout.fillWidth: true
            ListView {
                id: list
                implicitWidth: contentWidth
                implicitHeight: contentHeight
                clip: true
                model: lith.logger
                // TODO fix this later, need to push it now
                delegate: ColumnLayout {
                    spacing: 1
                    RowLayout {
                        spacing: 0
                        opacity: 0.6
                        Label {
                            Layout.preferredWidth: 64
                            text: model.dateTime
                        }
                        Label {
                            Layout.preferredWidth: 64
                            text: model.type
                        }
                        Label {
                            Layout.preferredWidth: 128
                            text: model.context
                        }
                    }
                    RowLayout {
                        Label {
                            visible: lith.logger.showDetails && model.details.length > 0
                            text: model.details
                            opacity: 0.6
                        }
                    }
                    Label {
                        Layout.minimumWidth: implicitWidth
                        text: model.summary
                    }
                }
            }
        }

        TableView {
            id: table
            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: false // TODO decide which one to use, maybe different ones for desktop and mobile
            model: null //lith.logger
            clip: true
            reuseItems: true

            readonly property point hoveredPoint: {
                return table.cellAtPosition(tableMouse.mouseX, tableMouse.mouseY, false)
            }

            delegate: Rectangle {
                implicitWidth: delegateLabel.implicitWidth + 12
                implicitHeight: delegateLabel.implicitHeight + 2

                color: {
                    if (row === table.hoveredPoint.y) {
                        var c = Qt.color(palette.windowText)
                        c.a = 0.1
                        return c
                    }
                    return "transparent"
                }
                Label {
                    x: 6
                    y: 1
                    id: delegateLabel
                    text: model.display
                }
            }

            onRowsChanged: {
            }

            MouseArea {
                id: tableMouse
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                hoverEnabled: true
            }
        }
    }
}
