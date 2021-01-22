#include <UIKit/UIKit.h>

#include <QtGui>
#include <QtQuick>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "iostextinput.h"

@interface TextInputDelegate : NSObject <UITextFieldDelegate>
{
    NativeTextInput* m_inputWrapper;
}

- (id) initWithTextInput: (NativeTextInput*) textInput;
- (void) onTextChange;

- (BOOL) textField: (UITextField*) textField
         shouldChangeCharactersInRange:(NSRange) range
         replacementString:(NSString*) string;

- (void) textFieldDidBeginEditing: (UITextField*) textField;
- (void) textFieldDidEndEditing: (UITextField*) textField;
- (BOOL) textFieldShouldBeginEditing: (UITextField*) textField;
- (BOOL) textFieldShouldClear: (UITextField*) textField;
- (BOOL) textFieldShouldEndEditing: (UITextField*) textField;
- (BOOL) textFieldShouldReturn: (UITextField*) textField;

@end

@implementation TextInputDelegate

- (id) initWithTextInput:(NativeTextInput*) textInput
{
    self = [super init];
    if (self) {
        m_inputWrapper = textInput;
    }
    return self;
}

- (void) onTextChange
{
    m_inputWrapper->textChanged();
}

- (BOOL) textField: (UITextField*) textField
         shouldChangeCharactersInRange:(NSRange) range
         replacementString:(NSString*) string
{
    Q_UNUSED(textField);
    Q_UNUSED(range);
    Q_UNUSED(string);
    return YES;
}

- (void) textFieldDidBeginEditing: (UITextField*) textField
{
    Q_UNUSED(textField);
    return;
}

- (void) textFieldDidEndEditing: (UITextField*) textField
{
    Q_UNUSED(textField);
    return;
}

- (BOOL) textFieldShouldBeginEditing: (UITextField*) textField
{
    Q_UNUSED(textField);
    return YES;
}

- (BOOL) textFieldShouldClear: (UITextField*) textField
{
    Q_UNUSED(textField);
    return YES;
}

- (BOOL) textFieldShouldEndEditing: (UITextField*) textField
{
    Q_UNUSED(textField);
    return YES;
}

- (BOOL) textFieldShouldReturn: (UITextField*) textField
{
    qApp->inputMethod()->hide();
    [textField resignFirstResponder];
    return YES;
}

@end

void logViewHierarchy(UIView* view)
{
    NSLog(@"%@", view);
    for (UIView* subview in view.subviews)
    {
        logViewHierarchy(subview);
    }
}

struct NativeTextInputPrivate {
    UITextField* textInput;
    TextInputDelegate* delegate;
};

NativeTextInput::NativeTextInput(QQuickItem *parent)
    : QQuickItem(parent),
      m_data(new NativeTextInputPrivate()),
      m_initialized(false)
{
    setFlag(ItemHasContents, true);

    m_data->textInput = [[UITextField alloc] initWithFrame: CGRectMake(0, 0, 1, 1)];
    [m_data->textInput setReturnKeyType:UIReturnKeyDone];

    m_data->delegate = [[TextInputDelegate alloc] initWithTextInput: this];
    [m_data->textInput addTarget: m_data->delegate
                       action: @selector(onTextChange)
                       forControlEvents: UIControlEventEditingChanged];
    m_data->textInput.delegate = m_data->delegate;

    m_data->textInput.text = QString("Hello NSWorld!").toNSString();

    if (window() && window()->openglContext())
        initTextInput();
    else if (window())
        connect(window(), SIGNAL(sceneGraphInitialized()), this, SLOT(initTextInput()));
    else
        connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(onWindowChanged(QQuickWindow*)));

    onParentChange();
}

NativeTextInput::~NativeTextInput()
{
    [m_data->textInput release];
    [m_data->delegate release];
    delete m_data;
}

QString NativeTextInput::fontName()
{
    return QString::fromNSString(m_data->textInput.font.fontName);
}

void NativeTextInput::setFontName(QString name)
{
    m_data->textInput.font = [UIFont fontWithName: name.toNSString() size: fontPointSize()];
    emit fontChanged();
}

