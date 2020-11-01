import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0

TextInput {
    id: inputField
    font.family: "Menlo"
    font.pointSize: settings.baseFontSize
    verticalAlignment: TextField.AlignVCenter
    focus: true
    inputMethodHints: Qt.ImhMultiLine

    color: palette.text

    property alias inputFieldAlias: inputField

    onAccepted: {
        if (text.length > 0) {
            lith.selectedBuffer.input(text)
            text = ""
        }
    }

    Connections {
        target: lith
        function onSelectedBufferChanged() {
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
    property int lastCursorPos: 0
    property int matchedNickIndex: 0
    property variant matchedNicks: []

    function autocomplete() {
        // console.log("BEFORE matchedNicks = ", matchedNicks, "LAST=", lastCursorPos, "NOW=", cursorPosition);

        if(matchedNickIndex > matchedNicks.length-1) {
            matchedNickIndex = 0;
        }

        if(matchedNicks.length > 1) {
            if(lastCursorPos == cursorPosition) {
                inputField.text = inputField.text.substring(0,
                                                            inputField.text.length -
                                                            matchedNicks[matchedNickIndex == 0 ? matchedNicks.length-1 : matchedNickIndex - 1].length
                                                            - 2)
                inputField.text += matchedNicks[matchedNickIndex] + ": "

                matchedNickIndex += 1

                lastCursorPos = cursorPosition
                return;
            }
            else {
                // console.warn("ende.")
                matchedNicks = []
                matchedNickIndex = 0
                lastCursorPos = 0
            }
        }

        // TODO: refactor this, I want cursorPosition tabbing still :)
        var i = inputField.text.lastIndexOf(" ");
        var lastWord
        if (i >= 0)
            lastWord = inputField.text.substring(i+1, inputField.text.length).trim().toLocaleLowerCase();
        else {
            i = 0
            lastWord = inputField.text.trim().toLocaleLowerCase()
        }

        var nicks = lith.selectedBuffer.getVisibleNicks()

        for (var y = 0; y < nicks.length; y++) {
            // console.warn("\"" + lastWord + "\" " + nicks[y])
            if (nicks[y].toLocaleLowerCase().startsWith(lastWord) && lastWord !== "") {
                inputField.text = inputField.text.substring(0, i)
                if (i !== 0) {
                    inputField.text += " "
                }
                inputField.text += nicks[y] + ": "
                matchedNicks.push(nicks[y])
            }
        }

        if (matchedNicks.length == 1) {
            matchedNicks = []; // Reset matchedNicks if there's just one match
        }

        lastCursorPos = cursorPosition
    }

    Shortcut {
        sequences: lith.settings.shortcutSearchBuffer
        onActivated: bufferDrawer.open()
    }
    Shortcut {
        sequences: lith.settings.shortcutAutocomplete
        onActivated: autocomplete();
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToNextBuffer
        onActivated: lith.selectedBufferIndex += 1
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToPreviousBuffer
        onActivated: lith.selectedBufferIndex -= 1
    }

    Keys.onPressed: {
        if (event.modifiers & Qt.ControlModifier) {
            if (event.key === Qt.Key_W) {
                var str = inputField.text.replace(/\s+$/, '')
                var lastIndex = str.lastIndexOf(" ");
                inputField.text = inputField.text.substring(0, lastIndex + 1);
                event.accepted = true
            }
        }
        else {
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
