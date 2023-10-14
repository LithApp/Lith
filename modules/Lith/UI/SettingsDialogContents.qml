import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core

Item {
    id: root
    implicitWidth: Math.max(Math.max(tabBar.implicitWidth, stackLayout.implicitWidth), dialogButtons.implicitWidth)
    implicitHeight: tabBar.implicitHeight + stackLayout.implicitHeight + dialogButtons.implicitHeight

    function save() {
        settingsNetwork.save()
        settingsInterface.save()
        settingsShortcuts.save()
    }
    function restore() {
        settingsNetwork.restore()
        settingsInterface.restore()
        settingsShortcuts.restore()
    }

    signal closeRequested

    TabBar {
        id: tabBar

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        Repeater {
            model: LithPlatform.mobile ? [qsTr("Connection"), qsTr("Interface")]
                                          : [qsTr("Connection"), qsTr("Interface"), qsTr("Shortcuts")]

            delegate: TabButton {
                text: modelData
            }
        }
    }

    StackLayout {
        id: stackLayout

        anchors {
            top: tabBar.bottom
            left: parent.left
            right: parent.right
            bottom: dialogButtons.top
        }

        currentIndex: tabBar.currentIndex

        SettingsNetwork {
            id: settingsNetwork
        }

        SettingsInterface {
            id: settingsInterface
        }

        SettingsShortcuts {
            id: settingsShortcuts
            enabled: !LithPlatform.mobile
        }
    }

    DialogButtons {
        id: dialogButtons

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        acceptText: qsTr("Save and close")
        additionalButton: Button {
            text: qsTr("Save")
            onClicked: root.save()
        }

        onAccepted: () => {
            root.save()
            root.closeRequested()
        }
        onRejected: () => {
            root.restore()
            root.closeRequested()
        }
    }
}
