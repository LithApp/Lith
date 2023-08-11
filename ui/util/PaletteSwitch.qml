import QtQuick.Controls

// To conveniently switch colors from whatever screen I'm working on
Button {
    text: "[]"
    width: height
    onClicked: {
        if (lith.settings.forceLightTheme) {
            lith.settings.forceLightTheme = false
            lith.settings.forceDarkTheme = true
        }
        else if (lith.settings.forceDarkTheme) {
            if (lith.settings.useTrueBlackWithDarkTheme) {
                lith.settings.forceDarkTheme = false
                lith.settings.useTrueBlackWithDarkTheme = false
                lith.settings.forceLightTheme = true
            }
            else {
                lith.settings.useTrueBlackWithDarkTheme = true
            }
        }
        else {
            lith.settings.forceLightTheme = true
        }
    }
}
