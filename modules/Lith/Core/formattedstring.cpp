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

QString FormattedString::Part::toHtml(QStringView fullText, const ColorTheme& theme, qsizetype trim) const {
    QString ret;
    if (n <= 0) {
        return ret;
    }

    if (italic) {
        ret.append(QStringLiteral("<i>"));
    }
    if (bold) {
        ret.append(QStringLiteral("<b>"));
    }
    if (underline) {
        ret.append(QStringLiteral("<u>"));
    }
    if (foreground.index >= 0) {
        ret.append(QStringLiteral("<font color=\""));
        if (foreground.extended) {
            if (theme.extendedColors().count() > foreground.index) {
                ret.append(theme.extendedColors()[foreground.index]);
            } else {
                ret.append(QStringLiteral("pink"));
            }
        } else {
            if (theme.weechatColors().count() > foreground.index) {
                ret.append(theme.weechatColors()[foreground.index]);
            } else {
                ret.append(QStringLiteral("pink"));
            }
        }
        ret.append(QStringLiteral("\">"));
    }
    if (hyperlink) {
        ret.append(QStringLiteral("<a href=\""));
        ret.append(text(fullText, -1));
        ret.append(QStringLiteral("\">"));
    }

    QString finalText;
    const auto urlThreshold = Lith::settingsGet()->shortenLongUrlsThresholdGet();
    const auto urlShortenEnabled = Lith::settingsGet()->shortenLongUrlsGet();
    if (urlThreshold > 0 && hyperlink && n > urlThreshold && urlShortenEnabled) {
        auto url = QUrl(text(fullText, -1).toString());
        auto scheme = url.scheme();
        auto host = url.host();
        auto file = url.fileName();
        auto query = url.query();
        auto path = url.path();

        // If we only have a hostname, we'll use it as is.
        if (path.isEmpty() || path == QStringLiteral("/")) {
            finalText = text(fullText, trim).toString();
        } else {
            // We'll show always show the host and the scheme.
            const auto hostPrefix = scheme + QStringLiteral("://") + host + QStringLiteral("/");
            constexpr auto ellipsis = QLatin1String("\u2026");

            // The threshold is so small that it doesn't even accomodate the hostPrefix. We'll just put the hostPrefix and
            // ellipsis...
            if (hostPrefix.length() >= urlThreshold) {
                finalText = hostPrefix + ellipsis;
            } else {
                // This is a "nice" url with just a hostname and then one path fragment. We'll let these slide, because these tend
                // to look nice even if they're long. Something like https://host.domain/file.extension
                if (path == QStringLiteral("/") + file && !url.hasQuery()) {
                    finalText = text(fullText, trim).toString();
                } else {
                    // Otherwise it's a weird link with multiple path fragments and queries and stuff. We'll just use the host and 10
                    // characters of the path.
                    const auto maxCharsToAppend = urlThreshold - hostPrefix.length();
                    finalText = hostPrefix + ellipsis + path.right(maxCharsToAppend - 1);
                }
            }
        }
    } else {
        finalText = text(fullText, trim).toString();
    }
    ret.append(finalText.toHtmlEscaped());

    if (hyperlink) {
        ret.append(QStringLiteral("</a>"));
    }
    if (foreground.index >= 0) {
        ret.append(QStringLiteral("</font>"));
    }
    if (underline) {
        ret.append(QStringLiteral("</u>"));
    }
    if (bold) {
        ret.append(QStringLiteral("</b>"));
    }
    if (italic) {
        ret.append(QStringLiteral("</i>"));
    }
    return ret;
}

QStringView FormattedString::Part::text(QStringView fullText, qsizetype trim) const {
    if (trim > 0) {
        return fullText.sliced(pos, qMin(n, trim));
    }
    return fullText.sliced(pos, n);
}

FormattedString::FormattedString()
    : m_parts {
          Part {0, 0}
} {
}

FormattedString::FormattedString(QString&& o)
    : m_fullText(std::move(o)) {
    m_parts.emplace_back(0, m_fullText.size());
}

bool FormattedString::operator==(const QString& o) const {
    return toPlain() == o;
}

bool FormattedString::operator!=(const QString& o) const {
    return !operator==(o);
}

