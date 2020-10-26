import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0

TextField {
    id: inputField
    font.family: "Menlo"
    font.pointSize: settings.baseFontSize
    verticalAlignment: TextField.AlignVCenter
    focus: true
    inputMethodHints: Qt.ImhMultiLine

    background: Rectangle {
        color: palette.base
    }
    color: palette.text

    property alias inputFieldAlias :inputField

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

    function autocomplete(lastPos = -1) {

        console.log("lastPos = ", lastPos, "currPos", cursorPosition);
        // var index = matchedIndex;

        var i = inputField.text.lastIndexOf(" ");
        var lastWord
        if (i >= 0)
            lastWord = inputField.text.substring(i+1, inputField.text.length).trim().toLocaleLowerCase();
        else {
            i = 0
            lastWord = inputField.text.trim().toLocaleLowerCase()
        }
        var nicks = stuff.selected.getVisibleNicks()
        // var matched_nicks = [];

        for (var y = 0; y < nicks.length; y++) {
            console.warn("\"" + lastWord + "\" " + nicks[y])
            if (nicks[y].toLocaleLowerCase().startsWith(lastWord) && lastWord !== "") {
                inputField.text = inputField.text.substring(0, i)
                if (i !== 0) {
                    inputField.text += " "
                }
                inputField.text += nicks[y] + ": "
                return;
                // matchedNicks.push(nicks[y]);
            }
        }
        console.log("lastPos = ", lastPos, "currPos", cursorPosition);

        //return cursorPosition;
        //return [cursorPosition, matchedNicks, matchIndex]
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
        else if (event.modifiers & Qt.ControlModifier) {
            if (event.key === Qt.Key_W) {
                var str = inputField.text.replace(/\s+$/, '')
                var lastIndex = str.lastIndexOf(" ");
                inputField.text = inputField.text.substring(0, lastIndex + 1);
                event.accepted = true
            }
        }
        else {
            if (event.key === Qt.Key_Tab) {
                autocomplete();
                // var last = autocomplete();
                event.accepted = true
            }
            if (event.key === Qt.Key_Up) {
                channelMessageList.contentY += 30
            }
            if (event.key === Qt.Key_Down) {
                if (channelMessageList.contentY > 30)
                    channelMessageList.contentY -= 30
                else
                    channelMessageList.contentY = 0
            }
            if (event.key === Qt.Key_PageUp) {
                channelMessageList.contentY += channelMessageList.height - 30
            }
            if (event.key === Qt.Key_PageDown) {
                if (channelMessageList.contentY > channelMessageList.height)
                    channelMessageList.contentY -= channelMessageList.height - 30
                else
                    channelMessageList.contentY = 0
            }
            if (event.key === Qt.Key_End) {
                channelMessageList.contentY = 0
            }
        }
    }
}
