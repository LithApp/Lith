#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QTranslator>

class Translator : public QTranslator {
    Q_OBJECT
public:
    Translator(QObject* parent = nullptr);
    virtual QString translate(const char* context, const char* sourceText, const char* disambiguation = nullptr, int n = -1) const;
};

#endif  // TRANSLATOR_H
