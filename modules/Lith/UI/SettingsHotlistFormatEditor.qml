import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "SettingsFields" as Fields
import Lith.Core
import Lith.Style

Dialog {
    id: root

    property bool helpVisible: false

    modal: true
    anchors.centerIn: Overlay.overlay
    implicitWidth: 400
    closePolicy: autocomplete.visible ? Popup.CloseOnPressOutside : (Popup.CloseOnEscape | Popup.CloseOnPressOutside)
    onVisibleChanged: {
        helpVisible = false
        if (visible) {
            formatSplitter.fromStringList(Lith.settings.hotlistFormat)
        }
        if (!visible) {
            autocomplete.visible = false
        }
    }

    onAccepted: {
        Lith.settings.hotlistFormat = formatSplitter.stringList
    }
    onRejected: {
        formatSplitter.fromStringList(Lith.settings.hotlistFormat)
    }

    header: Fields.Header {
        id: headerField
        width: parent.width
        standalone: true
        text: qsTr("Hotlist format editor")

        leftPadding: helpButton.x + helpButton.width + 6
        labelItem.wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        labelItem.maximumLineCount: 2

        Button {
            id: helpButton
            text: root.helpVisible ? qsTr("Back") : qsTr("Help")
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 6
            onClicked: root.helpVisible = !root.helpVisible
        }
    }

    footer: DialogButtons {
        id: dialogButtons
        enabled: !root.helpVisible // disable the buttons so the user doesn't get confused
        acceptable: formatSplitter.variablesValid && formatSplitter.formatValid
        dialog: root
    }

    Reflection {
        id: reflection
        className: "Buffer"
    }

    Popup {
        id: autocomplete
        y: parent ? parent.height + 3 : 0
        width: parent ? parent.width : 400
        height: mainItem.height - dialogButtons.height - headerField.height

        Rectangle {
            width: parent.width
            height: parent.height
            border.color: Qt.rgba(LithPalette.regular.buttonText.r, LithPalette.regular.buttonText.g, LithPalette.regular.buttonText.b, 0.2)
            border.width: 1
            color: LithPalette.regular.window

            ScrollView {
                id: autocompleteScrollView
                anchors.fill: parent
                Flow {
                    id: autocompleteFlow
                    x: 6
                    y: 6
                    width: autocompleteScrollView.width - autocompleteScrollView.ScrollBar.vertical.width - 6
                    spacing: 12
                    Repeater {
                        model: reflection.stringProperties
                        Rectangle {
                            required property string modelData
                            visible: modelData && autocomplete.parent && "%1".arg(modelData).startsWith(autocomplete.parent.text)
                            width: suggestionLabel.width + 27
                            height: suggestionLabel.height + 18
                            color: ColorUtils.mixColors(LithPalette.regular.buttonText, LithPalette.regular.window, autocompleteDelegateMouse.pressed ? 0.25 : autocompleteDelegateMouse.containsMouse ? 0.15 : 0.1)
                            Label {
                                id: suggestionLabel
                                anchors.centerIn: parent
                                text: modelData
                                color: LithPalette.regular.buttonText
                            }
                            MouseArea {
                                id: autocompleteDelegateMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    autocomplete.parent.text = modelData
                                    autocomplete.visible = false
                                }
                            }
                        }
                    }
                    Item {
                        width: autocompleteFlow.width
                        height: 3
                    }
                }
            }
        }
    }

    ScrollView {
        id: mainItem
        width: parent.width
        height: parent.height
        contentHeight: mainLayout.height

        NumberAnimation {
            id: scrollToSelectedPropertyAnimation
            target: mainItem.ScrollBar.vertical
            property: "position"
            duration: 120
            function run(newPosition) {
                to = newPosition
                running = true
            }
        }

        ColumnLayout {
            id: mainLayout
            width: mainItem.width - (mainItem.ScrollBar.vertical.visible ? mainItem.ScrollBar.vertical.width : 0)
            spacing: 0

            Fields.Base {
                id: formatField
                visible: !root.helpVisible
                summary: qsTr("Format definition")

                columnComponent: ColumnLayout {
                    id: formatLayout
                    Layout.fillWidth: true
                    Layout.bottomMargin: 6
                    spacing: 6

                    TextField {
                        id: formatInput
                        Layout.fillWidth: true
                        text: formatSplitter.format
                        onTextChanged: formatSplitter.format = text
                        borderColor: formatSplitter.formatValid ? "transparent" : "red"

                        FormatStringSplitter {
                            id: formatSplitter
                            allowedPropertyNames: reflection.stringProperties
                            dataSource: Lith.selectedBuffer
                            onErrorStringChanged: console.warn(errorString)
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

                    Repeater {
                        id: variableRepeater
                        model: formatSplitter.variables
                        delegate: Item {
                            id: variableDelegate
                            Layout.fillWidth: true
                            implicitHeight: variableDelegateLayout.implicitHeight
                            RowLayout {
                                id: variableDelegateLayout
                                width: parent.width
                                height: parent.height
                                Label {
                                    Layout.alignment: Qt.AlignVCenter
                                    text: "%%1".arg(modelData.index + 1)
                                }
                                TextField {
                                    id: variableDelegateInput
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
                                        if (focus) {
                                            scrollToSelectedPropertyAnimation.run(variableDelegate.y / mainLayout.height)
                                            if (!valid) {
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
            }

            Fields.Base {
                visible: !root.helpVisible
                summary: qsTr("Preview")
                details: qsTr("Data from %1").arg(Lith.selectedBuffer ? Lith.selectedBuffer.name : "N/A")

                rowComponent: Label {
                    id: preview
                    Layout.fillWidth: true
                    horizontalAlignment: Label.AlignRight
                    elide: Label.ElideRight
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    property bool displaysError: formatSplitter.errorString.length > 0
                    text: Lith.selectedBuffer ? displaysError ? formatSplitter.errorString : formatSplitter.formattedData : ""
                    color: displaysError ? "red" : LithPalette.disabled.text
                }
            }

            Fields.Button {
                visible: !root.helpVisible
                summary: qsTr("Undo current changes")
                enabled: formatSplitter.stringList.toString() !== Lith.settings.hotlistFormat.toString()

                text: qsTr("Reset")
                onClicked: formatSplitter.fromStringList(Lith.settings.hotlistFormat)
            }

            Fields.Base {
                id: presetFieldBase
                visible: !root.helpVisible
                summary: qsTr("Use a preset")
                details: qsTr("Recommended")
                rowComponent: ColumnLayout {
                    id: presetLayout
                    Layout.fillWidth: true

                    Button {
                        text: qsTr("Default")
                        onClicked: formatSplitter.fromStringList(Lith.settings.hotlistDefaultFormat)
                        Layout.fillWidth: true
                        Layout.maximumWidth: 120
                    }
                    Button {
                        text: qsTr("Buffer Number")
                        onClicked: formatSplitter.fromStringList(["%1", "number"])
                        Layout.fillWidth: true
                        Layout.maximumWidth: 120
                    }
                }
            }

            Item {
                id: help
                visible: root.helpVisible
                implicitHeight: helpLayout.implicitHeight
                Layout.fillWidth: true
                Layout.preferredHeight: helpLayout.implicitHeight
                ColumnLayout {
                    id: helpLayout
                    x: 6
                    y: 3
                    width: parent.width - x * 2
                    Label {
                        Layout.fillWidth: true
                        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        text: qsTr("How to use this editor")
                        horizontalAlignment: Label.AlignHCenter
                        font.pixelSize: FontSizes.medium
                    }
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
                        Item { Layout.fillWidth: true }
                        HotListItem {
                            text: "3 ABC #lith 3"
                        }
                        Item { Layout.fillWidth: true }
                        HotListItem {
                            hot: true
                            text: "3 ABC #lith 3"
                        }
                        Item { Layout.fillWidth: true }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.margins: 6
                        height: 1
                        color: LithPalette.disabled.text
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
                            model: reflection.stringProperties
                            Label {
                                color: LithPalette.disabled.text
                                Layout.leftMargin: 18
                                text: modelData
                            }
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.margins: 6
                        height: 1
                        color: LithPalette.disabled.text
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
