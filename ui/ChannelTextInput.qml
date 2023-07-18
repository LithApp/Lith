// Lith
// Copyright (C) 2020 Martin Bříza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import lith 1.0

TextField {
    id: inputField
    verticalAlignment: TextField.AlignVCenter
    inputMethodHints: Qt.ImhMultiLine
    renderType: TextInput.NativeRendering

    borderColor: backgroundColor
    color: palette.text


    // Second part of a hack to show more lines of text when writing a long message
    topPadding: 0
    bottomPadding: contentHeight > font.pixelSize * 2 ? 6 : 0 // if it's twice the size of the font - add a margin
    wrapMode: TextField.WordWrap

    Connections {
        target: Qt.inputMethod
    }

    onAccepted: {
        if (text.length > 0) {
            if (lith.selectedBuffer.input(text)) {
                text = ""
            }
        }
    }

    property var previousBuffer: null
    Connections {
        target: lith
        function onSelectedBufferChanged() {
            inputField.forceActiveFocus()

            if (lith.settings.useLocalInputBar) {
                if (previousBuffer)
                    previousBuffer.lastUserInput = inputField.text
                if (lith.selectedBuffer)
                    inputField.text = lith.selectedBuffer.lastUserInput
                else
                    inputField.text = ""
                previousBuffer = lith.selectedBuffer
            }
        }
    }

    property int lastCursorPos: 0
    property int matchedNickIndex: 0
    property variant matchedNicks: []
    property bool cursorWasAtStart: false // This is simpler than checking for ": " everytime, could be replaced though if you don't like it

    function pasteText(pasted) {
        var left = text.slice(0, selectionStart)
        //var mid = text.slice(selectionStart, selectionEnd)
        var right = text.slice(selectionEnd)
        text = left + pasted + right
        cursorPosition = left.length + pasted.length
    }

    function autocomplete() {
        // console.log("BEFORE matchedNicks = ", matchedNicks, "LAST=", lastCursorPos, "NOW=", cursorPosition);

        if(matchedNickIndex > matchedNicks.length-1) {
            matchedNickIndex = 0;
        }

        if(matchedNicks.length > 1) {
            if(lastCursorPos == cursorPosition) {
                var prevNick = matchedNicks[matchedNickIndex == 0 ? matchedNicks.length - 1 : matchedNickIndex - 1]
                var tmp_msg_str_before_nick = inputField.text.substring(0, cursorPosition - prevNick.length - 1 - (cursorWasAtStart ? 2 : 0))
                var tmp_msg_str_after_nick = inputField.text.substring(cursorPosition, inputField.text.length)

                inputField.text = "" // reset field's text
                inputField.text += tmp_msg_str_before_nick // add all the text before the tabbed nickname
                inputField.text += matchedNicks[matchedNickIndex] + (cursorWasAtStart ? ": " : " ")   // add the tabbed nickname
                lastCursorPos = cursorPosition // save the current position, it's where we should put the cursor back up
                inputField.text += tmp_msg_str_after_nick // add the rest of the message after the tabbed nickname

                // put the saved cursor position back in place, so we don't end up having the cursor after the appended text
                cursorPosition = lastCursorPos
                matchedNickIndex += 1

                return;
            }
            else {
                // console.warn("ende.")
                matchedNicks = []
                matchedNickIndex = 0
                lastCursorPos = 0
                cursorWasAtStart = false
            }
        }

        var i = -1; // This is the starting position if the message is empty

        // Like indexOf, but let's go backwards from the current curpos
        for(var x = cursorPosition - 1; x >= 0; x--) // TODO: Accurate?
        {
            if(inputField.text.charAt(x) == " ")
            {
                i = x
                break;
            }
        }

        var lastWord
        if (i >= 0) {
            lastWord = inputField.text.substring(i+1, cursorPosition).trim().toLocaleLowerCase()
        }
        else {
            i = 0
            lastWord = inputField.text.substring(i, cursorPosition).trim().toLocaleLowerCase()
        }
        var nicks = lith.selectedBuffer.getVisibleNicks()

        for (var y = 0; y < nicks.length; y++) {
            if (nicks[y].toLocaleLowerCase().startsWith(lastWord) && lastWord !== "") {
                if(matchedNicks.length < 1) // We only want to add the first nick to the message for now
                {
                    var tmp_orig = inputField.text.substring(0, i)
                    var tmp_to_add = ""; // Since we sometimes have ": " and " ", let's store it seperately for the cursorPosition hack

                    if (i !== 0) {
                        tmp_to_add += " "
                        tmp_to_add += nicks[y] + " "
                        cursorWasAtStart = false
                    }
                    else {
                        tmp_to_add += nicks[y] + ": "
                        cursorWasAtStart = true // this is just easier than grabbing ": " again
                    }

                    // Add the text before the tabbed "lastWord", then add the nickname and finally add the text that was there after the tabbed "lastWord"
                    inputField.text = tmp_orig + tmp_to_add + inputField.text.substring(i + lastWord.length + (cursorWasAtStart ? 0 : 1), inputField.text.length)

                    // Hack back the cursorPosition since we used inputField.text = which messed it up to the end
                    cursorPosition = i + tmp_to_add.length
                }

                matchedNicks.push(nicks[y]) // But add all of them to the list to be used when Tab is used again
            }
        }

        if (matchedNicks.length == 1) {
            matchedNicks = [] // Reset matchedNicks if there's just one match
        }
        else
        {
            matchedNickIndex += 1 // We need to add + 1 to the index, since we already matched the first one (0) here ^^^^^
        }
        lastCursorPos = cursorPosition
    }

    Connections {
        target: uploader
        function onSuccess(url) {
            if (inputField.length !== 0 && !inputField.text.endsWith(" "))
                inputField.pasteText(" ")
            inputField.pasteText(url + " ")
        }
        function onError(message) {
            console.warn("IMAGE UPLOAD ERROR")
            console.warn(message)
            lith.errorString = message
        }
    }

    Shortcut {
        sequences: lith.settings.shortcutSearchBuffer
        onActivated: bufferDrawer.open()
    }
    Shortcut {
        sequences: lith.settings.shortcutNicklist
        onActivated: nickDrawer.open()
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
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer1
        onActivated: lith.switchToBufferNumber(1)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer2
        onActivated: lith.switchToBufferNumber(2)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer3
        onActivated: lith.switchToBufferNumber(3)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer4
        onActivated: lith.switchToBufferNumber(4)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer5
        onActivated: lith.switchToBufferNumber(5)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer6
        onActivated: lith.switchToBufferNumber(6)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer7
        onActivated: lith.switchToBufferNumber(7)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer8
        onActivated: lith.switchToBufferNumber(8)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer9
        onActivated: lith.switchToBufferNumber(9)
    }
    Shortcut {
        sequences: lith.settings.shortcutSwitchToBuffer10
        onActivated: lith.switchToBufferNumber(10)
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+W"
        onActivated: {
            var str = inputField.text.replace(/\s+$/, '')
            var lastIndex = str.lastIndexOf(" ");
            inputField.text = inputField.text.substring(0, lastIndex + 1);
        }
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+D"
        onActivated: {
            var str = inputField.text
            str = str.slice(0, inputField.cursorPosition - 1) + str.slice(inputField.cursorPosition);
            inputField.text = str
        }
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+K"
        onActivated: {
            var right = inputField.text.slice(inputField.cursorPosition)
            inputField.text = inputField.text.slice(0, inputField.cursorPosition - 1)
            clipboardProxy.setText(right)
        }
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+U"
        onActivated: {
            var left = inputField.text.slice(0, inputField.cursorPosition - 1)
            inputField.text = inputField.text.slice(inputField.cursorPosition)
            clipboardProxy.setText(left)
        }
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+Y"
        onActivated: {
            if (clipboardProxy.text.length > 0)
                inputField.text = clipboardProxy.text()
        }
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+F"
        onActivated: inputField.cursorPosition++
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+B"
        onActivated: inputField.cursorPosition--
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+A"
        onActivated: inputField.cursorPosition = 0
    }
    Shortcut {
        enabled: lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+E"
        onActivated: inputField.cursorPosition = inputField.text.length
    }
    Keys.onPressed: (event) => {
        if (event.matches(StandardKey.Paste)) {
            if (clipboardProxy.hasImage()) {
                uploader.uploadBinary(clipboardProxy.image())
            }
            else {
                pasteText(clipboardProxy.text())
            }
            event.accepted = true
        }

        if (event.key === Qt.Key_Up) {
            channelMessageList.contentY -= 30
        }
        if (event.key === Qt.Key_Down) {
            if (channelMessageList.height + channelMessageList.contentY > -25)
                channelMessageList.positionViewAtBeginning()
            else
                channelMessageList.contentY += 30
        }
        if (event.key === Qt.Key_PageUp) {
            channelMessageList.contentY -= channelMessageList.height - 30
        }
        if (event.key === Qt.Key_PageDown) {
            if (channelMessageList.height + channelMessageList.contentY > -(channelMessageList.height - 35))
                channelMessageList.positionViewAtBeginning()
            else
                channelMessageList.contentY += channelMessageList.height - 30
        }
        if (event.key === Qt.Key_End) {
            channelMessageList.contentY = 0
        }
    }
}
