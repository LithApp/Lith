pragma Singleton

import QtQuick

QtObject {
    property var regular: SystemPalette {
    }
    property var disabled: SystemPalette {
        colorGroup: SystemPalette.Disabled
    }
}
