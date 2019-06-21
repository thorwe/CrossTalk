#include "ts_ptt_qt.h"

#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"

#include "plugin.h"

#include "core/ts_settings_qt.h"
#include "core/ts_logging_qt.h"
#include "core/ts_helpers_qt.h"
#include "ts3_functions.h"

#define PLUGIN_THREAD_TIMEOUT 1000

TSPtt* TSPtt::m_Instance = 0;

TSPtt::TSPtt(){}

TSPtt::~TSPtt()
{
    if(timer)
        timer->stop();

    delete timer;
}

void TSPtt::Init(QMutex *mutex_cmd)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TSPtt::onPttDelayFinished);
    timer->setSingleShot(true);
    command_mutex = mutex_cmd;
    this->setObjectName("TSPtt");
}

int TSPtt::SetPushToTalk(uint64 serverConnectionHandlerID, PTT_CHANGE_STATUS action)
{
    if (action == PTT_ACTIVATE)
    {
        if (timer->isActive())
            timer->stop();

        return SetPushToTalk(serverConnectionHandlerID, true);
    }
    else if (action == PTT_DEACTIVATE)
    {
        UpdatePttDelayInfo();
        if (pttDelayEnabled)
            timer->start(pttDelayMsec);
        else
            return SetPushToTalk(serverConnectionHandlerID, false);
    }
    else if (action == PTT_TOGGLE)  // Toggling is always instant
    {
        if (pttActive && timer->isActive())
            timer->stop();

        return SetPushToTalk(serverConnectionHandlerID, !pttActive);
    }
    else    //use NULL for Toggle?
    {
        TSLogging::Error("Ptt Status Error",serverConnectionHandlerID,NULL);
    }

    return 1;
}

int TSPtt::SetPushToTalk(uint64 serverConnectionHandlerID, bool shouldTalk)
{
    unsigned int error;

    // If PTT is inactive, store the current settings
    if(!pttActive)
    {
        // Get the current VAD setting
        char* vad;
        if((error = ts3Functions.getPreProcessorConfigValue(serverConnectionHandlerID, "vad", &vad)) != ERROR_ok)
        {
            TSLogging::Error("Error retrieving vad setting",serverConnectionHandlerID,error);
            return 1;
        }
        vadActive = !strcmp(vad, "true");
        ts3Functions.freeMemory(vad);

        // Get the current input setting, this will indicate whether VAD is being used in combination with PTT
        int input;
        if((error = ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerID, CLIENT_INPUT_DEACTIVATED, &input)) != ERROR_ok)
        {
            TSLogging::Error("Error retrieving input setting",serverConnectionHandlerID,error);
            return 1;
        }
        inputActive = !input; // We want to know when it is active, not when it is inactive
    }

    // If VAD is active and the input is active, disable VAD, restore VAD setting afterwards
    if((error = ts3Functions.setPreProcessorConfigValue(serverConnectionHandlerID, "vad",
        (shouldTalk && (vadActive && inputActive)) ? "false" : (vadActive)?"true":"false")) != ERROR_ok)
    {
        TSLogging::Error("Error toggling vad",serverConnectionHandlerID,error);
        return 1;
    }

    // Activate the input, restore the input setting afterwards
    if((error = ts3Functions.setClientSelfVariableAsInt(serverConnectionHandlerID, CLIENT_INPUT_DEACTIVATED,
        (shouldTalk || inputActive) ? INPUT_ACTIVE : INPUT_DEACTIVATED)) != ERROR_ok)
    {
        TSLogging::Error("Error toggling input",serverConnectionHandlerID,error);
        return 1;
    }

    // Update the client
    ts3Functions.flushClientSelfUpdates(serverConnectionHandlerID, NULL);

    // Commit the change
    pttActive = shouldTalk;

    return 0;
}

void TSPtt::UpdatePttDelayInfo()
{
    // Get default capture profile and preprocessor data
    char** profiles;
    int defaultProfile;
    unsigned int error;
    if((error = ts3Functions.getProfileList(PLUGIN_GUI_SOUND_CAPTURE, &defaultProfile, &profiles)) != ERROR_ok)
    {
        TSLogging::Error("Error retrieving capture profiles",error);
        return;
    }
    QString profile = profiles[defaultProfile];
    ts3Functions.freeMemory(profiles);

    QString preProcessorData;
    if (!(TSSettings::instance()->GetPreProcessorData(profile,preProcessorData)))
    {
        TSLogging::Error("(TSPtt::UpdatePttDelayInfo()) (GetPreProcessorData) (" + profile + ")" + TSSettings::instance()->GetLastError().text());
        return;
    }

    auto lines = preProcessorData.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    QStringListIterator iterator(lines);
    while (iterator.hasNext())
    {
        QString str_qstr = iterator.next();
        if (str_qstr.startsWith("delay_ptt_msecs"))
        {
            str_qstr.remove("delay_ptt_msecs=");
            bool ok;
            int msec = str_qstr.toInt(&ok);
            if (!ok)
                TSLogging::Error("Error retrieving delay_ptt_msecs");
            else
                pttDelayMsec = msec;
        }
        else if (str_qstr.startsWith("delay_ptt"))
        {
            pttDelayEnabled = (str_qstr == "delay_ptt=true");
        }
    }
}

//! Emits PttDelayFinished when the timer runs out
/*!
 * \brief TSPtt::onPttDelayFinished Qt Slot
 */
void TSPtt::onPttDelayFinished()
{
    command_mutex->tryLock(PLUGIN_THREAD_TIMEOUT);
    emit PttDelayFinished();
    command_mutex->unlock();
}
