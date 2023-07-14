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

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import "LithStyle"

ApplicationWindow {
    id: window
    visible: true
    width: platform.mobile ? 480 : 1024
    height: 800
    title: "Lith"

    flags: platform.ios ? Qt.Window | Qt.MaximizeUsingFullscreenGeometryHint : Qt.Window

    property bool landscapeMode: !platform.mobile & width > height
    property string currentTheme: lith.windowHelper.darkTheme ? "dark" : "light"

    property alias platform: platform
    QtObject {
        id: platform
        readonly property bool mobile: ios || android
        readonly property bool desktop: windows || macos || linux || wasm
        readonly property bool unknown: !mobile && !desktop

        readonly property bool ios: Qt.platform.os === "ios"
        readonly property bool android: Qt.platform.os === "android"
        readonly property bool windows: Qt.platform.os === "windows"
        readonly property bool macos: Qt.platform.os === "osx"
        readonly property bool linux: Qt.platform.os === "linux"
        readonly property bool wasm: Qt.platform.os === "wasm"
    }

    // TODO find a more sensible place for this
    QtObject {
        id: colorUtils
        function mixColors(a, b, ratio) {
            var c = Qt.color(a)
            c.r = a.r * ratio + b.r * (1 - ratio)
            c.g = a.g * ratio + b.g * (1 - ratio)
            c.b = a.b * ratio + b.b * (1 - ratio)
            c.a = a.a * ratio + b.a * (1 - ratio)
            return c
        }
        function setAlpha(c, alpha) {
            var modified = Qt.color(c)
            modified.a = alpha
            return modified
        }
        function darken(c, ratio) {
            var modified = Qt.color(c)
            modified.hslLightness = Math.min(Math.max(modified.hslLightness / ratio, 0.0), 1.0)
            return modified
        }
        function lighten(c, ratio) {
            var modified = Qt.color(c)
            modified.hslLightness = Math.min(Math.max(modified.hslLightness * ratio, 0.0), 1.0)
            return modified
        }
    }

    SystemPalette {
        id: palette
    }
    SystemPalette {
        id: disabledPalette
        colorGroup: SystemPalette.Disabled
    }

    Rectangle {
        id: windowBackground
        anchors.fill: parent
        color: palette.base
    }

    MainView { }
}
