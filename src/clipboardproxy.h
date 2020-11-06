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

#ifndef CLIPBOARDPROXY_H
#define CLIPBOARDPROXY_H

#include <QObject>
#include <QClipboard>
#include <QPixmap>

class ClipboardProxy : public QObject {
    Q_OBJECT
public:
    ClipboardProxy(QObject *parent = nullptr);

    Q_INVOKABLE bool hasImage();

    Q_INVOKABLE QString text();
    Q_INVOKABLE QImage image();

    Q_INVOKABLE void setText(const QString &text);

private:
    QClipboard *m_clipboard;
};

#endif // CLIPBOARDPROXY_H
