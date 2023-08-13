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
    Q_PROPERTY(int length READ length CONSTANT)
    Q_PROPERTY(QStringList urls READ urls CONSTANT)
public:
    struct Part {
        struct Color {
            int32_t index { -1 };
            bool extended { false };
            QColor toQColor(const ColorTheme &theme = getCurrentTheme()) const;
        };

        Part(QString text) : text(std::move(text)) {}
        bool containsHtml() const { return foreground.index >= 0 || background.index >= 0 || hyperlink || bold || underline || italic; }
        QString toHtml(const ColorTheme &theme) const;

        QString text {};
        Color foreground { -1, false };
        Color background { -1, false };
        bool hyperlink { false };
        bool bold { false };
        bool underline { false };
        bool italic { false };
    };

    FormattedString(const FormattedString &o);
    FormattedString(FormattedString &&o) noexcept;
    FormattedString(const QString &o);
    FormattedString(QString &&o);
    FormattedString(const char *d);
    FormattedString();
    ~FormattedString() = default;

    FormattedString &operator=(const QString &o);
    FormattedString &operator=(QString &&o);
    FormattedString &operator=(const FormattedString &o);
    FormattedString &operator=(FormattedString &&o) noexcept;
    FormattedString &operator=(const char *o);

    bool operator==(const FormattedString &o) const;
    bool operator!=(const FormattedString &o) const;
    bool operator==(const QString &o) const;
    bool operator!=(const QString &o) const;

    // these methods append to the last available segment
    FormattedString &operator+=(const char *s);
    FormattedString &operator+=(const QString &s);

    operator QString() const;

    static const ColorTheme &getCurrentTheme();

    Q_INVOKABLE QString toPlain() const;
    Q_INVOKABLE QString toHtml(const ColorTheme &theme = getCurrentTheme()) const;
    Q_INVOKABLE QString toTrimmedHtml(int n, const ColorTheme &theme = getCurrentTheme()) const;

    bool containsHtml() const;

    int count() const;
    void clear();

    Part &addPart(const Part &p = {{}});
    Part &firstPart();
    const Part &firstPart() const;
    Part &lastPart();
    const Part &lastPart() const;
    const Part& at(int index) const;
    // prune would potentially (not 100% done) remove all empty parts and merge the ones with the same formatting
    void prune();

    // QString compatibility wrappers
    QStringList split(const QString &sep) const;
    qlonglong toLongLong(bool *ok = nullptr, int base = 10) const;
    QString toLower() const;
    std::string toStdString() const;
    int length() const;

    QStringList urls() const;

private:
    QList<Part> m_parts {};
};

Q_DECLARE_METATYPE(FormattedString)

#endif // FORMATTEDSTRING_H
