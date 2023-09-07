import QtQuick
import QtQuick.Templates as T

import Lith.Core
import Lith.Style

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

    background: Rectangle {
        color: LithPalette.regular.window

        border.width: borderless ? 0 : 1
        border.color: LithPalette.regular.light
    }

    T.Overlay.modal: Rectangle {
        color: ColorUtils.setAlpha(LithPalette.regular.window, 0.7)
    }

    T.Overlay.modeless: Rectangle {
        color: ColorUtils.setAlpha(LithPalette.regular.window, 0.7)
    }
}
