// Lith
// Copyright (C) 2021 Martin Bříza
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

#ifndef FORMATTEDSTRING_H
#define FORMATTEDSTRING_H

#include <QObject>
#include <QString>
#include <QList>

#include "colortheme.h"

class FormattedString {
    Q_GADGET
public:
    struct Part {
        struct Color {
            int32_t index { 0 };
            bool extended { false };
        };

        Part(const QString &text) : text(text) {}
        QString text {};
        Color foreground;
        Color background;
        bool hyperlink { false };
        bool bold { false };
        bool underline { false };
        bool italic;
    };

    FormattedString() : m_parts({QString()}) {}
    FormattedString(const char *d) : m_parts({QString(d)}) {}
    FormattedString(const QString &o) : m_parts({o}) {}
    FormattedString(QString &&o) : m_parts({std::move(o)}) {}
    FormattedString &operator=(const QString &o) {
        m_parts = { o };
        return *this;
    }
    FormattedString &operator=(QString &&o) {
        m_parts = { std::move(o) };
        return *this;
    }
    FormattedString &operator=(const char *o) {
        m_parts = { QString(o) };
        return *this;
    }

    operator QString() const {
        return toPlain();
    }

    bool operator==(const FormattedString &o) {
        return toPlain() == o.toPlain();
    }
    bool operator!=(const FormattedString &o) {
        return !operator==(o);
    }
    bool operator==(const QString &o) {
        return toPlain() == o;
    }
    bool operator!=(const QString &o) {
        return !operator==(o);
    }

    FormattedString &operator+=(const char *s) {
        lastPart().text += s;
        return *this;
    }
    FormattedString &operator+=(const QString &s) {
        lastPart().text += s;
        return *this;
    }

    void clear() {
        m_parts = {{}};
    }

    Q_INVOKABLE QString toPlain() const {
        QString ret;
        for (auto &i : m_parts) {
            ret.append(i.text);
        }
        return ret;
    }
    Q_INVOKABLE QString testHtml() const {
        return toHtml(lightTheme);
    }
    Q_INVOKABLE QString toHtml(const ColorTheme &theme) const {
        QString ret { "<html><body>"};
        for (auto &i : m_parts) {
            if (i.bold)
                ret.append("<b>");
            if (i.underline)
                ret.append("<u>");
            if (i.foreground.index >= 0) {
                ret.append("<font color=\"");
                if (i.foreground.extended) {
                    if (theme.extendedColors.count() > i.foreground.index)
                        ret.append(theme.extendedColors[i.foreground.index]);
                    else
                        ret.append("pink");
                }
                else {
                    if (theme.weechatColors.count() > i.foreground.index)
                        ret.append(theme.weechatColors[i.foreground.index]);
                    else
                        ret.append("pink");
                }
                ret.append("\">");
            }
            ret.append(i.text);
            if (i.foreground.index >= 0)
                ret.append("</color>");
            if (i.underline)
                ret.append("</u>");
            if (i.bold)
                ret.append("</b>");
        }
        ret.append("</body></html>");
        return ret;
    }

    FormattedString &append(const Part &p = {{}}) {
        m_parts.append(p);
        return *this;
    }
    Part &lastPart() {
        return m_parts.last();
    }
    void prune() {
        auto it = m_parts.begin();
        while (it != m_parts.end()) {
            if (it->text.isEmpty())
                it = m_parts.erase(it);
            else
                ++it;
        }
    }

    QStringList split(const QString &sep) const {
        return toPlain().split(sep);
    }

    qlonglong toLongLong(bool *ok = nullptr, int base = 10) const {
        return toPlain().toLongLong(ok, base);
    }

    QString toLower() const {
        return toPlain().toLower();
    }

    std::string toStdString() const {
        return toPlain().toStdString();
    }


private:
    QList<Part> m_parts {};
};

Q_DECLARE_METATYPE(FormattedString)

#endif // FORMATTEDSTRING_H
