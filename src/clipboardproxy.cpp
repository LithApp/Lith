// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Jakub Mach
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

#include "clipboardproxy.h"

#include <QApplication>
#include <QMimeData>
#include <QImage>
#include <QDebug>
#include <QUrl>

#include "iosclipboard.h"


ClipboardProxy::ClipboardProxy(QObject *parent)
    : QObject(parent)
    , m_clipboard(QApplication::clipboard())
{
}

bool ClipboardProxy::hasImage() {
#ifdef Q_OS_IOS
    return iosHasImage();
#else // not iOS
    return m_clipboard->mimeData()->hasImage();
#endif
}

QString ClipboardProxy::text() {
#ifdef Q_OS_IOS
    if (iosHasUrl()) {
        return getIosUrl();
    }
    else {
        return m_clipboard->text();
    }
#else // not iOS
    return m_clipboard->text();
#endif
}

QImage ClipboardProxy::image() {    
#ifdef Q_OS_IOS
    return getIosImage();
#else // not iOS
    return m_clipboard->image();
#endif
}

void ClipboardProxy::setText(const QString &text) {
    m_clipboard->setText(text);
}
