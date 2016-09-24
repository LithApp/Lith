import QtQuick 2.5
import QtQuick.Layouts 1.1

RowLayout {
    width: parent.width
    spacing: 0
    Text {
        font.family: "Consolas"
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
        color: "gray"
        text: Qt.formatTime(line.date, Qt.DefaultLocaleShortDate) + " " + line.prefix + " "
    }
    Text {
        font.family: "Consolas"
        Layout.fillWidth: true
        text: line.message
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
