#include "tsfunctions.h"

#include "plugin.h"
#include <QTime>
#include <QTextStream>
#include <QtSql>
#include <QtDebug>

#define PLUGIN_THREAD_TIMEOUT 1000

TSFunctions* TSFunctions::m_Instance = 0;

TSFunctions::TSFunctions() :
    pttActive(false),
    vadActive(false),
    inputActive(false),
    pttDelayEnabled(false),
    pttDelayMsec(0)
{
    //myClientIDs = new QMap<uint64,anyID>;
    command_mutex = new QMutex();
}

TSFunctions::~TSFunctions()
{
    if(timer)
        timer->stop();

    delete timer;
}


// Error, Debug etc.

bool TSFunctions::GetErrorSound(QString &in)
{
    QString pack;
    if (!(Settings.GetSoundPack(pack)))
    {
        return false;
    }
    else
    {
        // Find the path to the soundpack
        char path[PATH_BUFSIZE];
        ts3Functions.getResourcesPath(path, PATH_BUFSIZE);
        QString path_qstr(path);
        path_qstr.append("sound/" + pack);

        QSettings cfg(path_qstr + "/settings.ini", QSettings::IniFormat);
        QString snd_qstr = cfg.value("soundfiles/SERVER_ERROR").toString();
        if (snd_qstr.isEmpty() != true)
        {
            // towatch: QSettings insists on eliminating the double quotas '\"' on read
            // no, I won't spend one more minute creating a regexp that fits for a fragging error sound that should be available via the api.
            snd_qstr.remove("play(");
            snd_qstr.remove(")");

            path_qstr.append("/" + snd_qstr);
            in = path_qstr;
        }
        return true; // Here so that the user setting of "No Sound" (and speech synthesis? don't throw errors)
    }
}

bool TSFunctions::GetInfoIcon(QString &in)
{
    QString pack;
    if (!(Settings.GetIconPack(pack)))
    {
        return false;
    }
    else
    {
        // Find the path to the skin
        char path[PATH_BUFSIZE];
        ts3Functions.getResourcesPath(path, PATH_BUFSIZE);
        QString path_qstr(path);
        in = (path_qstr + "gfx/" + pack + "/16x16_message_info.png");
        return true;
    }
}

void TSFunctions::PlayErrorSound(uint64 serverConnectionHandlerID)
{
    QString errorSound;
    if (!(GetErrorSound(errorSound)))
    {
        //Get Error
        QString msg = "TSFunctions::PlayErrorSound:" + Settings.GetLastError().text();
        ts3Functions.logMessage(msg.toLocal8Bit().constData(), LogLevel_WARNING, PLUGIN_NAME, 0);
    }
    else
    {
        if((errorSound.isEmpty()) != true)
        {
            unsigned int error;
            // Play the error sound
            if((error = ts3Functions.playWaveFile(serverConnectionHandlerID, errorSound.toLocal8Bit().constData())) != ERROR_ok)
            {
                char* errorMsg;
                if(ts3Functions.getErrorMessage(error, &errorMsg) == ERROR_ok)
                {
                    ts3Functions.logMessage("Error playing error sound:", LogLevel_WARNING, PLUGIN_NAME, 0);
                    ts3Functions.logMessage(errorMsg, LogLevel_WARNING, PLUGIN_NAME, 0);
                    ts3Functions.freeMemory(errorMsg);
                }
            }
        }
    }
}

void TSFunctions::PlayErrorSound()
{
    PlayErrorSound(GetActiveServerConnectionHandlerID());
}

