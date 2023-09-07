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

    flags: LithPlatform.ios ? Qt.Window | Qt.MaximizeUsingFullscreenGeometryHint : Qt.Window

    onActiveChanged: {
        if (active && Lith.debugVersion && LithPlatform.desktop && debugWindowLoader.item)
            debugWindowLoader.item.raise()
    }

    Loader {
        id: debugWindowLoader
        active: Lith.debugVersion && LithPlatform.desktop
        source: "qrc:/qt/qml/Lith/UI/util/DebugWindow.qml"
        property bool firstLoadX: true
        property bool firstLoadY: true
        onLoaded: {
            item.x = window.x + window.width
            item.y = window.y
            item.raise()
        }
        Connections {
            target: window
            enabled: debugWindowLoader.item && (debugWindowLoader.firstLoadX || debugWindowLoader.firstLoadY)
            function onXChanged() {
                debugWindowLoader.item.x = window.x + window.width
                debugWindowLoader.firstLoadX = false
            }
            function onYChanged() {
                debugWindowLoader.item.y = window.y
                debugWindowLoader.firstLoadY = false
            }
        }
    }

    MainView { }
}
