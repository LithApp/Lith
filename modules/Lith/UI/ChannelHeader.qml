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

import "util" as Util

Item {
    id: root

    implicitHeight: totalExpectedHeight + Lith.settings.uiMargins * 2
    height: implicitHeight

    // There seems to be no way to query the height of a multiline text in QML so we need to do this.
    // The "+ 1" is a bit of a hack but it works this way so...
    // All this makes the buffer header always fit the name of the buffer and two lines of title text
    // (even if there isn't any - so it doesn't change size when switching)
    readonly property real maximumTitleHeight: titleTextMetrics.height * 2 + 1
    readonly property real maximumNameHeight: nameFontMetrics.height
    readonly property real totalSpacerHeight: topSpacer.implicitHeight + middleSpacer.implicitHeight + bottomSpacer.implicitHeight
    readonly property real totalTextContentsHeight: maximumTitleHeight + maximumNameHeight + totalSpacerHeight
    readonly property real buttonWithMarginsHeight: bufferListButton.implicitHeight + headerLayout.anchors.margins * 2
    readonly property real totalExpectedHeight: Math.max(totalTextContentsHeight, buttonWithMarginsHeight)
    readonly property real implicitContentHeight: headerLayout.height

    DragHandler {
        onActiveChanged: if (active) window.startSystemMove();
        target: null
    }

    Util.ControlPanel {
        anchors.fill: parent
        anchors.leftMargin: Lith.settings.uiMargins
        anchors.topMargin: Lith.settings.uiMargins
        anchors.rightMargin: anchors.leftMargin
        anchors.bottomMargin: anchors.topMargin / 2
        radius: Math.pow(Lith.settings.uiMargins, 0.9)
    }

    RowLayout {
        id: headerLayout
        anchors {
            fill: parent
            leftMargin: Lith.settings.uiMargins * 2 + 2
            topMargin: Lith.settings.uiMargins
            rightMargin: anchors.leftMargin + 1
            bottomMargin: anchors.topMargin / 2
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
                textFormat: Label.RichText
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
                text: Lith.selectedBuffer ? Lith.selectedBuffer.title.toPlain() :
                      Lith.status === Lith.UNCONFIGURED ? "Not configured" :
                      Lith.status === Lith.CONNECTING ? "Connecting" :
                      Lith.status === Lith.CONNECTED ? "Connected" :
                      Lith.status === Lith.DISCONNECTED ? "Disconnected" :
                      Lith.status === Lith.ERROR ? "Error: " + Lith.errorString :
                      Lith.status === Lith.REPLAY ? "Replaying a recording" :
                                                   ""
                textFormat: Label.PlainText
                elide: Label.ElideRight
                wrapMode: Label.Wrap
                maximumLineCount: 2
                font.pixelSize: FontSizes.tiny
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
                TextMetrics {
                    id: titleTextMetrics
                    font: titleLabel.font
                    text: "Ay🐱"
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