void TSFunctions::Error(uint64 serverConnectionHandlerID, unsigned int error, char* message)
{

    QString message_qstr(message);

    if (error != NULL)
    {
        char* errorMsg;
        if(ts3Functions.getErrorMessage(error, &errorMsg) == ERROR_ok)
        {
            QTextStream(&message_qstr) << ": " << errorMsg;
            ts3Functions.freeMemory(errorMsg);
        }
    }

    Log(message_qstr,LogLevel_WARNING);

    if ((serverConnectionHandlerID == NULL) || (serverConnectionHandlerID == 0))
    {
        PlayErrorSound();
        return;
    }
    PlayErrorSound(serverConnectionHandlerID);

    // Format and print the error message
    // Use a transparent underscore because a double space will be collapsed
    QTime time = QTime::currentTime ();
    QString time_qstr = time.toString(Qt::TextDate);

    QString infoIcon;
    if (!(GetInfoIcon(infoIcon)))
    {
        //Get Error
        QString msg = "TSFunctions::Error:GetInfoIcon:" + Settings.GetLastError().text();
        ts3Functions.logMessage(msg.toLocal8Bit().constData(), LogLevel_WARNING, PLUGIN_NAME, 0);
    }

    QString styledQstr;
    QTextStream(&styledQstr) << "[img]" << infoIcon << "[/img][color=red]" << time_qstr << "[/color][color=transparent]_[/color]" << ": "  << PLUGIN_NAME << ": " << message_qstr;
    ts3Functions.printMessage(serverConnectionHandlerID, styledQstr.toLocal8Bit().constData(), PLUGIN_MESSAGE_TARGET_SERVER);
}

void TSFunctions::Print(QString message, LogLevel logLevel)
{
    if (!(CONSOLE_OUTPUT))
        return;

    switch (logLevel)
    {
    case LogLevel_CRITICAL:
        message.prepend("Critical: ");
        break;
    case LogLevel_DEBUG:
        message.prepend("Debug: ");
        break;
    case LogLevel_DEVEL:
        message.prepend("Devel: ");
        break;
    case LogLevel_ERROR:
        message.prepend("Error: ");
        break;
    case LogLevel_INFO:
        message.prepend("Info: ");
        break;
    case LogLevel_WARNING:
        message.prepend("Warning: ");
        break;
    }
    QTime time = QTime::currentTime ();
    QString time_qstr = time.toString(Qt::TextDate);
    QString styledQstr;
    QTextStream(&styledQstr) << time_qstr << ": " << PLUGIN_NAME << ": " << message << "\n";
    printf(styledQstr.toLocal8Bit().constData());
}

void TSFunctions::Print(QString message)
{
    Print(message, LogLevel_INFO);
}

void TSFunctions::Log(QString message, LogLevel logLevel)
{
    ts3Functions.logMessage(message.toLocal8Bit().constData(), logLevel, PLUGIN_NAME, 0);
    Print(message,logLevel);
}

void TSFunctions::Log(QString message)
{
    Log(message, LogLevel_INFO);
}

QString TSFunctions::GetConfigPath()
{
    // Find config path for config class
    char* configPath = (char*)malloc(PATH_BUFSIZE);
    ts3Functions.getConfigPath(configPath, PATH_BUFSIZE);

    return configPath;
}

QString TSFunctions::GetFullConfigPath()
{
    QString fullPath=GetConfigPath();
    fullPath.append(PLUGIN_NAME);
    fullPath.append(".ini");
    return fullPath;
}

QString TSFunctions::GetLanguage()
{
    QString lang = "";
    if (!Settings.GetLanguage(lang))
    {
        //Get Error
        QString msg = "(TSFunctions::GetLanguage())" + Settings.GetLastError().text();
        Error(NULL,NULL,msg.toLocal8Bit().data());
    }
    return lang;
}

