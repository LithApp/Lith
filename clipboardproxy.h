#ifndef CLIPBOARDPROXY_H
#define CLIPBOARDPROXY_H

#include <QObject>
#include <QClipboard>

class ClipboardProxy : public QObject {
    Q_OBJECT
public:
    ClipboardProxy(QObject *parent = nullptr);

    Q_INVOKABLE bool hasImage();

    Q_INVOKABLE QString text();

private:
    QClipboard *m_clipboard;
};

#endif // CLIPBOARDPROXY_H
