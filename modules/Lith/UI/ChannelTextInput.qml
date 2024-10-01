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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Lith.Core
import Lith.Style

TextField {
    id: inputField
    verticalAlignment: TextField.AlignVCenter
    inputMethodHints: Qt.ImhMultiLine

    borderColor: backgroundColor
    color: LithPalette.regular.text


    // Second part of a hack to show more lines of text when writing a long message
    topPadding: 0
    bottomPadding: contentHeight > font.pixelSize * 2 ? 6 : 0 // if it's twice the size of the font - add a margin
    wrapMode: TextField.WordWrap

    Connections {
        target: Qt.inputMethod
    }

    onAccepted: {
        inputField.input()
    }

    function input() {
        // Save a copy first so the field gets cleared before sending because the call can block (Reported as #169)
        const message = inputField.text
        if (message.length > 0) {
            inputField.text = ""
            if (!Lith.selectedBuffer.userInput(message)) {
                // Here we could return the message that could not be sent
                // but some people (AHOJ POLIVKO) type so fast it's less annoying to not do that without asking
            }
        }
    }

    property var previousBuffer: null
    Connections {
        target: Lith
        function onSelectedBufferChanged() {
            inputField.forceActiveFocus()

            if (Lith.settings.useLocalInputBar) {
                if (previousBuffer)
                    previousBuffer.lastUserInput = inputField.text
                if (Lith.selectedBuffer)
                    inputField.text = Lith.selectedBuffer.lastUserInput
                else
                    inputField.text = ""
                previousBuffer = Lith.selectedBuffer
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
        var nicks = Lith.selectedBuffer.getVisibleNicks()

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
        target: Uploader
        function onSuccess(url) {
            if (inputField.length !== 0 && !inputField.text.endsWith(" "))
                inputField.pasteText(" ")
            inputField.pasteText(url + " ")
        }
        function onError(message) {
            console.warn("IMAGE UPLOAD ERROR")
            console.warn(message)
            Lith.errorString = message
        }
    }

    Shortcut {
        sequences: Lith.settings.shortcutSearchBuffer
        onActivated: {
            bufferDrawer.open()
            bufferDrawer.enforceFocus()
        }
    }
    Shortcut {
        sequences: Lith.settings.shortcutNicklist
        onActivated: nickDrawer.open()
    }
    Shortcut {
        sequences: Lith.settings.shortcutAutocomplete
        onActivated: autocomplete();
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToNextBuffer
        onActivated: Lith.selectedBufferIndex += 1
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToPreviousBuffer
        onActivated: Lith.selectedBufferIndex -= 1
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer1
        onActivated: Lith.switchToBufferNumber(1)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer2
        onActivated: Lith.switchToBufferNumber(2)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer3
        onActivated: Lith.switchToBufferNumber(3)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer4
        onActivated: Lith.switchToBufferNumber(4)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer5
        onActivated: Lith.switchToBufferNumber(5)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer6
        onActivated: Lith.switchToBufferNumber(6)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer7
        onActivated: Lith.switchToBufferNumber(7)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer8
        onActivated: Lith.switchToBufferNumber(8)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer9
        onActivated: Lith.switchToBufferNumber(9)
    }
    Shortcut {
        sequences: Lith.settings.shortcutSwitchToBuffer10
        onActivated: Lith.switchToBufferNumber(10)
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+W"
        onActivated: {
            var str = inputField.text.replace(/\s+$/, '')
            var lastIndex = str.lastIndexOf(" ");
            inputField.text = inputField.text.substring(0, lastIndex + 1);
        }
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+D"
        onActivated: {
            var str = inputField.text
            str = str.slice(0, inputField.cursorPosition - 1) + str.slice(inputField.cursorPosition);
            inputField.text = str
        }
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+K"
        onActivated: {
            var right = inputField.text.slice(inputField.cursorPosition)
            inputField.text = inputField.text.slice(0, inputField.cursorPosition - 1)
            ClipboardProxy.setText(right)
        }
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+U"
        onActivated: {
            var left = inputField.text.slice(0, inputField.cursorPosition - 1)
            inputField.text = inputField.text.slice(inputField.cursorPosition)
            ClipboardProxy.setText(left)
        }
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+Y"
        onActivated: {
            if (ClipboardProxy.text.length > 0)
                inputField.text = ClipboardProxy.text()
        }
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+F"
        onActivated: inputField.cursorPosition++
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+B"
        onActivated: inputField.cursorPosition--
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+A"
        onActivated: inputField.cursorPosition = 0
    }
    Shortcut {
        enabled: Lith.settings.enableReadlineShortcuts
        sequence: "Ctrl+E"
        onActivated: inputField.cursorPosition = inputField.text.length
    }
    Keys.onPressed: (event) => {
        if (event.matches(StandardKey.Paste)) {
            if (ClipboardProxy.hasImage()) {
                Uploader.uploadBinary(ClipboardProxy.image())
            }
            else {
                pasteText(ClipboardProxy.text())
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
