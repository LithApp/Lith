import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

import lith 1.0

ColumnLayout {
    SystemPalette {
        id: palette
    }

    width: ListView.view.width + timeMetrics.width
    rotation: 180
    RowLayout {
        Layout.fillWidth: true
        Text {
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: timeMetrics.width
            text: line.date.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
            font.family: "Menlo"
            font.pointSize: 16
            color: palette.text
        }

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
                        focusPolicy: Qt.NoFocus
                        text: "⤶"
                        rotation: 180
                        visible: modelData.type !== LineSegment.PLAIN
                        font.pointSize: 20
                        Layout.preferredWidth: height
                        onClicked: Qt.openUrlExternally(modelData.plainText)
                    }
                    Button {
                        focusPolicy: Qt.NoFocus
                        text: "🖼️"
                        visible: modelData.type === LineSegment.IMAGE // || modelData.type === LineSegment.VIDEO || modelData.type === LineSegment.EMBED
                        font.family: "Menlo"
                        font.pointSize: 18
                        Layout.preferredWidth: height
                        onClicked: {
                            if (modelData.type === LineSegment.IMAGE) {
                                delegateImage.source = modelData.plainText
                                imageWrapper.visible = !imageWrapper.visible
                            }
                            /*
                            else if (modelData.type === LineSegment.EMBED) {
                                delegateWeb.url = modelData.embedUrl
                                webWrapper.visible = !webWrapper.visible
                            }
                            else if (modelData.type === LineSegment.VIDEO) {
                                delegateVideo.source = modelData.plainText
                                videoWrapper.visible = !videoWrapper.visible
                            }
                            */
                            previewDialog.visible = true
                        }
                    }
                }
            }
        }
        Item {
            Layout.fillWidth: true
        }
    }
}
