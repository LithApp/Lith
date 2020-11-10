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
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0

Frame {
    background: Rectangle {
        color: palette.window
    }

    RowLayout {
        width: parent.width
        Button {
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: height
            font.pointSize: settings.baseFontSize * 1.25
            text: "☰"
            onClicked: bufferDrawer.visible = !bufferDrawer.visible
        }
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                height: 1
                font.bold: true
                font.pointSize: lith.selectedBuffer && lith.selectedBuffer.title.length > 0 ? settings.baseFontSize * 0.875 :
                                                                                    settings.baseFontSize * 1.125
                color: palette.windowText
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: lith.selectedBuffer ? lith.selectedBuffer.name : ""
                renderType: Text.NativeRendering
            }
            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: lith.selectedBuffer && lith.selectedBuffer.title.length > 0
                clip: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: lith.selectedBuffer ? lith.selectedBuffer.title : ""
                elide: Text.ElideRight
                maximumLineCount: 2
                font.pointSize: settings.baseFontSize * 0.75
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                renderType: Text.NativeRendering
                color: palette.windowText
            }
        }
        Item { width: 1 }
        Button {
            focusPolicy: Qt.NoFocus
            Layout.preferredWidth: height
            font.pointSize: settings.baseFontSize * 1.25
            visible: lith.status !== Lith.UNCONFIGURED
            enabled: lith.status === Lith.CONNECTED
            text: lith.status === Lith.CONNECTING   ? "🤔" :
                  lith.status === Lith.CONNECTED    ? "🙂" :
                  lith.status === Lith.DISCONNECTED ? "😴" :
                  lith.status === Lith.ERROR        ? "☠" :
                                                      "😱"
            onClicked: nickDrawer.visible = !nickDrawer.visible
        }
    }
}
