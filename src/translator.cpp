#include "translator.h"

#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"

Translator* Translator::m_Instance = 0;

Translator::Translator(){}
Translator::~Translator(){}

bool Translator::InitLocalization()
{
    QString lang = TSHelpers::GetLanguage();
    if (lang == "")
        return false;

    translator = new QTranslator(this);
    bool isTranslate;
    isTranslate = translator->load(":/locales/crosstalk_" + lang);
//    TSLogging::Log(QString("Have translation: %1").arg((isTranslate)?"true":"false"),LogLevel_DEBUG);

//    if (isTranslate)
//        qApp->installTranslator(translator);
    // this will crash the client when disabling the plugin in any way I tried.
    // one could probably copy the crosstalk_xx_YY.qm to the translations dir on install, however
    // I decided to leave it embedded for now and manually bulk apply the translations
    // when the config window is opened
    return isTranslate;
}

void Translator::ShutdownLocalization()
{
//    if (!translator->isEmpty())
//        qApp->removeTranslator(translator);
    //translator->deleteLater();
}
