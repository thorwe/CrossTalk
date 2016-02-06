#include "translator.h"

#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"

Translator* Translator::m_Instance = 0;

Translator::Translator(){}

bool Translator::InitLocalization()
{
    QString lang = TSHelpers::GetLanguage();
    if (lang == "")
        return false;

    translator = new QTranslator(this);
    bool isTranslate;
    isTranslate = translator->load(":/locales/crosstalk_" + lang);
    if (!isTranslate)
        TSLogging::Log("No translation available.");

    return isTranslate;
}

void Translator::ShutdownLocalization()
{
//    if (!translator->isEmpty())
//        qApp->removeTranslator(translator);
    //translator->deleteLater();
}