float NativeTextInput::fontPointSize()
{
    return m_data->textInput.font.pointSize;
}

void NativeTextInput::setFontPointSize(float pointSize)
{
    m_data->textInput.font = [m_data->textInput.font fontWithSize: pointSize];
    emit fontChanged();
}

QColor NativeTextInput::color()
{
    double r = 0; double g = 0; double b = 0; double a = 0;
    UIColor* color = m_data->textInput.textColor;
    [color getRed:&r green:&g blue:&b alpha:&a];
    return QColor(r * 255, g * 255, b * 255, a * 255);
}

QColor NativeTextInput::tintColor()
{
    double r = 0; double g = 0; double b = 0; double a = 0;
    UIColor* color = m_data->textInput.tintColor;
    [color getRed:&r green:&g blue:&b alpha:&a];
    return QColor(r * 255, g * 255, b * 255, a * 255);
}

void NativeTextInput::setColor(QColor color)
{
    m_data->textInput.textColor = [UIColor
        colorWithRed: color.red() / 255.0
        green: color.green() / 255.0
        blue: color.blue() / 255.0
        alpha: color.alpha() / 255.0 ];

    emit colorChanged();
}

void NativeTextInput::setTintColor(QColor color)
{
    m_data->textInput.tintColor = [UIColor
        colorWithRed: color.red() / 255.0
        green: color.green() / 255.0
        blue: color.blue() / 255.0
        alpha: color.alpha() / 255.0 ];

    emit tintColorChanged();
}

QString NativeTextInput::text()
{
    return QString::fromNSString(m_data->textInput.text);
}

void NativeTextInput::setText(QString text)
{
    m_data->textInput.text = text.toNSString();
    emit textChanged();
}

NativeTextInput::HAlignment NativeTextInput::horizontalAlignment()
{
    NSTextAlignment alignment = m_data->textInput.textAlignment;

    if (alignment == NSTextAlignmentLeft) {
        return AlignLeft;
    }
    if (alignment == NSTextAlignmentCenter) {
        return AlignHCenter;
    }
    if (alignment == NSTextAlignmentRight) {
        return AlignRight;
    }
    return AlignLeft;
}

void NativeTextInput::setHorizontalAlignment(HAlignment alignment)
{
    if (alignment == AlignLeft) {
        m_data->textInput.textAlignment = NSTextAlignmentLeft;
    }
    if (alignment == AlignHCenter) {
        m_data->textInput.textAlignment = NSTextAlignmentCenter;
    }
    if (alignment == AlignRight) {
        m_data->textInput.textAlignment = NSTextAlignmentRight;
    }

    emit horizontalAlignmentChanged();
}

NativeTextInput::VAlignment NativeTextInput::verticalAlignment()
{
    UIControlContentVerticalAlignment alignment = m_data->textInput.contentVerticalAlignment;

    if (alignment == UIControlContentVerticalAlignmentCenter) {
        return AlignVCenter;
    }
    if (alignment == UIControlContentVerticalAlignmentTop) {
        return AlignTop;
    }
    if (alignment == UIControlContentVerticalAlignmentBottom) {
        return AlignBottom;
    }
    return AlignVCenter;
}

void NativeTextInput::setVerticalAlignment(VAlignment alignment)
{
    if (alignment == AlignTop) {
        m_data->textInput.contentVerticalAlignment = UIControlContentVerticalAlignmentTop;
    }
    if (alignment == AlignVCenter) {
        m_data->textInput.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
    }
    if (alignment == AlignBottom) {
        m_data->textInput.contentVerticalAlignment = UIControlContentVerticalAlignmentBottom;
    }
    emit verticalAlignmentChanged();
}

void NativeTextInput::initTextInput()
{
    m_initialized = true;
    onGeometryChanged();
}

void NativeTextInput::componentComplete()
{
    QQuickItem::componentComplete();
}

