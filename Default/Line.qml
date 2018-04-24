import QtQuick 2.5
import QtQuick.Layouts 1.1

RowLayout {
    width: parent ? parent.width : implicitWidth
    spacing: 0
    TextEdit {
        readOnly: true
        persistentSelection: true
        font.family: "Consolas"
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
        color: "gray"
        text: Qt.formatTime(line.date, Qt.DefaultLocaleShortDate) + " " + line.prefix + " "
    }
    TextEdit {
        readOnly: true
        persistentSelection: true
        font.family: "Consolas"
        Layout.fillWidth: true
        text: line.message
        textFormat: Text.RichText
        onLinkActivated: Qt.openUrlExternally(link)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
