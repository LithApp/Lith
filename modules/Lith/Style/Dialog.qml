import QtQuick
import QtQuick.Templates as T

T.Dialog {
    padding: 0
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property bool borderless: false

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding,
                            implicitHeaderWidth,
                            implicitFooterWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding
                             + (implicitHeaderHeight > 0 ? implicitHeaderHeight + spacing : 0)
                             + (implicitFooterHeight > 0 ? implicitFooterHeight + spacing : 0))

    SystemPalette {
        id: palette
    }

    background: Rectangle {
        color: palette.window

        border.width: borderless ? 0 : 1
        border.color: palette.light
    }

    T.Overlay.modal: Rectangle {
        color: colorUtils.setAlpha(palette.window, 0.7)
    }

    T.Overlay.modeless: Rectangle {
        color: colorUtils.setAlpha(palette.window, 0.7)
    }
}
