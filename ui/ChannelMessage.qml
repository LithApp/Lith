import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

import lith 1.0

Rectangle {
    width: ListView.view.width // + timeMetrics.width
    property var messageModel: null
    rotation: 180
    height: messageRootLayout.height

    color: messageModel.highlight ? "#22880000" : "transparent"

    ColumnLayout {
        id: messageRootLayout
        width: parent.width

        SystemPalette {
            id: palette
        }

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
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.alignment: Qt.AlignTop
                            verticalAlignment: Text.AlignTop
                            text: modelData.type === LineSegment.PLAIN ? modelData.plainText : (settings.shortenLongUrls ? modelData.summary : modelData.plainText)
                            textFormat: Text.RichText
                            // text: modelData.type ===
                            color: modelData.type === LineSegment.PLAIN ? palette.text : palette.highlight
                            font.underline: modelData.type !== LineSegment.PLAIN
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.family: "Menlo"
                            font.pointSize: settings.baseFontSize
                            MouseArea {
                                anchors.fill: parent
                                pressAndHoldInterval: 400 // does this do anything? or is it just for signals?
                                onPressAndHold: {
                                    // no rofl, tohle tady je, protoze "prefix" uz obsahuje barvicky
                                    // TODO: fakt nevim
                                    console.log(messageModel.colorlessNickname)

                                    channelMessageActionMenu.visible = true

                                    // TODO: how to do this better? neco s modelama? nevim jak to tam poslat, kdyz ten dialog mam jako jiny QML file :(
                                    channelMessageActionMenu.message = messageModel.colorlessText;

                                    channelMessageActionMenu.nickname = messageModel.colorlessNickname;
                                    channelMessageActionMenu.timestamp = messageModel.date.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                                }

                            }
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
                            visible: modelData.type === LineSegment.IMAGE || modelData.type === LineSegment.VIDEO // || modelData.type === LineSegment.EMBED
                            font.family: "Menlo"
                            font.pointSize: settings.baseFontSize * 1.125
                            Layout.preferredWidth: height
                            onClicked: {
                                if (modelData.type === LineSegment.IMAGE) {
                                    previewPopup.showImage(modelData.plainText)
                                }
                                else if (modelData.type === LineSegment.VIDEO) {
                                    previewPopup.showVideo(modelData.plainText)
                                }
                                /*
                                else if (modelData.type === LineSegment.EMBED) {
                                    delegateWeb.url = modelData.embedUrl
                                    webWrapper.visible = !webWrapper.visible
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
}
