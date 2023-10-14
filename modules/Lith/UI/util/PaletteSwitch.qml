import QtQuick.Controls

// To conveniently switch colors from whatever screen I'm working on
Button {
    text: "[]"
    implicitWidth: implicitHeight
    onClicked: {
        if (Lith.settings.forceLightTheme) {
            Lith.settings.forceLightTheme = false
            Lith.settings.forceDarkTheme = true
        }
        else if (Lith.settings.forceDarkTheme) {
            if (Lith.settings.useTrueBlackWithDarkTheme) {
                Lith.settings.forceDarkTheme = false
                Lith.settings.useTrueBlackWithDarkTheme = false
                Lith.settings.forceLightTheme = true
            }
            else {
                Lith.settings.useTrueBlackWithDarkTheme = true
            }
        }
        else {
            Lith.settings.forceLightTheme = true
        }
    }
}
