#ifndef NATIVETEXTINPUT_H
#define NATIVETEXTINPUT_H

#include <QQuickItem>

struct NativeTextInputPrivate;
class NativeTextInput : public QQuickItem
{
    Q_OBJECT

    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)

    Q_PROPERTY(QString fontName READ fontName WRITE setFontName NOTIFY fontChanged)
    Q_PROPERTY(float fontPointSize READ fontPointSize WRITE setFontPointSize NOTIFY fontChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float keyboardHeight READ keyboardHeight NOTIFY keyboardDidShow)

    Q_PROPERTY(QColor iOSTintColor READ tintColor WRITE setTintColor NOTIFY tintColorChanged)

    Q_PROPERTY(HAlignment horizontalAlignment READ horizontalAlignment
                   WRITE setHorizontalAlignment NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(VAlignment verticalAlignment READ verticalAlignment
                    WRITE setVerticalAlignment NOTIFY verticalAlignmentChanged)

public:
    enum HAlignment {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter
    };

    enum VAlignment {
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom,
        AlignVCenter = Qt::AlignVCenter
    };

    NativeTextInput(QQuickItem *parent = 0);
    ~NativeTextInput();

    QString fontName();
    float fontPointSize();
    QColor color();
    QColor tintColor();
    QString text();
    float keyboardHeight();

    HAlignment horizontalAlignment();
    VAlignment verticalAlignment();

    void setText(QString text);
    void setColor(QColor color);
    void setTintColor(QColor color);
    void setHorizontalAlignment(HAlignment alignment);
    void setVerticalAlignment(VAlignment alignment);
    void setFontName(QString fontName);
    void setFontPointSize(float pointSize);

signals:
    void fontChanged();
    void textChanged();
    void colorChanged();
    void tintColorChanged();
    void keyboardDidShow();
    void accepted();

    void horizontalAlignmentChanged();
    void verticalAlignmentChanged();

public slots:
    void onKeyboardDidShow(const QRectF &r);

protected slots:
    void initTextInput();

    void componentComplete();
    void onGeometryChanged();
    void onWindowChanged(QQuickWindow* window);
    void onParentChange();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private:
    NativeTextInputPrivate* m_data;
    bool m_initialized;
    QList<QQuickItem*> m_visualParents;
};

#endif // NATIVETEXTINPUT_H
