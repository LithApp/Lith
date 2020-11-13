// Lith
// Copyright (C) 2020 Martin Bříza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtMultimedia 5.12 as Multimedia
import QtWebView 1.1

Dialog {
    id: root
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

    function showImage(url) {
        delegateImage.source = url
        imageWrapper.visible = true
        videoWrapper.visible = false
        root.open()
    }
    function showVideo(url) {
        delegateVideo.source = url
        videoWrapper.visible = true
        imageWrapper.visible = false
        root.open()
    }

    Item {
        id: videoWrapper
        anchors.fill: parent
        height: delegateVideo.height + 2
        visible: false
        Multimedia.Video {
            id: delegateVideo
            x: 1
            y: 1
            autoPlay: true
            fillMode: Image.PreserveAspectFit
            width: parent.width - 2
            height: parent.height - 2
            anchors.centerIn: parent
        }
        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80
            height: 48
            width: 48

            opacity: 0.6
            source: delegateVideo.playbackState === Multimedia.MediaPlayer.PlayingState ? "qrc:/navigation/pause.png" :
                                                                                          "qrc:/navigation/play.png"
            Rectangle {
                z: -1
                anchors.centerIn: parent
                width: 64
                height: 64
                color: "#44000000"
                radius: 2
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (delegateVideo.playbackState === Multimedia.MediaPlayer.PlayingState)
                            delegateVideo.pause()
                        else
                            delegateVideo.play()
                    }
                }
            }
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onClicked: {
                if (mouse.button == Qt.LeftButton) {
                    videoWrapper.visible = false
                    root.visible = false
                }
            }
        }
    }

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
                if (wheel.modifiers & Qt.ShiftModifier) {
                    delegateImage.rotation += wheel.angleDelta.y / 12
                }
                else {
                    delegateImage.scale += wheel.angleDelta.y * 0.002
                }
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
                    root.visible = false
                }
                else if (mouse.button == Qt.MiddleButton) {
                    if (mouse.modifiers & Qt.ShiftModifier) {
                        delegateImage.rotation = (Math.floor((delegateImage.rotation + 90) / 90) * 90) % 360
                    }
                    else {
                        delegateImage.scale = 1.0
                        delegateImage.x = 1
                        delegateImage.y = 1
                        delegateImage.rotation = 0
                    }
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
                root.visible = false
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