bool TSFunctions::InitLocalization()
{
    QString lang = GetLanguage();
    if (lang == "")
        return false;
    //Print(lang);
    //QTranslator translator;
    translator = new QTranslator(this);
    bool isTranslate;
    isTranslate = translator->load(":/locales/crosstalk_" + lang);
    Print(QString("Have translation: %1").arg((isTranslate)?"true":"false"));

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

//Getters
uint64 TSFunctions::GetActiveServerConnectionHandlerID()
{
    unsigned int error;
    uint64* servers;
    uint64* server;
    uint64 handle = NULL;

    if((error = ts3Functions.getServerConnectionHandlerList(&servers)) != ERROR_ok)
    {
        Error(NULL,error,"Error retrieving list of servers:");
        return NULL;
    }

    // Find the first server that matches the criteria
    for(server = servers; *server != (uint64)NULL && handle == NULL; server++)
    {
        int result;
        if((error = ts3Functions.getClientSelfVariableAsInt(*server, CLIENT_INPUT_HARDWARE, &result)) != ERROR_ok)
        {
            Error(*server,error,"Error retrieving client variable:");
        }
        else if(result)
        {
            handle = *server;
        }
    }

    ts3Functions.freeMemory(servers);
    return handle;
}

// Server interaction
int TSFunctions::SetActiveServer(uint64 serverConnectionHandlerID)
{
    unsigned int error;

    if((error = ts3Functions.activateCaptureDevice(serverConnectionHandlerID)) != ERROR_ok)
    {
        Error(serverConnectionHandlerID,error,"Error activating server:");
        return 1;
    }

    return 0;
}

int TSFunctions::SetActiveServerRelative(uint64 serverConnectionHandlerID, bool next)
{
    unsigned int error;

    uint64 server;
    if ((error = GetActiveServerRelative(serverConnectionHandlerID,next,&server)) != ERROR_ok)
        return 1;

    // Check if already active
    int result;
    if((error = ts3Functions.getClientSelfVariableAsInt(server, CLIENT_INPUT_HARDWARE, &result)) != ERROR_ok)
    {
        Error(serverConnectionHandlerID,error,"Error retrieving client variable:");
        return 1;
    }
    if(!result) SetActiveServer(server);

    return 0;
}

unsigned int TSFunctions::GetActiveServerRelative(uint64 serverConnectionHandlerID, bool next, uint64* result)
{
    unsigned int error;
    uint64* servers;
    uint64* server;

    // Get server list
    if((error = ts3Functions.getServerConnectionHandlerList(&servers)) != ERROR_ok)
        return error;

    // Find active server in the list
    for(server = servers; *server != (uint64)NULL && *server!=serverConnectionHandlerID; server++);

    // Find the server in the direction given
    if(next)
    {
        if(*(server+1) != NULL) server++;
        else server = servers; // Wrap around to first server
    }
    else
    {
        if(server != servers) server--;
        else
        {
            for(server = servers; *server != (uint64)NULL; server++);
            server--;
        }
    }
    *result = *server;
    ts3Functions.freeMemory(servers);
    return ERROR_ok;
}

// Communication
int TSFunctions::SetPushToTalk(uint64 serverConnectionHandlerID, PTT_CHANGE_STATUS action)
{

    if (action == PTT_ACTIVATE)
    {
        //Error(serverConnectionHandlerID,NULL,"Ptt Status Activate\n");
        timer->stop();
        return SetPushToTalk(serverConnectionHandlerID, true);
    }
    else if (action == PTT_DEACTIVATE)
    {
        //Error(serverConnectionHandlerID,NULL,"Ptt Status Deactivate\n");
        UpdatePttDelayInfo();
        if (pttDelayEnabled)
        {
            timer->start(pttDelayMsec);
        }
        else
        {
            return SetPushToTalk(serverConnectionHandlerID, false);
        }
    }
    else if (action == PTT_TOGGLE)  // Toggling is always instant
    {
        Error(serverConnectionHandlerID,NULL,"Ptt Status Toggle\n");
        if (pttActive) timer->stop();
        return SetPushToTalk(serverConnectionHandlerID, !pttActive);
    }
    else    //use NULL for Toggle
    {
        Error(serverConnectionHandlerID,NULL,"Ptt Status Error\n");
    }

    return 1;
}

int TSFunctions::SetPushToTalk(uint64 serverConnectionHandlerID, bool shouldTalk)
{
    unsigned int error;

    // If PTT is inactive, store the current settings
    if(!pttActive)
    {
        // Get the current VAD setting
        char* vad;
        if((error = ts3Functions.getPreProcessorConfigValue(serverConnectionHandlerID, "vad", &vad)) != ERROR_ok)
        {
            char* errorMsg;
            if(ts3Functions.getErrorMessage(error, &errorMsg) == ERROR_ok)
            {
                ts3Functions.logMessage("Error retrieving vad setting:", LogLevel_WARNING, PLUGIN_NAME, 0);
                ts3Functions.logMessage(errorMsg, LogLevel_WARNING, PLUGIN_NAME, 0);
                ts3Functions.freeMemory(errorMsg);
            }
            return 1;
        }
        vadActive = !strcmp(vad, "true");
        ts3Functions.freeMemory(vad);

        // Get the current input setting, this will indicate whether VAD is being used in combination with PTT
        int input;
        if((error = ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerID, CLIENT_INPUT_DEACTIVATED, &input)) != ERROR_ok)
        {
            char* errorMsg;
            if(ts3Functions.getErrorMessage(error, &errorMsg) == ERROR_ok)
            {
                ts3Functions.logMessage("Error retrieving input setting:", LogLevel_WARNING, PLUGIN_NAME, 0);
                ts3Functions.logMessage(errorMsg, LogLevel_WARNING, PLUGIN_NAME, 0);
                ts3Functions.freeMemory(errorMsg);
            }
            return 1;
        }
        inputActive = !input; // We want to know when it is active, not when it is inactive
    }

    // If VAD is active and the input is active, disable VAD, restore VAD setting afterwards
    if((error = ts3Functions.setPreProcessorConfigValue(serverConnectionHandlerID, "vad",
        (shouldTalk && (vadActive && inputActive)) ? "false" : (vadActive)?"true":"false")) != ERROR_ok)
    {
        char* errorMsg;
        if(ts3Functions.getErrorMessage(error, &errorMsg) == ERROR_ok)
        {
            ts3Functions.logMessage("Error toggling vad:", LogLevel_WARNING, PLUGIN_NAME, 0);
            ts3Functions.logMessage(errorMsg, LogLevel_WARNING, PLUGIN_NAME, 0);
            ts3Functions.freeMemory(errorMsg);
        }
        return 1;
    }

    // Activate the input, restore the input setting afterwards
    if((error = ts3Functions.setClientSelfVariableAsInt(serverConnectionHandlerID, CLIENT_INPUT_DEACTIVATED,
        (shouldTalk || inputActive) ? INPUT_ACTIVE : INPUT_DEACTIVATED)) != ERROR_ok)
    {
        Error(serverConnectionHandlerID,error,"Error toggling input:");
        return 1;
    }

    // Update the client
    ts3Functions.flushClientSelfUpdates(serverConnectionHandlerID, NULL);

    // Commit the change
    pttActive = shouldTalk;

    return 0;
}

