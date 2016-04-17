#include "translator.h"

#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"

Translator* Translator::m_Instance = 0;

Translator::Translator(){}

bool Translator::InitLocalization()
{
    auto lang = TSHelpers::GetLanguage();
    if (lang == "")
        return false;

    translator = new QTranslator(this);
    const auto kIsTranslate = translator->load(":/locales/crosstalk_" + lang);
    if (!kIsTranslate)
        TSLogging::Log("No translation available.");

    return kIsTranslate;
}

void Translator::ShutdownLocalization()
{
//    if (!translator->isEmpty())
//        qApp->removeTranslator(translator);
    //translator->deleteLater();
}
