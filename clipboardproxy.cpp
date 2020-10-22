#include "clipboardproxy.h"

#include <QApplication>
#include <QImage>

ClipboardProxy::ClipboardProxy(QObject *parent)
    : QObject(parent)
    , m_clipboard(QApplication::clipboard())
{
}

bool ClipboardProxy::hasImage() {
    return !m_clipboard->image().isNull();
}

QString ClipboardProxy::text() {
    return m_clipboard->text();
}
