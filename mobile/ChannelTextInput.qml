import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0

TextField {
    id: inputField
    font.family: "Menlo"
    font.pointSize: 16
    verticalAlignment: TextField.AlignVCenter
    focus: true

    background: Rectangle {
        color: palette.base
    }
    color: palette.text

    onAccepted: {
        if (text.length > 0) {
            stuff.selected.input(text)
            text = ""
        }
    }

    Connections {
        target: stuff
        function onSelectedChanged() {
            inputField.focus = true
        }
    }

    Action {
        id: pasteAction
        text: "&Paste"
        shortcut: StandardKey.Paste
        //enabled: inputField.activeFocus
        onTriggered: {
            if (clipboard.hasImage) {
                uploader.uploadBinary(clipboard.image())
            }
        }
    }

    Shortcut {
        sequence: StandardKey.Paste
        onActivated: {
             console.warn("HA")
        }
    }

    function autocomplete() {
        var i = inputField.text.indexOf(" ");
        var lastWord
        if (i >= 0)
            lastWord = inputField.text.substring(i+1,-1).trim().toLocaleLowerCase();
        else {
            i = 0
            lastWord = inputField.text.trim().toLocaleLowerCase()
        }
        var nicks = stuff.selected.getVisibleNicks()
        for (var i = 0; i < nicks.length; i++) {
            console.warn("\"" + lastWord + "\" " + nicks[i])
            if (nicks[i].toLocaleLowerCase().startsWith(lastWord)) {
                inputField.text = inputField.text.substring(0, i)
                inputField.text += nicks[i]
                return
            }
        }
    }

    Keys.onPressed: {
        if (event.modifiers & Qt.AltModifier) {
            if (event.key === Qt.Key_Right || event.key === Qt.Key_Down) {
                stuff.selectedIndex += 1
                event.accepted = true
            }
            else if (event.key === Qt.Key_Left || event.key === Qt.Key_Up) {
                stuff.selectedIndex -= 1
                event.accepted = true
            }
            else if (event.key === Qt.Key_G) {
                bufferDrawer.open()
                event.accepted = true
            }
        }
        if (event.modifiers & Qt.ControlModifier) {
            if (event.key === Qt.Key_W) {
                var str = inputField.text.replace(/\s+$/, '')
                var lastIndex = str.lastIndexOf(" ");
                inputField.text = inputField.text.substring(0, lastIndex + 1);
                event.accepted = true
            }
        }
        if (event.key === Qt.Key_Tab) {
            autocomplete()
            event.accepted = true
        }
    }
}
