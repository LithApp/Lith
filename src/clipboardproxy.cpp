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
