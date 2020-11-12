// Lith
// Copyright (C) 2020 Martin Bříza
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

#include "lith.h"

#include "datamodel.h"
#include "weechat.h"

#include <iostream>
#include <QThread>
#include <QEventLoop>
#include <QAbstractEventDispatcher>

#include <QSystemTrayIcon>

#include <QUrl>

Lith *Lith::_self = nullptr;
Lith *Lith::instance() {
    if (!_self)
        _self = new Lith();
    return _self;
}

ProxyBufferList *Lith::buffers() {
    return m_proxyBufferList;
}

QmlObjectList *Lith::unfilteredBuffers() {
    return m_buffers;
}

Buffer *Lith::selectedBuffer() {
    if (m_selectedBufferIndex >=0 && m_selectedBufferIndex < m_buffers->count())
        return m_buffers->get<Buffer>(m_selectedBufferIndex);
    return nullptr;
}

void Lith::selectedBufferSet(Buffer *b) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto it = m_buffers->get<Buffer>(i);
        if (it && it == b) {
            selectedBufferIndexSet(i);
            return;
        }
    }
    selectedBufferIndexSet(-1);
}

int Lith::selectedBufferIndex() {
    return m_selectedBufferIndex;
}

void Lith::selectedBufferIndexSet(int index) {
    if (m_selectedBufferIndex != index && index < m_buffers->count()) {
        m_selectedBufferIndex = index;
        emit selectedBufferChanged();
        if (selectedBuffer())
            selectedBuffer()->fetchMoreLines();
        if (index >= 0)
            settingsGet()->lastOpenBufferSet(index);
    }
}

NickListFilter *Lith::selectedBufferNicks() {
    return m_selectedBufferNicks;
}

void Lith::switchToBufferNumber(int number) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto b = m_buffers->get<Buffer>(i);
        if (b && b->numberGet() == number) {
            selectedBufferIndexSet(i);
            break;
        }
    }
}

QObject *Lith::getObject(pointer_t ptr, const QString &type, pointer_t parent) {
    if (type.isEmpty()) {
        if (m_bufferMap.contains(ptr))
            return m_bufferMap[ptr];
        if (m_lineMap.contains(ptr))
            return m_lineMap[ptr];
        return nullptr;
    }
    else if (type == "line_data") {/*
        if (m_bufferMap.contains(parent)) {
            //Buffer *buffer = m_bufferMap[parent];
            //return buffer->getLine(ptr);
        }
        else if (parent == 0) { */
            if (!m_lineMap.contains(ptr))
                m_lineMap[ptr] = new BufferLine(nullptr);
            return m_lineMap[ptr];
            /*
        }
        else {
            qCritical() << "Got line information before the buffer was allocated";
        }
        */
    }
    else if (type == "nicklist_item") {
        if (m_bufferMap.contains(parent)) {
            return m_bufferMap[parent]->getNick(ptr);
        }
    }
    else if (type == "hotlist") {
        //qCritical() << ptr;
        if (!m_hotList.contains(ptr))
            m_hotList.insert(ptr, new HotListItem(nullptr));
        return m_hotList[ptr];
    }
    else {
        //qCritical() << "Unknown type of new stuff requested:" << type;
    }
    return nullptr;
}

QString Lith::getLinkFileExtension(const QString &url) {
    QUrl u(url);
    auto extension = u.fileName().split(".").last().toLower();
    return extension;
}

Weechat *Lith::weechat() {
    return m_weechat;
}

Lith::Lith(QObject *parent)
    : QObject(parent)
    , m_weechatThread(new QThread(this))
    , m_weechat(new Weechat(this))
    , m_buffers(QmlObjectList::create<Buffer>())
    , m_proxyBufferList(new ProxyBufferList(this, m_buffers))
    , m_selectedBufferNicks(new NickListFilter(this))
{
    connect(settingsGet(), &Settings::passphraseChanged, this, &Lith::hasPassphraseChanged);
    connect(this, &Lith::selectedBufferChanged, [this](){
        if (selectedBuffer())
            m_selectedBufferNicks->setSourceModel(selectedBuffer()->nicks());
        else
            m_selectedBufferNicks->setSourceModel(nullptr);
    });

    m_weechat->moveToThread(m_weechatThread);
    m_weechatThread->start();
    QTimer::singleShot(1, m_weechat, &Weechat::init);
}

bool Lith::hasPassphrase() const {
    return !settingsGet()->passphraseGet().isEmpty();
}

void Lith::resetData() {
    selectedBufferIndexSet(-1);

    m_buffers->clear();
    m_bufferMap.clear();
    m_lineMap.clear();
    m_hotList.clear();
}

void Lith::handleBufferInitialization(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer
        auto ptr = i.pointers.first();
        auto b = new Buffer(this, ptr);
        for (auto j : i.objects.keys()) {
            b->setProperty(qPrintable(j), i.objects[j]);
        }
        addBuffer(ptr, b);
    }
    delete hda;
}

