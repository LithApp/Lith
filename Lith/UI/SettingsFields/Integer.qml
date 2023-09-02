import QtQuick
import QtQuick.Controls

// This recursively instantiates the String field and just sets reasonable defaults for number input
String {
    validator: IntValidator {}
    inputMethodHints: Qt.ImhDigitsOnly
}
