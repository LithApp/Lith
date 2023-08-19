// Lith
// Copyright (C) 2021 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
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

#include "windowhelper.h"
#include "lith.h"

#include <QRegularExpression>
#include <QDebug>
#include <QUrl>

namespace {
    Q_GLOBAL_STATIC(
        const QRegularExpression, urlRegExp,
        R"(((?:(?:https?|ftp|file):\/\/|www\.|ftp\.)(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.;]*\)|[-A-Z0-9+&@#\/%=~_|$?!:,.;])*(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.;]*\)|[A-Z0-9+&@#\/%=~_|$;])))",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption |
            QRegularExpression::ExtendedPatternSyntaxOption
    );
}

QString FormattedString::Part::toHtml(const ColorTheme& theme) const {
    QString ret;
    if (bold) {
        ret.append("<b>");
    }
    if (underline) {
        ret.append("<u>");
    }
    if (foreground.index >= 0) {
        ret.append("<font color=\"");
        if (foreground.extended) {
            if (theme.extendedColors().count() > foreground.index) {
                ret.append(theme.extendedColors()[foreground.index]);
            } else {
                ret.append("pink");
            }
        } else {
            if (theme.weechatColors().count() > foreground.index) {
                ret.append(theme.weechatColors()[foreground.index]);
            } else {
                ret.append("pink");
            }
        }
        ret.append("\">");
    }
    if (hyperlink) {
        ret.append("<a href=\"");
        ret.append(text);
        ret.append("\">");
    }

    QString finalText;
    const auto urlThreshold = Lith::settingsGet()->shortenLongUrlsThresholdGet();
    const auto urlShortenEnabled = Lith::settingsGet()->shortenLongUrlsGet();
    if (urlThreshold > 0 && hyperlink && text.size() > urlThreshold && urlShortenEnabled) {
        auto url = QUrl(text);
        auto scheme = url.scheme();
        auto host = url.host();
        auto file = url.fileName();
        auto query = url.query();
        auto path = url.path();

        // If we only have a hostname, we'll use it as is.
        if (path.isEmpty() || path == "/") {
            finalText = text;
        } else {
            // We'll show always show the host and the scheme.
            const auto hostPrefix = scheme + "://" + host + "/";
            const auto* const ellipsis = "\u2026";

            // The threshold is so small that it doesn't even accomodate the hostPrefix. We'll just put the hostPrefix and
            // ellipsis...
            if (hostPrefix.length() >= urlThreshold) {
                finalText = hostPrefix + ellipsis;
            } else {
                // This is a "nice" url with just a hostname and then one path fragment. We'll let these slide, because these tend
                // to look nice even if they're long. Something like https://host.domain/file.extension
                if (path == "/" + file && !url.hasQuery()) {
                    finalText = text;
                } else {
                    // Otherwise it's a weird link with multiple path fragments and queries and stuff. We'll just use the host and 10
                    // characters of the path.
                    const auto maxCharsToAppend = urlThreshold - hostPrefix.length();
                    finalText = hostPrefix + ellipsis + path.right(maxCharsToAppend - 1);
                }
            }
        }
    } else {
        finalText = text;
    }
    ret.append(finalText.toHtmlEscaped());

    if (hyperlink) {
        ret.append("</a>");
    }
    if (foreground.index >= 0) {
        ret.append("</font>");
    }
    if (underline) {
        ret.append("</u>");
    }
    if (bold) {
        ret.append("</b>");
    }
    return ret;
}

FormattedString::FormattedString()
    : m_parts({QString()}) {
}

FormattedString::FormattedString(const char* d)
    : m_parts({QString(d)}) {
}

FormattedString::FormattedString(const QString& o)
    : m_parts({o}) {
}

FormattedString::FormattedString(QString&& o) {
    m_parts.emplace_back(std::move(o));
}

FormattedString::FormattedString(const FormattedString& o)
    : m_parts({o.m_parts}) {
}

FormattedString::FormattedString(FormattedString&& o) noexcept
    : m_parts(std::move(o.m_parts)) {
}

FormattedString& FormattedString::operator=(const char* o) {
    m_parts = {QString(o)};
    return *this;
}

bool FormattedString::operator==(const QString& o) const {
    return toPlain() == o;
}

bool FormattedString::operator!=(const QString& o) const {
    return !operator==(o);
}

FormattedString& FormattedString::operator+=(const QString& s) {
    lastPart().text += s;
    return *this;
}

const ColorTheme& FormattedString::getCurrentTheme() {
    return Lith::instance()->windowHelperGet()->currentTheme();
}

void FormattedString::clear() {
    m_parts = {{}};
}

FormattedString::Part& FormattedString::addPart(const FormattedString::Part& p) {
    m_parts.append(p);
    return m_parts.last();
}

FormattedString::Part& FormattedString::firstPart() {
    return m_parts.first();
}

const FormattedString::Part& FormattedString::firstPart() const {
    return m_parts.first();
}

QString FormattedString::toPlain() const {
    QString ret;
    for (const auto& i : m_parts) {
        ret.append(i.text);
    }
    return ret;
}