void Lith::handleFirstReceivedLine(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer - lines - line - line_data
        auto bufPtr = i.pointers.first();
        auto linePtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto line = getLine(bufPtr, linePtr);
        if (line)
            continue;
        line = new BufferLine(buffer);
        for (auto j : i.objects.keys()) {
            if (j != "buffer")
                line->setProperty(qPrintable(j), i.objects[j]);
        }
        buffer->appendLine(line);
        addLine(bufPtr, linePtr, line);
    }
    delete hda;
}

void Lith::handleHotlistInitialization(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // hotlist
        auto ptr = i.pointers.first();
        auto item = new HotListItem(this);
        for (auto j : i.objects.keys()) {
            item->setProperty(qPrintable(j), i.objects[j]);
        }
        addHotlist(ptr, item);
    }
    delete hda;
}

void Lith::handleNicklistInitialization(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Nick missing a parent:";
            continue;
        }
        auto nick = new Nick(buffer);
        for (auto j : i.objects.keys()) {
            nick->setProperty(qPrintable(j), i.objects[j]);
        }
        buffer->addNick(nickPtr, nick);
    }
    delete hda;
}

void Lith::handleFetchLines(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer - lines - line - line_data
        auto bufPtr = i.pointers.first();
        auto linePtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto line = getLine(bufPtr, linePtr);
        if (line)
            continue;
        line = new BufferLine(buffer);
        for (auto j : i.objects.keys()) {
            if (j != "buffer")
                line->setProperty(qPrintable(j), i.objects[j]);
        }
        buffer->appendLine(line);
        addLine(bufPtr, linePtr, line);
    }
    delete hda;
}

void Lith::handleHotlist(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // hotlist
        auto hlPtr = i.pointers.first();
        auto bufPtr = qvariant_cast<pointer_t>(i.objects["buffer"]);
        auto hl = getHotlist(hlPtr);
        auto buf = getBuffer(bufPtr);
        if (!buf) {
            qWarning() << "Got a hotlist item" << QString("%1").arg(hlPtr, 16, 16, QChar('0')) <<  "for nonexistent buffer" << QString("%1").arg(bufPtr, 16, 16, QChar('0'));
            continue;
        }
        if (!hl) {
            hl = new HotListItem(this);
            hl->bufferSet(buf);
        }
        for (auto j : i.objects.keys()) {
            if (j == "buffer")
                continue;
            hl->setProperty(qPrintable(j), i.objects[j]);
        }
    }
    delete hda;
}

void Lith::_buffer_opened(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buffer = getBuffer(bufPtr);
        if (buffer)
            continue;
        buffer = new Buffer(this, bufPtr);
        for (auto j : i.objects.keys()) {
            buffer->setProperty(qPrintable(j), i.objects[j]);
        }
        addBuffer(bufPtr, buffer);
    }
    delete hda;
}

void Lith::_buffer_type_changed(Protocol::HData *hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    delete hda;
}

void Lith::_buffer_moved(Protocol::HData *hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    delete hda;
}

void Lith::_buffer_merged(Protocol::HData *hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    delete hda;
}

void Lith::_buffer_unmerged(Protocol::HData *hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    delete hda;
}

void Lith::_buffer_hidden(Protocol::HData *hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    delete hda;
}

void Lith::_buffer_unhidden(Protocol::HData *hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    delete hda;
}

void Lith::_buffer_renamed(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buf = getBuffer(bufPtr);
        if (!buf)
            continue;
        for (auto j : i.objects.keys()) {
            if (j.endsWith("name")) {
                buf->setProperty(qPrintable(j), i.objects[j]);
            }
        }
    }
    delete hda;
}

void Lith::_buffer_title_changed(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buf = getBuffer(bufPtr);
        if (!buf)
            continue;
        buf->titleSet(i.objects["title"].toString());
    }
    delete hda;
}

void Lith::_buffer_localvar_added(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buf = getBuffer(bufPtr);
        if (!buf)
            continue;
        auto strm = i.objects["local_variables"].value<StringMap>();
        buf->local_variablesSet(strm);
    }
    delete hda;
}

void Lith::_buffer_localvar_changed(Protocol::HData *hda) {
    // These three seem to be the same
    _buffer_localvar_added(hda);
}

void Lith::_buffer_localvar_removed(Protocol::HData *hda) {
    // These three seem to be the same
    _buffer_localvar_added(hda);
}

void Lith::_buffer_closing(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buffer = getBuffer(bufPtr);
        if (!buffer)
            continue;

        buffer->deleteLater();
        removeBuffer(bufPtr);
    }
    delete hda;
}

void Lith::_buffer_cleared(Protocol::HData *hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    std::cerr << hda->toString().toStdString() << std::endl;
    delete hda;
}

