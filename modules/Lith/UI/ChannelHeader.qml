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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Lith.Core
import Lith.Style

Rectangle {
    id: root
    color: LithPalette.regular.window

    implicitHeight: totalExpectedHeight
    height: implicitHeight

    // There seems to be no way to query the height of a multiline text in QML so we need to do this.
    // The "+ 1" is a bit of a hack but it works this way so...
    // All this makes the buffer header always fit the name of the buffer and two lines of title text
    // (even if there isn't any - so it doesn't change size when switching)
    readonly property real maximumTitleHeight: titleFontMetrics.height * 2 + 1
    readonly property real maximumNameHeight: nameFontMetrics.height
    readonly property real totalSpacerHeight: topSpacer.implicitHeight + middleSpacer.implicitHeight + bottomSpacer.implicitHeight
    readonly property real totalExpectedHeight: maximumTitleHeight + maximumNameHeight + totalSpacerHeight

    DragHandler {
        onActiveChanged: if (active) window.startSystemMove();
        target: null
    }

    Rectangle {
        anchors {
            left: parent.left
            leftMargin: 1
            right: parent.right
            bottom: parent.bottom
        }
        height: 1
        color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.5)
    }

    RowLayout {
        id: headerLayout
        anchors {
            fill: parent
            margins: 3
        }
        Button {
            id: bufferListButton
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: implicitHeight
            flat: true
            icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/menu.png"
            onClicked: {
                bufferDrawer.toggle()
            }
            ToolTip.text: "Open buffer list,\nAccess configuration"
            ToolTip.visible: bufferListButton.hovered
            ToolTip.delay: 800
        }
        ColumnLayout {
            id: mainColumnLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            Item {
                id: topSpacer
                implicitHeight: 4
                Layout.fillHeight: true
            }
            Label {
                id: bufferNameLabel
                Layout.fillWidth: true
                horizontalAlignment: Label.AlignHCenter
                color: LithPalette.regular.text
                font.bold: true
                text: Lith.selectedBuffer ? Lith.selectedBuffer.name : "Lith"
                elide: Label.ElideRight
                FontMetrics {
                    id: nameFontMetrics
                    font: bufferNameLabel.font
                }
            }
            Item {
                id: middleSpacer
                implicitHeight: 1
                Layout.minimumHeight: implicitHeight
                Layout.maximumHeight: Layout.minimumHeight
                Layout.fillHeight: true
            }
            Label {
                id: titleLabel
                Layout.fillWidth: true
                Layout.alignment: Layout.Top
                color: LithPalette.regular.text
                visible: !Lith.selectedBuffer || Lith.selectedBuffer.title.length > 0
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: Lith.selectedBuffer ? Lith.selectedBuffer.title.toPlain() :
                      Lith.status === Lith.UNCONFIGURED ? "Not configured" :
                      Lith.status === Lith.CONNECTING ? "Connecting" :
                      Lith.status === Lith.CONNECTED ? "Connected" :
                      Lith.status === Lith.DISCONNECTED ? "Disconnected" :
                      Lith.status === Lith.ERROR ? "Error: " + Lith.errorString :
                      Lith.status === Lith.REPLAY ? "Replaying a recording" :
                                                   ""
                elide: Text.ElideRight
                maximumLineCount: 2
                font.pointSize: FontSizes.tiny
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                onLinkActivated: {
                    linkHandler.show(link, root)
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
                FontMetrics {
                    id: titleFontMetrics
                    font: titleLabel.font
                }
            }
            Item {
                id: bottomSpacer
                Layout.fillHeight: true
                implicitHeight: 4
            }
        }
        Button {
            id: nickListButton
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: height
            flat: true
            onClicked: {
                nickDrawer.visible = !nickDrawer.visible
                if(!LithPlatform.mobile) nickDrawer.open()
            }
            icon.source: Lith.status === Lith.UNCONFIGURED ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/sleeping.png" :
                         Lith.status === Lith.CONNECTING   ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/transfer.png" :
                         Lith.status === Lith.CONNECTED    ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/smile.png" :
                         Lith.status === Lith.DISCONNECTED ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/no-wifi.png" :
                         Lith.status === Lith.ERROR        ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/sleeping.png" :
                         Lith.status === Lith.REPLAY       ? (Lith.networkProxy.replaying ? "qrc:/navigation/"+WindowHelper.currentThemeName+"/play-color.png" : "qrc:/navigation/"+WindowHelper.currentThemeName+"/pause-color.png") :
                                                             "qrc:/navigation/"+WindowHelper.currentThemeName+"/dizzy.png"
            ToolTip.text: "Open channel information"
            ToolTip.visible: nickListButton.hovered
            ToolTip.delay: 800
        }
    }
}
