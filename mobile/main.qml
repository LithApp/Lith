import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtMultimedia 5.9 as Multimedia
import QtWebView 1.1

ApplicationWindow {
    visible: true
    width: 480
    height: 800
    title: "Lith"

    SystemPalette {
        id: palette
    }

    BufferList {
        id: bufferDrawer
        width: 0.66 * parent.width
        height: parent.height
    }

    NickList {
        id: nickDrawer
        edge: Qt.RightEdge
        width: 0.66 * parent.width
        height: parent.height
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }

    FrontPage {
        anchors.fill: parent
    }

    SettingsDialog {
        id: settingsDialog
        anchors.centerIn: parent
    }

    Dialog {
        id: previewDialog
        modal: true
        width: parent.width
        height: parent.height
        header: null
        footer: null
        margins: 0
        topInset: 0
        topPadding: 0
        topMargin: 0
        bottomInset: 0
        padding: 0
        background: Item {}

        Rectangle {
            id: imageWrapper
            color: "transparent"
            anchors.fill: parent
            visible: false
            Image {
                id: delegateImage
                x: 1
                y: 1
                onVisibleChanged: {
                    x = 1
                    y = 1
                    rotation = 0
                    scale = 1
                }

                fillMode: Image.PreserveAspectFit
                width: parent.width - 2
                height: parent.height - 2
                Behavior on scale {
                    NumberAnimation {
                        duration: 60
                    }
                }
            }
            /*
            PinchArea {
                anchors.fill: parent
                pinch.target: delegateImage
                pinch.dragAxis: Pinch.XAndYAxis
                pinch.minimumX: -imageWrapper.width + 32
                pinch.maximumX: imageWrapper.width - 32
                pinch.minimumY: -imageWrapper.height + 32
                pinch.maximumY: imageWrapper.height - 32
                pinch.minimumScale: 0.001
                pinch.maximumScale: 20
            }*/

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
                onWheel: {
                    delegateImage.scale += wheel.angleDelta.y * 0.002
                }
                drag.target: delegateImage
                drag.axis: Drag.XAndYAxis
                drag.minimumX: -imageWrapper.width + 32
                drag.maximumX: imageWrapper.width - 32
                drag.minimumY: -imageWrapper.height + 32
                drag.maximumY: imageWrapper.height - 32
                onClicked: {
                    if (mouse.button == Qt.LeftButton || mouse.button == Qt.RightButton) {
                        imageWrapper.visible = false
                        previewDialog.visible = false
                    }
                    else if (mouse.button == Qt.MiddleButton) {
                        delegateImage.scale = 1.0
                        delegateImage.x = 1
                        delegateImage.y = 1
                    }
                }
            }
            PinchHandler {
                target: delegateImage
            }
            TapHandler {
                target: delegateImage
                onSingleTapped: {
                    imageWrapper.visible = false
                    previewDialog.visible = false
                }
                onDoubleTapped: {
                    if (delegateImage.scale != 2.0)
                        delegateImage.scale = 2.0
                    else
                        delegateImage.scale = 1.0
                }
            }

        }
        /*
        MouseArea {
            anchors.fill: parent
            onClicked: {
                //imageWrapper.visible = false
                //previewDialog.visible = false
            }
        }
        */

        /*
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
        */
    }
}
