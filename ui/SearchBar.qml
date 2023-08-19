import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core
import Lith.Style

// Had to use a wrapper Item to enforce parent layout boundaries... shouldn't be necessary, really, but I probably made a mistake in some higher level
Item {
    id: root

    property var search: Lith.search
    property int currentIndex: -1
    property int currentMessageIndex: currentIndex >= 0 && currentIndex < search.matches.length ? search.matches[currentIndex] : -1

    property alias textInput: termField

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
        height: parent.height - 1
        spacing: 3
        Button {
            id: searchModeButton
            Layout.preferredHeight: implicitHeight - 2
            Layout.preferredWidth: height
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
            Layout.horizontalStretchFactor: 2
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
        }

        Button {
            id: clearTermButton
            Layout.preferredHeight: implicitHeight - 16
            Layout.minimumWidth: height
            Layout.alignment: Qt.AlignVCenter
            icon.source: "qrc:/navigation/"+currentTheme+"/close.png"
            flat: true
            focusPolicy: Qt.NoFocus
            onClicked: Lith.search.term = ""
        }

        ColumnLayout {
            id: resultsLayout
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            TextMetrics {
                id: resultsMetrics
                text: "Total: 123 lines"
                font: totalCountLabel.font
            }

            spacing: 0
            Label {
                id: totalCountLabel
                visible: resultsMetrics.width * 3.5 < rootLayout.width
                size: Label.Small
                text: qsTr("Total: %1 lines").arg(Lith.selectedBuffer ? Lith.selectedBuffer.lines.count : "N/A")
                opacity: 0.7
            }
            Label {
                size: Label.Small
                visible: resultsMetrics.width * 3.5 < rootLayout.width
                text: search.term.length === 0 ? qsTr("Start typing to search")
                                               : search.highlightedMatchIndex >= 0 ? qsTr("Showing match %1 of %2").arg(search.highlightedMatchIndex + 1).arg(search.matches.length)
                                                                                   : qsTr("Matched: %1 lines").arg(search.matches.length)
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                visible: resultsMetrics.width * 3.5 >= rootLayout.width
                opacity: search.term.length === 0 || search.highlightedMatchIndex < 0 ? 0.7 : 1
                text: search.term.length === 0 || search.highlightedMatchIndex < 0 ? "N/A" : search.highlightedMatchIndex + 1
            }
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                visible: resultsMetrics.width * 3.5 >= rootLayout.width && search.term.length !== 0
                width: totalCountLabel.font.pixelSize
                height: 1
                color: totalCountLabel.color
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                visible: resultsMetrics.width * 3.5 >= rootLayout.width && search.term.length !== 0
                text: search.matches.length
            }
        }
        Button {
            icon.source: "qrc:/navigation/"+currentTheme+"/down-arrow.png"
            Layout.preferredHeight: implicitHeight - 2
            Layout.minimumWidth: height
            flat: true
            focusPolicy: Qt.NoFocus
            onClicked: search.highlightPrevious()
            enabled: search.previousEnabled
        }
        Button {
            icon.source: "qrc:/navigation/"+currentTheme+"/up-arrow.png"
            Layout.preferredHeight: implicitHeight - 2
            Layout.minimumWidth: height
            flat: true
            focusPolicy: Qt.NoFocus
            onClicked: search.highlightNext()
            enabled: search.nextEnabled
        }
    }
}