/* Playback */
unsigned int TSFunctions::GetPlaybackConfigValueAsFloat(uint64 serverConnectionHandlerID, const char* ident, float* result)
{
    unsigned int error;

    if((error = ts3Functions.getPlaybackConfigValueAsFloat(serverConnectionHandlerID, ident, result)) != ERROR_ok) {
        Error(serverConnectionHandlerID,error,"Error retrieving Playback config value:");
    }
    return error;
}

unsigned int TSFunctions::SetPlaybackConfigValue(uint64 serverConnectionHandlerID, const char* ident, const char* value)
{
    unsigned int error;

    if((error = ts3Functions.setPlaybackConfigValue(serverConnectionHandlerID, ident, value)) != ERROR_ok) {
        Error(serverConnectionHandlerID,error,"Error setting Playback config value:");
    }
    return error;
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

unsigned int TSFunctions::GetClientUID(uint64 serverConnectionHandlerID, anyID clientID, QString &result) {
    unsigned int error;
    char* res;
    if((error = ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_UNIQUE_IDENTIFIER, &res)) == ERROR_ok) {
        result = QString::fromUtf8(res);
        ts3Functions.freeMemory(res);
    }
    return error;
}

unsigned int TSFunctions::GetChannelClientList(uint64 serverConnectionHandlerID, uint64 channelID,  anyID** result)
{
    unsigned int error;
    if((error = ts3Functions.getChannelClientList(serverConnectionHandlerID,channelID, result)) != ERROR_ok)
    {
        Error(serverConnectionHandlerID,error,"Error getting Client Channel Client list:");
    }
    return error;
}

