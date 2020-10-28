#include "clipboardproxy.h"

#include <QApplication>
#include <QMimeData>
#include <QImage>

ClipboardProxy::ClipboardProxy(QObject *parent)
    : QObject(parent)
    , m_clipboard(QApplication::clipboard())
{
}

bool ClipboardProxy::hasImage() {
    return !m_clipboard->mimeData()->hasImage();
}

QString ClipboardProxy::text() {
    return m_clipboard->text();
}

QImage ClipboardProxy::image() {
    return m_clipboard->image();
}

void ClipboardProxy::setText(const QString &text) {
    m_clipboard->setText(text);
}