void NativeTextInput::onGeometryChanged()
{

    if (!m_initialized) return;

    qreal originX;
    qreal originY;
    switch (this->transformOrigin()) {
    case QQuickItem::TopLeft:
        originX = 0; originY = 0;
        break;
    case QQuickItem::Top:
        originX = this->width() / 2; originY = 0;
        break;
    case QQuickItem::TopRight:
        originX = this->width(); originY = 0;
        break;
    case QQuickItem::Left:
        originX = 0; originY = this->height() / 2;
        break;
    case QQuickItem::Center:
        originX = this->width() / 2; originY = this->height() / 2;
        break;
    case QQuickItem::Right:
        originX = this->width(); originY = this->height() / 2;
        break;
    case QQuickItem::BottomLeft:
        originX = 0; originY = this->height();
        break;
    case QQuickItem::Bottom:
        originX = this->width() / 2; originY = this->height();
        break;
    case QQuickItem::BottomRight:
        originX = this->width(); originY = this->height();
        break;
    }

    QPointF point = this->mapToScene(QPointF(0, 0));
    UITextField* textField = m_data->textInput;
    bool visible = true;
    qreal opacity = 1;
    QQuickItem* parent = this;
    do {
        opacity *= parent->opacity();
        visible = parent->isVisible();
        parent = parent->parentItem();
    } while (parent && visible && opacity != 0);

    textField.alpha = opacity;

    if (!visible || opacity == 0) {
        // Remove it from the view entirely
        UIView *view = static_cast<UIView *>(
                    QGuiApplication::platformNativeInterface()
                    ->nativeResourceForWindow("uiview", window()));
        if ([textField isDescendantOfView: view]) {
            [textField removeFromSuperview];
        }
    }
    else {
        // Put it back into the view
        UIView* view = static_cast<UIView *>(
                    QGuiApplication::platformNativeInterface()
                    ->nativeResourceForWindow("uiview", window()));
        if (![textField isDescendantOfView: view]) {
            [view addSubview: textField];
        }
    }

    CGRect viewFrame = textField.frame;
    viewFrame.origin.x = point.x();
    viewFrame.origin.y = point.y();
    viewFrame.size.width = this->width();
    viewFrame.size.height = this->height();

    textField.frame = viewFrame;

    [textField setNeedsDisplay];
}

void NativeTextInput::onWindowChanged(QQuickWindow* window)
{
    if (window == NULL) return;
    if (window->openglContext())
        initTextInput();
    else
        connect(window, SIGNAL(sceneGraphInitialized()), this, SLOT(initTextInput()));
}

void NativeTextInput::onParentChange()
{
    foreach (QQuickItem* parent, m_visualParents) {
        parent->disconnect(this, SIGNAL(heightChanged()));
        parent->disconnect(this, SIGNAL(opacityChanged()));
        parent->disconnect(this, SIGNAL(rotationChanged()));
        parent->disconnect(this, SIGNAL(scaleChanged()));
        parent->disconnect(this, SIGNAL(transformOriginChanged(TransformOrigin)));
        parent->disconnect(this, SIGNAL(visibleChanged()));
        parent->disconnect(this, SIGNAL(widthChanged()));
        parent->disconnect(this, SIGNAL(xChanged()));
        parent->disconnect(this, SIGNAL(yChanged()));
        parent->disconnect(this, SIGNAL(parentChanged(QQuickItem*)));
    }
    m_visualParents.clear();

    QQuickItem* parent = this;
    do {
        m_visualParents.append(parent);
        connect(parent, SIGNAL(heightChanged()), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(opacityChanged()), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(rotationChanged()), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(scaleChanged()), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(transformOriginChanged(TransformOrigin)), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(visibleChanged()), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(widthChanged()), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(xChanged()), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(yChanged()), this, SLOT(onGeometryChanged()));
        connect(parent, SIGNAL(parentChanged(QQuickItem*)), this, SLOT(onParentChange()));
        parent = parent->parentItem();
    } while (parent != NULL);
}

void NativeTextInput::geometryChanged(const QRectF& newGeometry,
                                      const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    onGeometryChanged();
}