QString FormattedString::toHtml(const ColorTheme& theme) const {
    QString ret {"<html><body><span style='white-space: pre-wrap;'>"};
    for (const auto& i : m_parts) {
        ret.append(i.toHtml(theme));
    }
    ret.append("</span></body></html>");
    return ret;
}

QString FormattedString::toTrimmedHtml(int n, const ColorTheme& theme) const {
    if (n < 0) {
        return toHtml(theme);
    }
    QString ret = "<html><body><span style='white-space: pre-wrap;'>";
    for (const auto& i : m_parts) {
        QString word = i.text.left(n);
        Part tempPart = i;
        tempPart.text = word;
        ret.append(tempPart.toHtml(theme));
        n -= static_cast<int>(word.size());
        if (n <= 0) {
            break;
        }
    }
    while (n > 0) {
        ret.append("\u00A0");
        n--;
    }
    ret += "</span></body></html>";
    return ret;
}

bool FormattedString::containsHtml() const {
    return m_parts.count() > 1 || m_parts.first().containsHtml();
}

int FormattedString::count() const {
    return static_cast<int>(m_parts.count());
}

FormattedString::Part& FormattedString::lastPart() {
    return m_parts.last();
}

const FormattedString::Part& FormattedString::lastPart() const {
    return m_parts.last();
}

const FormattedString::Part& FormattedString::at(int index) const {
    return m_parts.at(index);
}

void FormattedString::prune() {
    auto it = m_parts.begin();
    while (it != m_parts.end()) {
        // Originally: QRegExp
        // re(R"(((?:(?:https?|ftp|file):\/\/|www\.|ftp\.)(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.]*\)|[-A-Z0-9+&@#\/%=~_|$?!:,.])*(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.]*\)|[A-Z0-9+&@#\/%=~_|$])))",
        // Qt::CaseInsensitive, QRegExp::W3CXmlSchema11); ; was added to handle &amp; escapes right

        auto reIt = urlRegExp->globalMatch(it->text, 0, QRegularExpression::NormalMatch);
        if (reIt.hasNext()) {
            QList<Part> segments;
            int previousEnd = 0;
            while (reIt.hasNext()) {
                auto reMatch = reIt.next();
                Part prefix = {it->text.mid(previousEnd, reMatch.capturedStart() - previousEnd)};
                Part url = {it->text.mid(reMatch.capturedStart(), reMatch.capturedLength())};
                url.hyperlink = true;
                segments.append(prefix);
                segments.append(url);
                previousEnd = static_cast<int>(reMatch.capturedEnd());
            }
            if (previousEnd < it->text.size()) {
                Part suffix = {it->text.mid(previousEnd, it->text.size() - previousEnd)};
                segments.append(suffix);
            }
            it = m_parts.erase(it);
            for (auto& i : segments) {
                it = m_parts.insert(it, i);
                ++it;
            }
        } else {
            ++it;
        }
    }
    while (it != m_parts.end()) {
        if (it->text.isEmpty()) {
            it = m_parts.erase(it);
        } else {
            ++it;
        }
    }
    it = m_parts.begin();
}

QStringList FormattedString::split(const QString& sep) const {
    return toPlain().split(sep);
}

qlonglong FormattedString::toLongLong(bool* ok, int base) const {
    return toPlain().toLongLong(ok, base);
}

QString FormattedString::toLower() const {
    return toPlain().toLower();
}

std::string FormattedString::toStdString() const {
    return toPlain().toStdString();
}

int FormattedString::length() const {
    return static_cast<int>(toPlain().length());
}

QStringList FormattedString::urls() const {
    QStringList ret;
    for (const auto& i : m_parts) {
        if (i.hyperlink) {
            ret.append(i.text);
        }
    }
    return ret;
}

FormattedString& FormattedString::operator+=(const char* s) {
    lastPart().text += s;
    return *this;
}

bool FormattedString::operator!=(const FormattedString& o) const {
    return !operator==(o);
}

bool FormattedString::operator==(const FormattedString& o) const {
    return toPlain() == o.toPlain();
}

FormattedString::operator QString() const {
    return toPlain();
}

FormattedString& FormattedString::operator=(QString&& o) {
    m_parts.clear();
    m_parts.emplace_back(std::move(o));
    return *this;
}

FormattedString& FormattedString::operator=(const QString& o) {
    m_parts = {o};
    return *this;
}
FormattedString& FormattedString::operator=(FormattedString&& o) noexcept {
    m_parts = {std::move(o.m_parts)};
    return *this;
}

FormattedString& FormattedString::operator=(const FormattedString& o) {
    if (this == &o) {
        return *this;
    }
    m_parts = o.m_parts;
    return *this;
}

QColor FormattedString::Part::Color::toQColor(const ColorTheme& theme) const {
    if (index >= 0) {
        if (extended) {
            if (theme.extendedColors().count() > index) {
                return QColor(theme.extendedColors()[index]);
            } else {
                return QColor("pink");
            }
        } else {
            if (theme.weechatColors().count() > index) {
                return QColor(theme.weechatColors()[index]);
            } else {
                return QColor("pink");
            }
        }
    }
    return QColor("pink");
}
