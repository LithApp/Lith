import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "SettingsFields" as Fields
import lith

Dialog {
    id: root
    modal: true
    anchors.centerIn: Overlay.overlay
    width: 400
    implicitHeight: Math.min(header.height + mainItem.implicitHeight, mainView.height)
    closePolicy: autocomplete.visible ? Popup.CloseOnPressOutside : (Popup.CloseOnEscape | Popup.CloseOnPressOutside)
    onVisibleChanged: {
        if (visible) {
            formatSplitter.fromStringList(lith.settings.hotlistFormat)
        }
        if (!visible) {
            autocomplete.visible = false
        }
    }

    onAccepted: {
        lith.settings.hotlistFormat = formatSplitter.stringList
    }
    onRejected: {
        formatSplitter.fromStringList(lith.settings.hotlistFormat)
    }

    header: Fields.Header {
        width: parent.width
        standalone: true
        text: qsTr("Hotlist format editor")

        Button {
            text: help.visible ? "<" : "?"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 9
            width: height
            onClicked: help.visible = !help.visible
        }
    }

    Component.onCompleted: console.log(reflection.stringProperties)

    Reflection {
        id: reflection
        className: "Buffer"
    }

    Popup {
        id: autocomplete
        y: parent ? parent.height + 3 : 0
        width: parent ? parent.width : width
        height: mainLayout.height - formatField.height - 6
        Rectangle {
            width: parent.width
            height: parent.height
            border.color: Qt.rgba(palette.buttonText.r, palette.buttonText.g, palette.buttonText.b, 0.2)
            border.width: 1
            color: palette.window
            clip: true
            ScrollView {
                id: autocompleteScrollView
                anchors.fill: parent
                Flow {
                    id: autocompleteFlow
                    x: 6
                    width: autocompleteScrollView.width - 12
                    spacing: 12
                    Item {
                        width: autocompleteFlow.width
                        height: 6
                    }
                    Repeater {
                        model: reflection.stringProperties
                        Rectangle {
                            required property string modelData
                            visible: modelData && autocomplete.parent && "%1".arg(modelData).startsWith(autocomplete.parent.text)
                            width: suggestionLabel.width + 27
                            height: suggestionLabel.height + 18
                            color: Qt.rgba(palette.buttonText.r, palette.buttonText.g, palette.buttonText.b, 0.1)
                            Label {
                                id: suggestionLabel
                                anchors.centerIn: parent
                                text: modelData
                                color: palette.buttonText
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    autocomplete.parent.text = modelData
                                    autocomplete.visible = false
                                }
                            }
                        }
                    }
                    Item {
                        width: autocompleteFlow.width
                        height: 6
                    }
                }
            }
        }
    }

    Item {
        id: mainItem
        width: parent.width
        implicitHeight: mainLayout.implicitHeight
        height: parent.height
        ColumnLayout {
            id: mainLayout
            width: parent.width
            height: parent.height
            spacing: -1

            Fields.Base {
                id: formatField
                summary: qsTr("Format definition")
                Layout.fillHeight: true
                Layout.minimumHeight: 180

                columnComponent: ScrollView {
                    id: formatScrollView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    ColumnLayout {
                        id: formatLayout
                        width: formatScrollView.width
                        spacing: 6
                        // Setting focus directly from Repeater.itemAdded didn't work probably because all signals haven't been processed at that moment yet
                        // Delay by 1ms, it's a little bit of a hack but it works seamlessly
                        Timer {
                            id: delayedFocus
                            interval: 1
                            property int index
                            function execute(index) {
                                delayedFocus.index = index
                                start()
                            }
                            onTriggered: formatRepeater.itemAt(index).forceActiveFocus()
                        }

                        TextField {
                            id: formatInput
                            Layout.fillWidth: true
                            text: formatSplitter.format
                            onTextChanged: formatSplitter.format = text
                            borderColor: formatSplitter.formatValid ? "transparent" : "red"

                            FormatStringSplitter {
                                id: formatSplitter
                                allowedPropertyNames: reflection.stringProperties
                                dataSource: lith.selectedBuffer
                            }
                        }

                        Label {
                            Layout.fillWidth: true
                            Layout.minimumHeight: fontInfo.pixelSize + 16
                            Layout.leftMargin: 6
                            verticalAlignment: Label.AlignVCenter
                            elide: Label.ElideRight
                            text: qsTr("Properties")
                        }

                        ListView {
                            id: variableListView
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.bottomMargin: 9
                            implicitHeight: contentHeight
                            spacing: 6
                            model: formatSplitter.variables
                            delegate: RowLayout {
                                width: variableListView.width
                                Label {
                                    Layout.alignment: Qt.AlignVCenter
                                    text: "%%1".arg(modelData.index + 1)
                                }
                                TextField {
                                    Layout.fillWidth: true
                                    text: modelData.name
                                    Binding on text {
                                        value: modelData.name
                                    }

                                    borderColor: valid ? "transparent" : "red"

                                    property bool valid: modelData.valid
                                    onValidChanged: {
                                        if (valid && autocomplete.visible && autocomplete.parent === this)
                                            autocomplete.visible = false
                                    }

                                    property bool autocompleteDismissed: false
                                    Keys.onEscapePressed: {
                                        autocomplete.visible = false
                                        autocompleteDismissed = true
                                    }

                                    onTextChanged: {
                                        // onTextChanged because autocomplete writes here
                                        modelData.name = text
                                    }

                                    onTextEdited: {
                                        if (focus && !autocompleteDismissed) {
                                            autocomplete.parent = this
                                            autocomplete.visible = true
                                        }
                                    }

                                    onFocusChanged: {
                                        if (focus && !valid) {
                                            autocompleteDismissed = false
                                            autocomplete.parent = this
                                            autocomplete.visible = true
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Fields.Base {
                summary: qsTr("Preview")
                details: qsTr("Data from %1").arg(lith.selectedBuffer ? lith.selectedBuffer.name : "N/A")

                rowComponent: Label {
                    id: preview
                    Layout.fillWidth: true
                    horizontalAlignment: Label.AlignRight
                    elide: Label.ElideRight
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    property bool displaysError: formatSplitter.errorString.length > 0
                    text: lith.selectedBuffer ? displaysError ? formatSplitter.errorString : formatSplitter.formattedData : ""
                    opacity: displaysError ? 1.0 : 0.5
                    color: displaysError ? "red" : palette.windowText
                }
            }

            Fields.Button {
                summary: qsTr("Undo current changes")
                enabled: formatSplitter.stringList.toString() !== lith.settings.hotlistFormat.toString()

                text: qsTr("Reset")
                onClicked: formatSplitter.fromStringList(lith.settings.hotlistFormat)
            }

            Fields.Base {
                summary: qsTr("Use a preset")
                details: qsTr("Recommended")
                rowComponent: ColumnLayout {
                    Button {
                        text: qsTr("Default")
                        onClicked: formatSplitter.fromStringList(lith.settings.hotlistDefaultFormat)
                        Layout.preferredWidth: implicitWidth * 1.2
                    }
                    Button {
                        text: qsTr("Buffer Number")
                        onClicked: formatSplitter.fromStringList(["%1", "number"])
                        Layout.preferredWidth: implicitWidth * 1.2
                    }
                }
            }

            DialogButtons {
                id: dialogButtons
                acceptable: formatSplitter.variablesValid && formatSplitter.formatValid
                Layout.fillWidth: true
                dialog: root
            }
        }
        Rectangle {
            id: help
            anchors.fill: parent
            anchors.margins: 1
            visible: false
            color: palette.window
            ScrollView {
                id: helpScrollView
                anchors.fill: parent
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ColumnLayout {
                    x: 6
                    y: 6
                    width: helpScrollView.width - 12
                    Label {
                        Layout.fillWidth: true
                        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        text: qsTr("The top field defines the format of the string that will be displayed. Regular text will be displayed as-is and all percent-encoded numbers will be replaced with variables defined in the fields below. For example:")
                    }
                    TextField {
                        readOnly: true
                        Layout.fillWidth: true
                        text: "%1 ABC %2 %1"
                    }
                    Label {
                        Layout.fillWidth: true
                        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        text: qsTr("Will allow you to enter two variables, for example:")
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            Layout.alignment: Qt.AlignVCenter
                            text: "%%1".arg(1)
                        }
                        TextField {
                            readOnly: true
                            Layout.fillWidth: true
                            text: "number"
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            Layout.alignment: Qt.AlignVCenter
                            text: "%%1".arg(2)
                        }
                        TextField {
                            readOnly: true
                            Layout.fillWidth: true
                            text: "short_name"
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        text: qsTr("And these will then result in displaying this hotlist item:")
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Item { Layout.fillWidth: true}
                        HotListItem {
                            text: "3 ABC #lith 3"
                        }
                        Item { Layout.fillWidth: true}
                        HotListItem {
                            hot: true
                            text: "3 ABC #lith 3"
                        }
                        Item { Layout.fillWidth: true}
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.margins: 6
                        height: 1
                        color: palette.windowText
                        opacity: 0.3
                    }

                    Label {
                        Layout.fillWidth: true
                        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        text: qsTr("The following properties are available:")
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        Repeater {
                            model: reflection.bufferStringProperties
                            Label {
                                opacity: 0.7
                                Layout.leftMargin: 18
                                text: modelData
                            }
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.margins: 6
                        height: 1
                        color: palette.windowText
                        opacity: 0.3
                    }
                    Label {
                        Layout.fillWidth: true
                        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        text: qsTr("Please keep in mind the names of these variables can change in the future.")
                    }

                    Item {
                        Layout.fillHeight: true
                        Layout.minimumHeight: 9
                    }
                }
            }
        }
    }
}
