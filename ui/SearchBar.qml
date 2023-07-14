import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import lith

// Had to use a wrapper Item to enforce parent layout boundaries... shouldn't be necessary, really, but I probably made a mistake in some higher level
Item {
    id: root

    property var search: lith.search
    property int currentIndex: -1
    property int currentMessageIndex: currentIndex >= 0 && currentIndex < search.matches.length ? search.matches[currentIndex] : -1

    onVisibleChanged: {
        if (visible) {
            termField.forceActiveFocus()
        }
        else {
            search.term = ""
        }
    }

    implicitHeight: rootLayout.implicitHeight + 1
    implicitWidth: rootLayout.implicitWidth

    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 1
        color: colorUtils.mixColors(palette.text, palette.window, 0.3)
    }

    RowLayout {
        id: rootLayout
        y: 1
        x: 5
        width: parent.width - 2 * x
        spacing: 6
        Button {
            id: searchModeButton
            // This is a small hack to make this button appear in the same place as its input bar counterpart
            Layout.preferredHeight: implicitHeight - 1
            icon.source: "qrc:/navigation/"+currentTheme+"/loupe.png"
            focusPolicy: Qt.NoFocus
            flat: true
            onClicked: root.visible = false
            ToolTip.text: "Exit search mode"
            ToolTip.visible: searchModeButton.hovered
            ToolTip.delay: 800
            // Looks meh
            IconImage {
                mipmap: true
                width: parent.width / 3.5
                height: width
                source: "qrc:/navigation/"+currentTheme+"/close.png"
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: height / 3.5
                }
            }
        }

        TextField {
            id: termField
            Layout.fillWidth: true
            text: search.term
            onTextEdited: search.term = text
            rightPadding: clearTermButton.width + padding

            Keys.onEscapePressed: {
                if (search.highlightedMatchIndex >= 0)
                    search.resetHighlight()
                else
                    root.visible = false
            }
            Keys.onUpPressed: search.highlightNext()
            Keys.onDownPressed: search.highlightPrevious()

            Button {
                id: clearTermButton
                height: parent.height * 0.75
                width: height
                icon.source: "qrc:/navigation/"+currentTheme+"/close.png"
                flat: true
                focusPolicy: Qt.NoFocus
                onClicked: lith.search.term = ""
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: termField.padding / 2
                }
            }
        }

        ColumnLayout {
            spacing: 0
            Label {
                size: Label.Small
                text: qsTr("Total: %1 lines").arg(lith.selectedBuffer ? lith.selectedBuffer.lines.count : "N/A")
                opacity: 0.7
            }
            Label {
                size: Label.Small
                text: search.term.length === 0 ? qsTr("Start typing to search")
                                               : search.highlightedMatchIndex >= 0 ? qsTr("Showing match %1 of %2").arg(search.highlightedMatchIndex + 1).arg(search.matches.length)
                                                                                   : qsTr("Matched: %1 lines").arg(search.matches.length)
            }
        }
        Button {
            icon.source: "qrc:/navigation/"+currentTheme+"/down-arrow.png"
            flat: true
            focusPolicy: Qt.NoFocus
            onClicked: search.highlightPrevious()
            enabled: search.previousEnabled
        }
        Button {
            icon.source: "qrc:/navigation/"+currentTheme+"/up-arrow.png"
            flat: true
            focusPolicy: Qt.NoFocus
            onClicked: search.highlightNext()
            enabled: search.nextEnabled
        }
    }
}