unsigned int TSFunctions::GetSubChannels(uint64 serverConnectionHandlerID, uint64 channelId, QList<uint64>* result)
{
    unsigned int error = ERROR_ok;

    uint64* channelList;
    if ((error = ts3Functions.getChannelList(serverConnectionHandlerID,&channelList)) != ERROR_ok)
        return error;

    for (int i = 0; channelList[i]!=NULL; ++i)
    {
        uint64 channel;
        if ((error = ts3Functions.getParentChannelOfChannel(serverConnectionHandlerID,channelList[i],&channel)) != ERROR_ok)
            break;

        if (channel == channelId)
            result->append(channel);
    }
    ts3Functions.freeMemory(channelList);
    return error;
}

unsigned int TSFunctions::SetWhisperList(uint64 serverConnectionHandlerID, GroupWhisperType groupWhisperType, GroupWhisperTargetMode groupWhisperTargetMode)
{
    unsigned int error = ERROR_ok;

    anyID myID;
    if((error = GetClientId(serverConnectionHandlerID,&myID)) != ERROR_ok)
        return error;

    if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ALL)
    {
        //Get all visible clients
        anyID *clientList;
        if ((error = ts3Functions.getClientList(serverConnectionHandlerID, &clientList)) != ERROR_ok)
            return error;

        if (groupWhisperType == GROUPWHISPERTYPE_ALLCLIENTS)
            return ts3Functions.requestClientSetWhisperList(serverConnectionHandlerID,NULL,NULL,clientList,NULL);
        else if (groupWhisperType == GROUPWHISPERTYPE_CHANNELCOMMANDER)
        {
            QList<anyID> channelCommanders;
            for (int i=0; clientList[i]!=NULL ; ++i)
            {
                int isChannelCommander;
                if((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientList[i], CLIENT_IS_CHANNEL_COMMANDER, &isChannelCommander)) != ERROR_ok)
                    break;

                if (isChannelCommander == 1)
                    channelCommanders.append(clientList[i]);
            }
            ts3Functions.freeMemory(clientList);

            if (error == ERROR_ok)
                return ts3Functions.requestClientSetWhisperList(serverConnectionHandlerID,NULL,NULL,channelCommanders.toVector().constData(),NULL);
        }
    }
    else if (groupWhisperTargetMode > GROUPWHISPERTARGETMODE_ALL)
    {
        // get my channel
        uint64 mychannel;
        if ((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&mychannel)) != ERROR_ok)
            return error;
        QList<uint64> targetChannels;

        if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_CURRENTCHANNEL)
            targetChannels.append(mychannel);
        else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_PARENTCHANNEL)
        {
            uint64 channel;
            if ((error = ts3Functions.getParentChannelOfChannel(serverConnectionHandlerID,mychannel,&channel)) != ERROR_ok)
                return error;

            targetChannels.append(channel);
        }
        else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ALLPARENTCHANNELS) // I assume it's a straight walk up the hierarchy?
        {
            uint64 sourcechannel= mychannel;
            while(true)
            {
                uint64 channel;
                if ((error = ts3Functions.getParentChannelOfChannel(serverConnectionHandlerID,sourcechannel,&channel)) != ERROR_ok)
                    return error;

                if (channel == 0)
                    break;

                sourcechannel = channel;
                targetChannels.append(channel);
            }
        }
        else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY)    // ehr, hmm, dunno, whatever
        {

        }
        else if ((groupWhisperTargetMode == GROUPWHISPERTARGETMODE_CHANNELFAMILY) || (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_SUBCHANNELS))
        {
            if ((error = GetSubChannels(serverConnectionHandlerID,mychannel,&targetChannels)) != ERROR_ok)
                return error;

            if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_CHANNELFAMILY) // channel familty: "this channel and all sub channels"
                targetChannels.append(mychannel);
        }

        if (groupWhisperType == GROUPWHISPERTYPE_ALLCLIENTS)
        {
            // Set whisper with channels and client NULL
            //targetChannels.append(NULL); // conversion will do that methinks
            return ts3Functions.requestClientSetWhisperList(serverConnectionHandlerID,NULL,targetChannels.toVector().constData(),NULL,NULL);
        }
        else if (groupWhisperType == GROUPWHISPERTYPE_CHANNELCOMMANDER)
        {
            // Set whisper with channels NULL and client list
            QList<anyID> channelCommanders;
            for (int i = 0; i<targetChannels.count();++i)
            {
                // get clients in channel
                anyID *clientList;
                if ((error = ts3Functions.getChannelClientList(serverConnectionHandlerID,targetChannels.at(i),&clientList)) != ERROR_ok)
                    return error;

                for (int i=0; clientList[i]!=NULL ; ++i)
                {
                    int isChannelCommander;
                    if((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientList[i], CLIENT_IS_CHANNEL_COMMANDER, &isChannelCommander)) != ERROR_ok)
                        break;

                    if (isChannelCommander == 1)
                        channelCommanders.append(clientList[i]);
                }
                ts3Functions.freeMemory(clientList);
            }
            if (error == ERROR_ok)
                return ts3Functions.requestClientSetWhisperList(serverConnectionHandlerID,NULL,NULL,channelCommanders.toVector().constData(),NULL);
        }
    }
    return error;
}

