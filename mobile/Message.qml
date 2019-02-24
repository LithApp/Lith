import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtMultimedia 5.9 as Multimedia
import QtWebView 1.1

import lith 1.0

ColumnLayout {
    SystemPalette {
        id: palette
    }

    width: ListView.view.width
    rotation: 180
    RowLayout {
        Layout.fillWidth: true
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
                        text: "⤶"
                        rotation: 180
                        visible: modelData.type !== LineSegment.PLAIN
                        font.pointSize: 20
                        Layout.preferredWidth: height
                        onClicked: Qt.openUrlExternally(modelData.plainText)
                    }
                    Button {
                        text: "🎨"
                        visible: modelData.type === LineSegment.EMBED || modelData.type === LineSegment.VIDEO || modelData.type === LineSegment.IMAGE
                        font.family: "Menlo"
                        font.pointSize: 18
                        Layout.preferredWidth: height
                        onClicked: {
                            if (modelData.type === LineSegment.EMBED) {
                                delegateWeb.url = modelData.embedUrl
                                webWrapper.visible = !webWrapper.visible
                            }
                            else if (modelData.type === LineSegment.IMAGE) {
                                delegateImage.source = modelData.plainText
                                imageWrapper.visible = !imageWrapper.visible
                            }
                            else if (modelData.type === LineSegment.VIDEO) {
                                delegateVideo.source = modelData.plainText
                                videoWrapper.visible = !videoWrapper.visible
                            }
                        }
                    }
                }
            }
            Rectangle {
                id: imageWrapper
                color: palette.text
                Layout.fillWidth: true
                height: delegateImage.height + 2
                Layout.preferredHeight: delegateImage.height
                visible: false
                Image {
                    id: delegateImage
                    x: 1
                    y: 1
                    fillMode: Image.PreserveAspectFit
                    width: parent.width - 2
                }
            }
            Rectangle {
                id: videoWrapper
                color: palette.text
                Layout.fillWidth: true
                height: delegateVideo.height + 2
                Layout.preferredHeight: delegateVideo.height
                visible: false
                Multimedia.Video {
                    id: delegateVideo
                    x: 1
                    y: 1
                    autoPlay: true
                    fillMode: Image.PreserveAspectFit
                    width: parent.width - 2
                    height: width
                }
            }
            Rectangle {
                id: webWrapper
                color: palette.text
                Layout.fillWidth: true
                height: delegateWeb.height + 2
                Layout.preferredHeight: delegateWeb.height
                visible: false
                WebView {
                    id: delegateWeb
                    x: 1
                    y: 1
                    width: parent.width - 2
                    height: width
                }
            }
        }
        Item {
            Layout.fillWidth: true
        }
    }
}
