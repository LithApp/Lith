import QtQuick 2.5
import QtQuick.Layouts 1.1

RowLayout {
    width: parent ? parent.width : implicitWidth
    spacing: 0
    Item {
        Layout.preferredWidth: metrics.width
        height: 1
    }
    TextMetrics {
        id: metrics
        text: sender + "a"
        font.family: "Consolas"
    }

    TextEdit {
        readOnly: true
        persistentSelection: true
        font.family: "Consolas"
        Layout.fillWidth: true
        color: line.isPrivMsg ? "black" : "light gray"
        font.bold: line.highlight ? true : false
        text: line.message
        textFormat: Text.RichText
        onLinkActivated: Qt.openUrlExternally(link)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