void TSFunctions::UpdatePttDelayInfo()
{
    // Get default capture profile and preprocessor data
    char** profiles;
    int defaultProfile;
    unsigned int error;
    if((error = ts3Functions.getProfileList(PLUGIN_GUI_SOUND_CAPTURE, &defaultProfile, &profiles)) != ERROR_ok)
    {
        Error(NULL,error,"Error retrieving capture profiles");
        return;
    }
    QString profile = profiles[defaultProfile];
    ts3Functions.freeMemory(profiles);

    QString preProcessorData;
    if (!(Settings.GetPreProcessorData(profile,preProcessorData)))
    {
        //Get Error
        QString msg = "(TSFunctions::UpdatePttDelayInfo()) (GetPreProcessorData) (" + profile + ")" + Settings.GetLastError().text();
        Error(NULL,NULL,msg.toLocal8Bit().data());
        return;
    }

    QStringList lines = preProcessorData.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
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
            {
                Error(NULL,NULL,"Error retrieving delay_ptt_msecs");
            }
            else
            {
                pttDelayMsec = msec;
            }
        }
        else if (str_qstr.startsWith("delay_ptt"))
        {
            pttDelayEnabled = (str_qstr == "delay_ptt=true");
        }
    }
}

//! Explicit initialization called from within ts3plugin_init
/*!
 * \brief TSFunctions::Init For stuff that cannot be put inside the constructor, since that is called on file level
 */
void TSFunctions::Init()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onPttDelayFinished()));
    timer->setSingleShot(true);
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
            Error(serverConnectionHandlerID,error,"Error getting client id:");

        this->myClientIDs.insert(serverConnectionHandlerID,result);
    }
    else if (newStatus == STATUS_DISCONNECTED)
    {
        this->myClientIDs.remove(serverConnectionHandlerID);
    }
}

//! Emits PttDelayFinished when the timer runs out
/*!
 * \brief TSFunctions::onPttDelayFinished Qt Slot
 */
void TSFunctions::onPttDelayFinished()
{
    command_mutex->tryLock(PLUGIN_THREAD_TIMEOUT);
    emit PttDelayFinished();
    command_mutex->unlock();
}

//! Retrieves plugin commands, distributes them to the modules
/*!
 * \brief TSFunctions::ParseCommand Retrieves plugin commands, distributes them to the modules
 * \param cmd the command
 * \param arg arguments, if any
 */
void TSFunctions::ParseCommand(char* cmd, char* arg)
{
    if (!(command_mutex->tryLock(PLUGIN_THREAD_TIMEOUT)))
    {
        ts3Functions.logMessage("Timeout while waiting for mutex", LogLevel_WARNING, PLUGIN_NAME, 0);
        return;
    }
    emit Command(cmd,arg);

    command_mutex->unlock();
}
