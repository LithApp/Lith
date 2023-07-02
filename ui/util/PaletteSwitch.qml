import QtQuick.Controls

// To conveniently switch colors from whatever screen I'm working on
Button {
    text: "[]"
    width: height
    onClicked: {
        if (settings.forceLightTheme) {
            settings.forceLightTheme = false
            settings.forceDarkTheme = true
        }
        else if (settings.forceDarkTheme) {
            if (settings.useTrueBlackWithDarkTheme) {
                settings.forceDarkTheme = false
                settings.useTrueBlackWithDarkTheme = false
                settings.forceLightTheme = true
            }
            else {
                settings.useTrueBlackWithDarkTheme = true
            }
        }
        else {
            settings.forceLightTheme = true
        }
    }
}
