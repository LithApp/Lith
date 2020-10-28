#include "lith.h"

#include "protocol.h"
#include "datamodel.h"
#include "weechat.h"

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

QObject *Lith::getObject(pointer_t ptr, const QString &type, pointer_t parent) {
    if (type.isEmpty()) {
        if (m_bufferMap.contains(ptr))
            return m_bufferMap[ptr];
        if (m_lineMap.contains(ptr))
            return m_lineMap[ptr];
        return nullptr;
    }
    else if (type == "buffer") {
        if (!m_bufferMap.contains(ptr)) {
            Buffer *tmp = new Buffer(this, ptr);
            m_bufferMap[ptr] = tmp;
            m_buffers->append(tmp);
            auto lastOpenBuffer = settingsGet()->lastOpenBufferGet();
            if (m_buffers->count() == 1 && lastOpenBuffer < 0)
                emit selectedBufferChanged();
            else if (lastOpenBuffer == m_buffers->count() - 1) {
                selectedBufferIndexSet(lastOpenBuffer);
            }
        }
        return m_bufferMap[ptr];
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

Weechat *Lith::weechat() {
    return m_weechat;
}

Lith::Lith(QObject *parent)
    : QObject(parent)
    , m_weechat(new Weechat(this))
    , m_buffers(QmlObjectList::create<Buffer>())
    , m_proxyBufferList(new ProxyBufferList(this, m_buffers))
{
    connect(settingsGet(), &Settings::passphraseChanged, this, &Lith::hasPassphraseChanged);
}

bool Lith::hasPassphrase() const {
    return !settingsGet()->passphraseGet().isEmpty();
}


void Lith::onMessageReceived(QByteArray &data) {
    //qCritical() << "Message!" << data;
    QDataStream s(&data, QIODevice::ReadOnly);

    // For whatever fucking reason, there sometimes seems to be an extra byte at the start of the message
    // this of course completely fucks parsing of absolutely everything
    // reading the extra byte beforehand may slithly help with this
    char wtf;
    if ((data[0] || data[1] || data[2]) && *(uint8_t*)data.data() != 0xff) {
        s.readRawData(&wtf, 1);
    }

    Protocol::String id;
    Protocol::parse(s, id);

    //qCritical() << "=== ID" << id.d;


    char type[4] = { 0 };
    s.readRawData(type, 3);

    if (QString(type) == "hda") {
        Protocol::HData hda;
        Protocol::parse(s, hda);
    }
    else {
        qCritical() << "onMessageReceived is not handling type: " << type;
    }
}

void Lith::resetData() {
    selectedBufferIndexSet(-1);
    for (int i = 0; i < m_buffers->count(); i++) {
        if (m_buffers->get<Buffer>(i))
            m_buffers->get<Buffer>(i)->deleteLater();
    }
    m_buffers->clear();
    m_bufferMap.clear();
    qCritical() << "=== RESET";
    int lines = 0;
    for (auto i : m_lineMap) {
        if (i) {
            if (!i->parent())
                lines++;
            i->deleteLater();
        }
    }
    m_lineMap.clear();
    qCritical() << "There is" << m_lineMap.count() << "orphan lines";
    int hotlist = 0;
    for (auto i : m_hotList) {
        if (i) {
            if (!i->parent())
                hotlist++;
            i->deleteLater();
        }
    }
    m_hotList.clear();
    qCritical() << "There is" << m_hotList.count() << "hotlist items";
}


ProxyBufferList::ProxyBufferList(QObject *parent, QAbstractListModel *parentModel)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(parentModel);
    setFilterRole(Qt::UserRole);
    connect(this, &ProxyBufferList::filterWordChanged, this, [this](){
        setFilterFixedString(filterWordGet());
    });
}
bool ProxyBufferList::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    auto index = sourceModel()->index(source_row, 0, source_parent);
    QVariant v = sourceModel()->data(index);
    auto b = qvariant_cast<Buffer*>(v);
    if (b) {
        return b->nameGet().toLower().contains(filterWordGet().toLower());
    }
    return false;
}

