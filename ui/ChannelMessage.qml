import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

import lith 1.0

ColumnLayout {
    SystemPalette {
        id: palette
    }
    property var messageModel: null

    width: ListView.view.width // + timeMetrics.width
    rotation: 180
    RowLayout {
        Layout.fillWidth: true
        /*
        Text {
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: timeMetrics.width
            text: messageModel.date.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize
            color: palette.text
        }
        */

        Text {
            Layout.alignment: Qt.AlignTop
            font.bold: true
            text: messageModel.prefix
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize
            color: palette.text
        }

        ColumnLayout {
            Layout.fillWidth: true
            Repeater {
                model: messageModel.segments
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        // Component.onCompleted: console.warn(weechat.showFullLink)
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignTop
                        verticalAlignment: Text.AlignTop
                        text: modelData.type === LineSegment.PLAIN ? modelData.plainText : (settings.showFullLink ? modelData.plainText : modelData.summary)
                        // text: modelData.type ===
                        color: modelData.type === LineSegment.PLAIN ? palette.text : palette.highlight
                        font.underline: modelData.type !== LineSegment.PLAIN
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        font.family: "Menlo"
                        font.pointSize: settings.baseFontSize
                    }
                    Button {
                        focusPolicy: Qt.NoFocus
                        text: "⤶"
                        rotation: 180
                        visible: modelData.type !== LineSegment.PLAIN
                        font.pointSize: settings.baseFontSize * 1.25
                        Layout.preferredWidth: height
                        onClicked: Qt.openUrlExternally(modelData.plainText)
                    }
                    Button {
                        focusPolicy: Qt.NoFocus
                        text: "🖼️"
                        visible: modelData.type === LineSegment.IMAGE // || modelData.type === LineSegment.VIDEO || modelData.type === LineSegment.EMBED
                        font.family: "Menlo"
                        font.pointSize: settings.baseFontSize * 1.125
                        Layout.preferredWidth: height
                        onClicked: {
                            if (modelData.type === LineSegment.IMAGE) {
                                previewPopup.showImage(modelData.plainText)
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
