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
//import QtWebView 1.1

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

    property string currentUrl

    function showImage(url) {
        currentUrl = url
        delegateImage.source = url
        imageWrapper.visible = true
        videoWrapper.visible = false
        root.open()
    }
    function showVideo(url) {
        currentUrl = url
        delegateVideo.source = url
        delegateVideo.loops = lith.settings.loopVideosByDefault ? Multimedia.MediaPlayer.Infinite : 1
        delegateVideo.volume = lith.settings.muteVideosByDefault ? 0.0 : 1.0
        videoWrapper.visible = true
        imageWrapper.visible = false
        root.open()
    }
    onVisibleChanged: {
        if (!visible) {
            currentUrl = ""
            delegateVideo.stop()
            delegateVideo.source = ""
            delegateImage.source = ""
        }
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
            loops: Multimedia.MediaPlayer.Infinite
            width: parent.width - 2
            height: parent.height - 2
            anchors.centerIn: parent
        }
        BusyIndicator {
            anchors.centerIn: parent
            visible: delegateVideo.status === Multimedia.Video.Buffering ||
                     delegateVideo.status === Multimedia.Video.Loading
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

        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80
            spacing: 64

            Image {
                id: rewindImage
                Layout.preferredHeight: 40
                Layout.preferredWidth: 40

                opacity: 0.6

                source: delegateVideo.loops === Multimedia.MediaPlayer.Infinite ? "qrc:/navigation/"+currentTheme+"/reload.png" :
                                                                            "qrc:/navigation/"+currentTheme+"/reload-disabled.png"
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
                            if (delegateVideo.loops === Multimedia.MediaPlayer.Infinite)
                                delegateVideo.loops = 1
                            else
                                delegateVideo.loops = Multimedia.MediaPlayer.Infinite
                        }
                    }
                }
            }

            Image {
                Layout.preferredHeight: 40
                Layout.preferredWidth: 40

                opacity: 0.6
                source: delegateVideo.playbackState === Multimedia.MediaPlayer.PlayingState ? "qrc:/navigation/"+currentTheme+"/pause.png" :
                                                                                              "qrc:/navigation/"+currentTheme+"/play.png"
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

            Image {
                Layout.preferredHeight: 40
                Layout.preferredWidth: 40

                opacity: delegateVideo.hasAudio ? 0.6 : 0.0
                source: delegateVideo.volume > 0.0 ? "qrc:/navigation/"+currentTheme+"/volume.png" : "qrc:/navigation/"+currentTheme+"/mute.png"
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
                            if (delegateVideo.volume > 0.0)
                                delegateVideo.volume = 0.0
                            else
                                delegateVideo.volume = 1.0
                        }
                    }
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
            onStatusChanged: {
                errorMsgText.visible = delegateImage.status === Image.Error
            }

            fillMode: Image.PreserveAspectFit
            width: parent.width - 2
            height: parent.height - 2
            Behavior on scale {
                NumberAnimation {
                    duration: 60
                }
            }

            BusyIndicator {
                anchors.centerIn: parent
                visible: delegateImage.status === Image.Loading
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

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 96

        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 0.6; color: "#aa000000" }
        }

        ColumnLayout {
            anchors {
                bottom: parent.bottom
                bottomMargin: 12
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 12
            Text {
                id: errorMsgText
                visible: delegateImage.status === Image.Error || delegateVideo.errorString.length > 0
                Layout.alignment: Qt.AlignHCenter
                color: "red"
                text: delegateImage.status === Image.Error ? qsTr("The picture could not be displayed") : delegateVideo.errorString
            }
            RowLayout {
                spacing: 24
                Layout.alignment: Qt.AlignHCenter
                Button {
                    focusPolicy: Qt.NoFocus
                    font.pointSize: settings.baseFontSize
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: height
                    icon.source: "qrc:/navigation/"+currentTheme+"/copy.png"
                    onClicked: {
                        clipboardProxy.setText(root.currentUrl)
                    }
                }
                Button {
                    focusPolicy: Qt.NoFocus
                    font.pointSize: settings.baseFontSize
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: height
                    onClicked: {
                        Qt.openUrlExternally(root.currentUrl)
                    }
                    icon.source: "qrc:/navigation/"+currentTheme+"/resize.png"
                }
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
