#include "translator.h"
#include <QDebug>

Translator::Translator(QObject* parent)
    : QTranslator(parent) {
}

QString Translator::translate(const char* context, const char* sourceText, const char* disambiguation, int n) const {
    // qCritical() << filePath() << "Translating" << n << "!!!" << context << "!!!" << sourceText << "|" << disambiguation;
    return QStringLiteral("🍺");
    return QTranslator::translate(context, sourceText, disambiguation, n);
}