void Lith::_buffer_line_added(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // line_data
        auto linePtr = i.pointers.last();
        // path doesn't contain the buffer, we need to retrieve it like this
        auto bufPtr = qvariant_cast<pointer_t>(i.objects["buffer"]);
        auto buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto line = getLine(bufPtr, linePtr);
        if (line) {
            continue;
        }
        line = new BufferLine(buffer);
        for (auto j : i.objects.keys()) {
            if (j == "buffer")
                continue;
            line->setProperty(qPrintable(j), i.objects[j]);
        }
        buffer->prependLine(line);
        addLine(bufPtr, linePtr, line);
        if (line->highlightGet()) {
            qCritical() << "JSEM TU FFS";
            QSystemTrayIcon *icon = new QSystemTrayIcon();
            icon->show();
            icon->showMessage("LITH AF", line->messageGet());
            icon->hide();
            icon->deleteLater();
        }
    }
    delete hda;
}

void Lith::_nicklist(Protocol::HData *hda) {
    Buffer *previousBuffer = nullptr;
    for (auto &i : hda->data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer)
            continue;
        if (buffer != previousBuffer)
            buffer->clearNicks();
        previousBuffer = buffer;
        auto nick = new Nick(buffer);
        for (auto j : i.objects.keys()) {
            nick->setProperty(qPrintable(j), i.objects[j]);
        }
        buffer->addNick(nickPtr, nick);
    }
    delete hda;
}

void Lith::_nicklist_diff(Protocol::HData *hda) {
    for (auto &i : hda->data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer)
            continue;
        auto op = qvariant_cast<char>(i.objects["_diff"]);
        switch (op) {
        case '+': {
            auto nick = new Nick(buffer);
            for (auto j : i.objects.keys()) {
                if (j == "_diff")
                    continue;
                nick->setProperty(qPrintable(j), i.objects[j]);
            }
            buffer->addNick(nickPtr, nick);
            break;
        }
        case '-': {
            buffer->removeNick(nickPtr);
            break;
        }
        case '^':
        case '*': {
            auto nick = buffer->getNick(nickPtr);
            if (!nick)
                break;
            for (auto j : i.objects.keys()) {
                if (j == "_diff")
                    continue;
                nick->setProperty(qPrintable(j), i.objects[j]);
            }
            break;
        }
        default:
            break;
        }

    }
    delete hda;
}

void Lith::addBuffer(pointer_t ptr, Buffer *b) {
    m_bufferMap[ptr] = b;
    m_buffers->append(b);
    auto lastOpenBuffer = settingsGet()->lastOpenBufferGet();
    if (m_buffers->count() == 1 && lastOpenBuffer < 0)
        emit selectedBufferChanged();
    else if (lastOpenBuffer == m_buffers->count() - 1) {
        selectedBufferIndexSet(lastOpenBuffer);
    }
}

void Lith::removeBuffer(pointer_t ptr) {
    if (m_bufferMap.contains(ptr)) {
        auto buf = m_bufferMap[ptr];
        if (selectedBuffer() == buf)
            selectedBufferIndexSet(selectedBufferIndex() - 1);
        m_bufferMap.remove(ptr);
        m_buffers->removeItem(buf);
    }
}

Buffer *Lith::getBuffer(pointer_t ptr) {
    if (m_bufferMap.contains(ptr))
        return m_bufferMap[ptr];
    return nullptr;
}

void Lith::addLine(pointer_t bufPtr, pointer_t linePtr, BufferLine *line) {
    auto ptr = bufPtr << 32 | linePtr;
    if (m_lineMap.contains(ptr)) {
        // TODO
        qCritical() << "Line with ptr" << QString("%1").arg(ptr, 16, 16, QChar('0')) << "already exists";
        qCritical() << "Original: " << m_lineMap[ptr]->messageGet();
        qCritical() << "New:" << line->messageGet();
    }
    m_lineMap[ptr] = line;
}

BufferLine *Lith::getLine(pointer_t bufPtr, pointer_t linePtr) {
    auto ptr = bufPtr << 32 | linePtr;
    if (m_lineMap.contains(ptr)) {
        return m_lineMap[ptr];
    }
    return nullptr;
}

void Lith::addHotlist(pointer_t ptr, HotListItem *hotlist) {
    if (m_hotList.contains(ptr)) {
        // TODO
        qCritical() << "Hotlist with ptr" << QString("%1").arg(ptr, 8, 16, QChar('0')) << "already exists";
    }
    m_hotList[ptr] = hotlist;
}

HotListItem *Lith::getHotlist(pointer_t ptr) {
    if (m_hotList.contains(ptr))
        return m_hotList[ptr];
    return nullptr;
}


ProxyBufferList::ProxyBufferList(QObject *parent, QAbstractListModel *parentModel)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(parentModel);
    setFilterRole(Qt::UserRole);
    connect(this, &ProxyBufferList::filterWordChanged, [this] {
        setFilterFixedString(filterWordGet());
    });
}
bool ProxyBufferList::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    auto index = sourceModel()->index(source_row, 0, source_parent);
    auto v = sourceModel()->data(index);
    auto b = qvariant_cast<Buffer*>(v);
    if (b) {
        return b->nameGet().toLower().contains(filterWordGet().toLower());
    }
    return false;
}

