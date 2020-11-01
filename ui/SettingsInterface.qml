import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: root
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    function onAccepted() {
        settings.shortenLongUrls = shortenLongUrlsCheckbox.checked
        settings.shortenLongUrlsThreshold = shortenLongUrlsThreshold.text
        settings.showAutocompleteButton = showAutocompleteButtonCheckbox.checked
        settings.showGalleryButton = showGalleryButtonCheckbox.checked
        settings.showSendButton = showSendButtonCheckbox.checked
    }
    function onRejected() {
        shortenLongUrlsCheckbox.checked = settings.shortenLongUrls
        shortenLongUrlsThreshold.text = settings.shortenLongUrlsThreshold
        showAutocompleteButtonCheckbox.checked = settings.showAutocompleteButton
        showGalleryButtonCheckbox.checked = settings.showGalleryButton
        showSendButtonCheckbox.checked = settings.showSendButton
    }

    ColumnLayout {
        x: 6
        y: 6
        width: root.width - 12

        GroupBox {
            Layout.alignment: Qt.AlignHCenter
            title: "Input bar"
            GridLayout {
                id: inputBarLayout
                columns: 2
                Text {
                    text: "Show autocomplete button"
                }
                CheckBox {
                    id: showAutocompleteButtonCheckbox
                    checked: settings.showAutocompleteButton
                    Layout.alignment: Qt.AlignLeft
                }
                Text {
                    text: "Show gallery button"
                }
                CheckBox {
                    id: showGalleryButtonCheckbox
                    checked: settings.showGalleryButton
                    Layout.alignment: Qt.AlignLeft
                }
                Text {
                    text: "Show send button"
                }
                CheckBox {
                    id: showSendButtonCheckbox
                    checked: settings.showSendButton
                    Layout.alignment: Qt.AlignLeft
                }
            }
        }

        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            columns: 2
            Text {
                text: "Shorten long URLs"
            }
            CheckBox {
                id: shortenLongUrlsCheckbox
                checked: settings.shortenLongUrls
                Layout.alignment: Qt.AlignLeft
            }
            Text {
                text: "Length threshold"
            }
            TextField {
                id: shortenLongUrlsThreshold
                enabled: shortenLongUrlsCheckbox.checked
                text: settings.shortenLongUrlsThreshold
                inputMethodHints: Qt.ImhPreferNumbers
                validator: IntValidator {
                    bottom: 0
                }
            }
            Text {
                text: "Font size"
            }
            SpinBox {
                value: settings.baseFontSize
                onValueChanged: settings.baseFontSize = value
                from: 6
                to: 32
            }
        }
    }
}
