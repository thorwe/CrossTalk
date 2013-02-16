#include "tsfunctions.h"

#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"

//#define PLUGIN_THREAD_TIMEOUT 1000

TSFunctions* TSFunctions::m_Instance = 0;

TSFunctions::TSFunctions()
{
//    command_mutex = new QMutex();
}
TSFunctions::~TSFunctions(){}

bool TSFunctions::InitLocalization()
{
    QString lang = TSHelpers::GetLanguage();
    if (lang == "")
        return false;

    //QTranslator translator;
    translator = new QTranslator(this);
    bool isTranslate;
    isTranslate = translator->load(":/locales/crosstalk_" + lang);
    TSLogging::Print(QString("Have translation: %1").arg((isTranslate)?"true":"false"));

//    if (isTranslate)
//        qApp->installTranslator(translator);
    // this will crash the client when disabling the plugin in any way I tried.
    // one could probably copy the crosstalk_xx_YY.qm to the translations dir on install, however
    // I decided to leave it embedded for now and manually bulk apply the translations
    // when the config window is opened

    return isTranslate;
}

void TSFunctions::ShutdownLocalization()
{
//    if (!translator->isEmpty())
//        qApp->removeTranslator(translator);
    //translator->deleteLater();
}

// Mixed
/*! Instead of directly asking the client, this function returns the cached value; useful for OnClientMove when disconnecting, when you cannot compare the move clientID to yours since you already cannot get your ID anymore
 * \brief TSFunctions::GetClientId
 * \param serverConnectionHandlerID
 * \param result
 * \return either ERROR_ok or ERROR_not_connected
 */
unsigned int TSFunctions::GetClientId(uint64 serverConnectionHandlerID, anyID* result)
{
    if (this->myClientIDs.contains(serverConnectionHandlerID))
    {
        *result = this->myClientIDs.value(serverConnectionHandlerID);
        return ERROR_ok;
    }
    return ERROR_not_connected;
}

//! Collects self-clientIds so that they are checkable on disconnect (there'll be a client move event with your id on disconnect, however the API doesn't give you your id anymore at this point
/*!
 * \brief TSFunctions::onConnectStatusChangeEvent equivalent TS Client SDK
 * \param serverConnectionHandlerID equivalent TS Client SDK
 * \param newStatus equivalent TS Client SDK
 * \param errorNumber equivalent TS Client SDK
 */
void TSFunctions::onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    anyID result;
    if (newStatus == STATUS_CONNECTED)
    {
        unsigned int error;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&result)) != ERROR_ok)
            TSLogging::Error("Error getting client id:",serverConnectionHandlerID,error);

        this->myClientIDs.insert(serverConnectionHandlerID,result);
    }
    else if (newStatus == STATUS_DISCONNECTED)
    {
        this->myClientIDs.remove(serverConnectionHandlerID);
    }
}

////! Retrieves plugin commands, distributes them to the modules
///*!
// * \brief TSFunctions::ParseCommand Retrieves plugin commands, distributes them to the modules
// * \param serverConnectionHandlerID the server connection handler passed from the API
// * \param cmd the command
// * \param arg arguments, if any
// * \return the error code that aborted the function or ERROR_ok
// */
//int TSFunctions::ParseCommand(uint64 serverConnectionHandlerID, const char* cmd)
//{
//    QString cmd_qs;
//    cmd_qs = cmd;
//    QStringList args_qs = cmd_qs.split(" ",QString::SkipEmptyParts);
//    if (args_qs.isEmpty())  // this might evolve to some help output
//        return 1;

//    if (!(command_mutex->tryLock(PLUGIN_THREAD_TIMEOUT)))
//    {
//        TSLogging::Log("Timeout while waiting for mutex",serverConnectionHandlerID,LogLevel_WARNING);
//        return 1;
//    }

//    cmd_qs = args_qs.takeFirst();
//    emit Command(serverConnectionHandlerID,cmd_qs,args_qs); //ToDo: Not too happy using Signal/Slots here

//    command_mutex->unlock();
//    return 0;
//}