FormattedString& FormattedString::addChar(const QByteArray& s) {
    // most characters are ascii, avoid using the utf8 parser
    if (s.size() == 1 && !(s[0] & 0b1000'0000)) {
        m_fullText.append(QChar::fromLatin1(s[0]));
        lastPart().n++;
    } else if (s.size() >= 0) {
        qsizetype previousSize = m_fullText.size();
        m_fullText.append(s);
        qsizetype diff = m_fullText.size() - previousSize;
        lastPart().n += diff;
    }
    return *this;
}

FormattedString::Part& FormattedString::addPart(QStringView s) {
    auto& newPart = addPart();
    newPart.pos = m_fullText.size();
    newPart.n = s.size();
    return newPart;
}

const ColorTheme& FormattedString::getCurrentTheme() {
    return WindowHelper::instance()->currentTheme();
}

void FormattedString::clear() {
    m_parts = {{}};
}

FormattedString::Part& FormattedString::addPart() {
    auto& newPart = m_parts.emplace_back();
    newPart.pos = m_fullText.size();
    newPart.n = 0;
    return newPart;
}

FormattedString::Part& FormattedString::firstPart() {
    return m_parts.first();
}

const FormattedString::Part& FormattedString::firstPart() const {
    return m_parts.first();
}

QString FormattedString::toPlain() const {
    return m_fullText;
}

QString FormattedString::toHtml(const ColorTheme& theme) const {
    QString ret;
    ret.append(QStringLiteral("<span style='white-space: pre-wrap;'>"));
    for (const auto& i : m_parts) {
        ret.append(i.toHtml(m_fullText, theme));
    }
    ret.append("</span>");
    return ret;
}

QString FormattedString::toTrimmedHtml(int n, const ColorTheme& theme) const {
    if (n < 0) {
        return toHtml(theme);
    }
    QString ret;
    ret.append(QStringLiteral("<span style='white-space: pre-wrap;'>"));
    for (const auto& i : m_parts) {
        /* TODO this was weird too
        QString word = i.text.left(n);
        Part tempPart = Part {i};
        tempPart.text = word;
        ret.append(tempPart.toHtml(theme));
        n -= static_cast<int>(word.size());
        */
        ret.append(i.toHtml(m_fullText, theme, n));
        n -= i.n;
        if (n <= 0) {
            break;
        }
    }
    while (n > 0) {
        ret.append(QStringLiteral("\u00A0"));
        n--;
    }
    ret.append("</span>");
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

        auto currentText = it->text(m_fullText, -1);
        if (it->n < 4) {
            // Skip very short strings to avoid executing the expensive RE needlessly
            ++it;

            // TODO: decide if to omit all strings that don't contain a dot, or find a smarter heuristic
            // !it->text(m_fullText, -1).contains(QChar::fromLatin1('.')
        } else {
            auto reIt = urlRegExp->globalMatchView(currentText, 0, QRegularExpression::NormalMatch);
            if (reIt.hasNext()) {
                QList<Part> segments;
                int previousEnd = 0;
                while (reIt.hasNext()) {
                    auto reMatch = reIt.next();
                    Part prefix {previousEnd, reMatch.capturedStart() - previousEnd};
                    Part url = Part {reMatch.capturedStart(), reMatch.capturedLength()};
                    url.hyperlink = true;
                    segments.emplace_back(std::move(prefix));
                    segments.emplace_back(std::move(url));
                    previousEnd = static_cast<int>(reMatch.capturedEnd());
                }
                if (previousEnd < it->n) {
                    Part suffix = Part {previousEnd, it->n - previousEnd};
                    segments.emplace_back(std::move(suffix));
                }
                it = m_parts.erase(it);
                for (auto& i : segments) {
                    it = m_parts.emplace(it, std::move(i));
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }
    while (it != m_parts.end()) {
        if (it->n <= 0) {
            it = m_parts.erase(it);
        } else {
            ++it;
        }
    }
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
    return m_fullText.length();
}

QStringList FormattedString::urls() const {
    QStringList ret;
    for (const auto& i : m_parts) {
        if (i.hyperlink) {
            ret.append(i.text(m_fullText, -1).toString());
        }
    }
    return ret;
}

bool FormattedString::operator!=(const FormattedString& o) const {
    return !operator==(o);
}

bool FormattedString::operator==(const FormattedString& o) const {
    return toPlain() == o.toPlain();
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
