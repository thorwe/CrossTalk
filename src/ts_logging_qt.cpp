#include "ts_logging_qt.h"

#include "public_errors.h"
#include "public_errors_rare.h"

#include "ts_settings_qt.h"

bool TSLogging::GetErrorSound(QString &in)
{
    QString pack;
    if (TSSettings::instance()->GetSoundPack(pack))
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
    return false;
}

bool TSLogging::GetInfoIcon(QString &in)
{
    QString pack;
    if (TSSettings::instance()->GetIconPack(pack))
    {
        // Find the path to the skin
        char path[PATH_BUFSIZE];
        ts3Functions.getResourcesPath(path, PATH_BUFSIZE);
        QString path_qstr(path);
        in = (path_qstr + "gfx/" + pack + "/16x16_message_info.png");
        return true;
    }
    return false;
}

void TSLogging::PlayErrorSound(uint64 serverConnectionHandlerID)
{
    if ((serverConnectionHandlerID == NULL) || (serverConnectionHandlerID == 0))
        serverConnectionHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
    if ((serverConnectionHandlerID == NULL) || (serverConnectionHandlerID == 0))
        return;

    QString errorSound;
    if (!(GetErrorSound(errorSound)))
        Log(("TSLogging::PlayErrorSound:" + TSSettings::instance()->GetLastError().text()),LogLevel_WARNING);
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
                    Log(QString("Error playing error sound: %1").arg(errorMsg), LogLevel_WARNING);
                    ts3Functions.freeMemory(errorMsg);
                }
                else
                    Log("Error playing error sound.", LogLevel_WARNING);
            }
        }
    }
}

void TSLogging::Error(QString message, uint64 serverConnectionHandlerID, unsigned int error, bool isSoundSilent)
{

    if (error != NULL)
    {
        char* errorMsg;
        if(ts3Functions.getErrorMessage(error, &errorMsg) == ERROR_ok)
        {
            QTextStream(&message) << ": " << errorMsg;
            ts3Functions.freeMemory(errorMsg);
        }
    }

    Log(message,serverConnectionHandlerID,LogLevel_ERROR);
    if (!isSoundSilent)
        PlayErrorSound(serverConnectionHandlerID);

    // Format and print the error message
    if ((serverConnectionHandlerID == NULL) || (serverConnectionHandlerID == 0))
        serverConnectionHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
    if ((serverConnectionHandlerID == NULL) || (serverConnectionHandlerID == 0))
        return;

    // Use a transparent underscore because a double space will be collapsed
    QString styledQstr;

    QString infoIcon;
    if (!(GetInfoIcon(infoIcon)))
    {
        //Get Error
        QString msg = "TSLogging::Error:GetInfoIcon:" + TSSettings::instance()->GetLastError().text();
        ts3Functions.logMessage(msg.toLocal8Bit().constData(), LogLevel_ERROR, ts3plugin_name(), 0);
    }
    else
        QTextStream(&styledQstr) << "[img]" << infoIcon << "[/img]";

    QTime time = QTime::currentTime ();
    QString time_qstr = time.toString(Qt::TextDate);

    QTextStream(&styledQstr) << "[color=red]" << time_qstr << "[/color][color=transparent]_[/color]" << ": "  << ts3plugin_name() << ": " << message;
    ts3Functions.printMessage(serverConnectionHandlerID, styledQstr.toLocal8Bit().constData(), PLUGIN_MESSAGE_TARGET_SERVER);
}

void TSLogging::Print(QString message, uint64 serverConnectionHandlerID, LogLevel logLevel)
{
#ifndef CONSOLE_OUTPUT
    Q_UNUSED(message);
    Q_UNUSED(serverConnectionHandlerID);
    Q_UNUSED(logLevel);
#else
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

    if ((serverConnectionHandlerID != NULL) && (serverConnectionHandlerID != 0))
        message.prepend(QString("%1 : ").arg(serverConnectionHandlerID));

    QTime time = QTime::currentTime ();
    QString time_qstr = time.toString(Qt::TextDate);
    QString styledQstr;
    QTextStream(&styledQstr) << time_qstr << ": " << ts3plugin_name() << ": " << message << "\n";
    printf(styledQstr.toLocal8Bit().constData());
#endif
}

void TSLogging::Log(QString message, uint64 serverConnectionHandlerID, LogLevel logLevel)
{
    if (serverConnectionHandlerID == NULL)
        serverConnectionHandlerID = 0;
    ts3Functions.logMessage(message.toLocal8Bit().constData(), logLevel, ts3plugin_name(), serverConnectionHandlerID);
}
