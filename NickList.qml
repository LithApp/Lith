import QtQuick 2.0

import "Default"

ListView {
    id: root

    ListModel {
        id: fakeNickListModel
        ListElement { name: "me"; visible: false; level: 0; group: true }
        ListElement { name: "NickList"; visible: true; color: "red"; prefix: "@"; level: 1; group: false }
        ListElement { name: "Is"; visible: true; color: "blue"; level: 1; group: false }
        ListElement { name: "Not"; visible: true; color: "yellow"; level: 1; group: false }
        ListElement { name: "Implemented"; visible: true; color: "green"; level: 1; group: false }
        ListElement { name: "Yet"; visible: true; color: "orange"; level: 1; group: false }
    }
    //model: fakeNickListModel
    delegate: NickListItem { }

    property int lastWidth: implicitWidth
    property bool open: true
    onOpenChanged: {
        if (open) {
            nickListShowAnimation.start()
        }
        else {
            if (!nickListShowAnimation.running)
                root.lastWidth = root.width
            nickListHideAnimation.start()
        }
    }

    NumberAnimation on width {
        running: false
        id: nickListHideAnimation
        to: 0
        onStarted: root.lastWidth = root.width
        easing.type: Easing.OutCubic
    }
    NumberAnimation on width {
        running: false
        id: nickListShowAnimation
        to: root.lastWidth
        easing.type: Easing.InCubic
    }
}
