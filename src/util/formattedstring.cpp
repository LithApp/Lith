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

#include "formattedstring.h"

#include <QRegularExpression>
#include <QDebug>

QString FormattedString::Part::toHtml(const ColorTheme &theme) const {
    QString ret;
    if (bold)
        ret.append("<b>");
    if (underline)
        ret.append("<u>");
    if (foreground.index >= 0) {
        ret.append("<font color=\"");
        if (foreground.extended) {
            if (theme.extendedColors.count() > foreground.index)
                ret.append(theme.extendedColors[foreground.index]);
            else
                ret.append("pink");
        }
        else {
            if (theme.weechatColors.count() > foreground.index)
                ret.append(theme.weechatColors[foreground.index]);
            else
                ret.append("pink");
        }
        ret.append("\">");
    }
    if (hyperlink) {
        ret.append("<a href=\"");
        ret.append(text);
        ret.append("\">");
    }
    ret.append(text);
    if (hyperlink) {
        ret.append("</a>");
    }
    if (foreground.index >= 0)
        ret.append("</color>");
    if (underline)
        ret.append("</u>");
    if (bold)
        ret.append("</b>");
    return ret;
}

FormattedString::FormattedString()
    : m_parts({QString()})
{}

FormattedString::FormattedString(const char *d)
    : m_parts({QString(d)})
{}

FormattedString::FormattedString(const QString &o)
    : m_parts({o})
{}

FormattedString::FormattedString(QString &&o)
    : m_parts({std::move(o)})
{}

FormattedString &FormattedString::operator=(const char *o) {
    m_parts = { QString(o) };
    return *this;
}

bool FormattedString::operator==(const QString &o) {
    return toPlain() == o;
}

bool FormattedString::operator!=(const QString &o) {
    return !operator==(o);
}

FormattedString &FormattedString::operator+=(const QString &s) {
    lastPart().text += s;
    return *this;
}

void FormattedString::clear() {
    m_parts = {{}};
}

FormattedString::Part &FormattedString::addPart(const FormattedString::Part &p) {
    m_parts.append(p);
    return m_parts.last();
}

QString FormattedString::toPlain() const {
    QString ret;
    for (auto &i : m_parts) {
        ret.append(i.text);
    }
    return ret;
}

QString FormattedString::testHtml() const {
    return toHtml(lightTheme);
}

QString FormattedString::toHtml(const ColorTheme &theme) const {
    QString ret { "<html><body>"};
    for (auto &i : m_parts) {
        ret.append(i.toHtml(theme));
    }
    ret.append("</body></html>");
    return ret;
}

QString FormattedString::toTrimmedHtml(int n) const {
    if (n <= 0)
        return toHtml(lightTheme);
    QString ret = "<html><body>";
    for (auto &i : m_parts) {
        QString word = i.text.left(n);
        Part tempPart = i;
        tempPart.text = word;
        ret.append(tempPart.toHtml(lightTheme));
        n -= word.count();
        if (n <= 0)
            break;
    }
    while (n >= 0) {
        ret.append("\u00A0");
        n--;
    }
    ret += "</body></html>";
    return ret;
}

bool FormattedString::containsHtml() const {
    return m_parts.count() > 1 || m_parts.first().containsHtml();
}

FormattedString::Part &FormattedString::lastPart() {
    return m_parts.last();
}

void FormattedString::prune() {
    auto it = m_parts.begin();
    while (it != m_parts.end()) {
        QRegularExpression re(R"(((?:(?:https?|ftp|file):\/\/|www\.|ftp\.)(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.;]*\)|[-A-Z0-9+&@#\/%=~_|$?!:,.;])*(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.;]*\)|[A-Z0-9+&@#\/%=~_|$;])))",
                              QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::ExtendedPatternSyntaxOption);

        auto reIt = re.globalMatch(it->text, 0, QRegularExpression::PartialPreferFirstMatch);
        if (reIt.hasNext()) {
            QList<Part> segments;
            int previousEnd = 0;
            while (reIt.hasNext()) {
                auto reMatch = reIt.next();
                Part prefix = { it->text.mid(previousEnd, reMatch.capturedStart() - previousEnd) };
                Part url = { it->text.mid(reMatch.capturedStart(), reMatch.capturedLength()) };
                url.hyperlink = true;
                segments.append(prefix);
                segments.append(url);
                previousEnd = reMatch.capturedEnd();
            }
            if (previousEnd < it->text.count()) {
                Part suffix = { it->text.mid(previousEnd, it->text.count() - previousEnd) };
                segments.append(suffix);
            }
            it = m_parts.erase(it);
            for (auto &i : segments) {
                it = m_parts.insert(it, i);
                ++it;
            }
        }
        else {
            ++it;
        }
    }
    while (it != m_parts.end()) {
        if (it->text.isEmpty())
            it = m_parts.erase(it);
        else
            ++it;
    }
    it = m_parts.begin();
}

QStringList FormattedString::split(const QString &sep) const {
    return toPlain().split(sep);
}

qlonglong FormattedString::toLongLong(bool *ok, int base) const {
    return toPlain().toLongLong(ok, base);
}

QString FormattedString::toLower() const {
    return toPlain().toLower();
}

std::string FormattedString::toStdString() const {
    return toPlain().toStdString();
}

int FormattedString::length() const {
    return toPlain().length();
}

FormattedString &FormattedString::operator+=(const char *s) {
    lastPart().text += s;
    return *this;
}

bool FormattedString::operator!=(const FormattedString &o) {
    return !operator==(o);
}

bool FormattedString::operator==(const FormattedString &o) {
    return toPlain() == o.toPlain();
}

FormattedString::operator QString() const {
    return toPlain();
}

FormattedString &FormattedString::operator=(QString &&o) {
    m_parts = { std::move(o) };
    return *this;
}

FormattedString &FormattedString::operator=(const QString &o) {
    m_parts = { o };
    return *this;
}
