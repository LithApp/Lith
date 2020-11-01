import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Dialog {
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok | Dialog.Cancel

    padding: 0
    topPadding: 0

    SystemPalette {
        id: palette
    }

    header: TabBar {
        id: tabBar
        width: parent.width
        TabButton {
            text: "Connection"
        }
        TabButton {
            text: "Interface"
        }
        TabButton {
            text: "Shortcuts"
            visible: Qt.platform.name !== "ios" && Qt.platform.name !== "android"
        }
    }

    background: Rectangle {
        color: "#eeeeee"
    }

    onAccepted: {
        settingsNetwork.onAccepted()
        settingsInterface.onAccepted()
        settingsShortcuts.onAccepted()
    }
    onRejected: {
        settingsNetwork.onRejected()
        settingsInterface.onRejected()
        settingsShortcuts.onRejected()
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        SettingsNetwork {
            id: settingsNetwork
        }

        SettingsInterface {
            id: settingsInterface
        }

        SettingsShortcuts {
            id: settingsShortcuts
            enabled: Qt.platform.name !== "ios" && Qt.platform.name !== "android"
        }

    }
}
