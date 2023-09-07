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
    clip: true
    height: errorText.text.length > 0 ? 46 : 0
    width: parent.width
    //visible: height != 0
    gradient: Gradient {
        GradientStop { position: 0.0; color: WindowHelper.darkTheme ? "#881111" : "#bb2222" }
        GradientStop { position: 0.22; color: WindowHelper.darkTheme ? "#aa1111" : "#dd3333" }
    }
    Behavior on height {
        NumberAnimation {
            duration: 120
            easing.type: Easing.InOutQuart
        }
    }
    RowLayout {
        anchors {
            left: parent.left
            leftMargin: 12
            right: parent.right
            rightMargin: 12
            verticalCenter: parent.verticalCenter
        }
        spacing: 18
        Label {
            id: errorText
            Layout.fillHeight: true
            Layout.fillWidth: true
            verticalAlignment: Label.AlignVCenter
            text: Lith.errorString
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
            maximumLineCount: 2
            elide: Text.ElideRight
            color: LithPalette.regular.text
        }
        Button {
            icon.source: "qrc:/navigation/"+WindowHelper.currentThemeName+"/close.png"
            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
            onClicked: Lith.errorString = ""
        }
    }
}
