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

Rectangle {
    clip: true
    height: errorText.text.length > 0 ? 40 : 0
    width: parent.width
    //visible: height != 0
    gradient: Gradient {
        GradientStop { position: 0.0; color: "dark red" }
        GradientStop { position: 0.22; color: "red" }
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
        Text {
            Layout.fillHeight: true
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            id: errorText
            text: lith.errorString
            elide: Text.ElideRight
        }
        Button {
            icon.source: "qrc:/navigation/"+currentTheme+"/close.png"
            Layout.preferredWidth: 28
            Layout.preferredHeight: 36
            onClicked: lith.errorString = ""
        }
    }
}
