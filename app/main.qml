// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Jakub Mach
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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// This has to be here otherwise the plugin won't get linked
import Lith.Core
import Lith.Style
import Lith.UI

ApplicationWindow {
    id: window
    visible: true
    width: LithPlatform.mobile ? 480 : 1024
    height: 800
    title: "Lith"

    // flags: LithPlatform.ios ? (Qt.Window | Qt.FramelessWindowHint | Qt.ExpandedClientAreaHint | Qt.NoTitleBarBackgroundHint) : Qt.Window
    // flags:  Qt.Window


    leftPadding: SafeArea.margins.left
    rightPadding: SafeArea.margins.right
    topPadding: SafeArea.margins.top
    bottomPadding: SafeArea.margins.bottom //+ (height - Qt.inputMethod.keyboardRectangle.x)

    flags: Qt.Window | Qt.ExpandedClientAreaHint
    color: "yellow"

    Connections {
       target: Qt.inputMethod
       function onVisibleChanged() {
           window.foo()
           return
           console.warn("HOVNO %1 %2 %3 %4 %5".arg(Qt.inputMethod.visible).arg(window.SafeArea.margins.top).arg(window.SafeArea.margins.bottom)
                                     .arg(window.SafeArea.additionalMargins.top).arg(window.SafeArea.additionalMargins.bottom)
                        )
       }
       function onKeyboardRectangleChanged() {
           console.warn("InputMethod: %1".arg(window.aaa(Qt.inputMethod.keyboardRectangle)))
           console.warn("Clipped InputMethod: %1".arg(window.aaa(Qt.inputMethod.inputItemClipRectangle)))
           console.warn("Window: %1".arg(window.aaa(window)))
           console.warn("Window SCALED: %1".arg(aaa(window, Screen.devicePixelRatio)))

       }

       function onInputItemClipRectangleChanged() {
           console.warn("InputMethod: %1".arg(window.aaa(Qt.inputMethod.inputItemClipRectangle)))
           console.warn("Window: %1".arg(window.aaa(window)))

       }
    }

    function aaa(i, ratio = 1) {
        if (!i)
            return "N/A: %1".arg(i)
        return "%1 %2 %3 %4".arg(i.x * ratio).arg(i.y  * ratio).arg(i.width * ratio).arg(i.height * ratio)
    }

    function foo() {
        console.warn("PIXEL RATIO %1".arg(Screen.devicePixelRatio))
        console.warn("Window: %1".arg(aaa(window)))
        console.warn("Background: %1".arg(aaa(window.background)))
        console.warn("mainViewItem: %1".arg(aaa(mainViewItem)))
        console.warn("Window SCALED: %1".arg(aaa(window, Screen.devicePixelRatio)))
        console.warn("CONTENTITEM SCALED: %1".arg(aaa(window.contentItem, Screen.devicePixelRatio)))
        console.warn("InputMethod: %1".arg(aaa(Qt.inputMethod.keyboardRectangle)))
    }

    MainView {
        id: mainViewItem
        anchors.fill: parent
        anchors.topMargin: mainViewItem.topOverlap//window.bottomPadding
        anchors.bottomMargin: mainViewItem.bottomOverlap//window.bottomPadding
        onHeightChanged: {
            window.foo()
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 100

            spacing: 5

            Text {
                z: 10
                text: "Window\n%1".arg(window.aaa(window))
                color: "darkgreen"
                Rectangle {
                    z: -1
                    anchors.fill: parent
                    anchors.margins: -2
                }
            }
            Text {
                z: 10
                text: "Cover\n%1".arg(window.aaa(cover))
                color: "red"
                Rectangle {
                    z: -1
                    anchors.fill: parent
                    anchors.margins: -2
                }
            }
            Text {
                z: 10
                text: "Overlap\n%1 %2".arg(mainViewItem.topOverlap).arg(mainViewItem.bottomOverlap)
                color: "black"
                Rectangle {
                    z: -1
                    anchors.fill: parent
                    anchors.margins: -2
                }
            }
            Text {
                z: 10
                text: "Safe\n%1".arg("%1 %2 %3 %4".arg(window.leftPadding)
                                                  .arg(window.rightPadding)
                                                  .arg(window.topPadding)
                                                  .arg(window.bottomPadding)
                                     )
                color: "black"
                Rectangle {
                    id: fu
                    z: -1
                    anchors.fill: parent
                    anchors.margins: -2
                }
            }
        }

        Rectangle {
            id: cover
            z: 10
            color: "lightblue"
            property real scaleFactor: Screen.devicePixelRatio
            property real overlap: {
                if (!Qt.inputMethod.visible)
                    return 0
                if ((Qt.inputMethod.keyboardRectangle.y / Screen.devicePixelRatio) - window.topPadding > window.height)
                    return 0
                return window.height - (Qt.inputMethod.keyboardRectangle.y / Screen.devicePixelRatio) - window.topPadding
            }
            x: Qt.inputMethod.keyboardRectangle.x / scaleFactor - window.leftPadding
            y: Qt.inputMethod.keyboardRectangle.y / scaleFactor - window.topPadding
            width: Qt.inputMethod.keyboardRectangle.width / scaleFactor
            height: Qt.inputMethod.keyboardRectangle.height / scaleFactor
            Rectangle {
                anchors.fill: parent
                anchors.margins: -3
                z: -1
                color: "green"
            }
        }

        /*
        Rectangle {
            color: "dark blue"
            property real offset: 3
            x: Qt.inputMethod.inputItemClipRectangle.x - offset
            y: Qt.inputMethod.inputItemClipRectangle.y - offset - window.topPadding
            width: Qt.inputMethod.inputItemClipRectangle.width + 2 * offset
            height: Qt.inputMethod.inputItemClipRectangle.height + 2 * offset
        }
        */
    }
}
