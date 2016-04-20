#include "snt.h"

#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"
#include "ts_logging_qt.h"
#include "ts_ptt_qt.h"
#include "ts_helpers_qt.h"

#include "ts_serversinfo.h"
#include "ts_serverinfo_qt.h"

#ifndef RETURNCODE_BUFSIZE
#define RETURNCODE_BUFSIZE 128
#endif

//! Constructor
/*!
 * \brief SnT::SnT Creates an instance of this class
 * \param parent optional Qt Object
 */
SnT::SnT(QObject *parent) :
    QObject(parent)
{
}

//! When the input hardware gets activated, check if there's a Ptt activation scheduled and if so, turn Ptt on
/*!
 * \brief SnT::onClientSelfVariableUpdateEvent equivalent TS Client SDK
 * \param serverConnectionHandlerID equivalent TS Client SDK
 * \param flag equivalent TS Client SDK
 * \param oldValue equivalent TS Client SDK
 * \param newValue equivalent TS Client SDK
 */
void SnT::onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char *oldValue, const char *newValue)
{
    Q_UNUSED(oldValue);

    if ((flag == CLIENT_INPUT_HARDWARE) && (strcmp (newValue,"1") == 0))
    {
        if (m_shallActivatePtt==true)
        {
            if (m_returnCodeScHandler == 0)
            {
                //TSLogging::Log("Activating Ptt from onClientSelfVariableUpdateEvent",serverConnectionHandlerID,LogLevel_DEBUG);
                TSPtt::instance()->SetPushToTalk(serverConnectionHandlerID, PTT_ACTIVATE);
            }

            m_shallActivatePtt=false;
        }
        // seems to be equal to a ts3plugin_currentServerConnectionChanged WHEN initiated by the user
        // except that this version will fire when returning from a cross server ptt event
        // (ts3plugin_currentServerConnectionChanged is not even fired when ptt-switching for whatever reason)
        // Furtunately, we like that here.
    }
}

QString SnT::getReturnCode() const
{
    return m_returnCode;
}

//! Note that the primary intend is NOT to handle errors, but the "reply" from set whisper
void SnT::onServerError(uint64 serverConnectionHandlerID, const char *errorMessage, unsigned int error, const char *returnCode, const char *extraMessage)
{
    Q_UNUSED(errorMessage);
    Q_UNUSED(extraMessage);

    if (m_returnCode != returnCode)
        return;

    if (error != ERROR_ok)
        TSLogging::Log("Whoops. That shouldn't happen that this is an error. But do we care? We don't.",serverConnectionHandlerID,LogLevel_DEBUG);

    if (m_returnCodeScHandler != serverConnectionHandlerID)
        TSLogging::Log("Whoops. That shouldn't happen that those don't match. But do we care? We don't.",serverConnectionHandlerID,LogLevel_DEBUG);

    if (m_returnCodeScHandler == 0)
        TSLogging::Log("Whoops. That shouldn't happen that this is 0. But do we care? We don't.",serverConnectionHandlerID,LogLevel_DEBUG);

    if (!m_shallActivatePtt)
    {
        //TSLogging::Log("Activating Ptt from onServerReply",serverConnectionHandlerID,LogLevel_DEBUG);
        TSPtt::instance()->SetPushToTalk(m_returnCodeScHandler,PTT_ACTIVATE);
    }

    m_returnCodeScHandler = 0;
}

//! Parse Plugin Commands for this module
/*!
 * \brief SnT::ParseCommand Qt Slot
 * \param serverConnectionHandlerID the connection handler forwarded from the API
 * \param cmd the command
 * \param arg arguments (if any)
 */

void SnT::ParseCommand(uint64 serverConnectionHandlerID, QString cmd, QStringList args)
{
    if (m_last_cmd == cmd)
        return;

    m_last_cmd = cmd;

    if (m_returnCode.isEmpty())
    {
        char returnCode[RETURNCODE_BUFSIZE];
        ts3Functions.createReturnCode(pluginID,returnCode,RETURNCODE_BUFSIZE);
        m_returnCode = returnCode;
        //TSLogging::Log(QString("Created SnT Return Code: %1").arg(m_returnCode), LogLevel_DEBUG);
    }

    auto ptt = TSPtt::instance();
    connect(ptt, &TSPtt::PttDelayFinished, this, &SnT::PttDelayFinished, Qt::UniqueConnection); // UniqueConnection saving init

    unsigned int error = ERROR_ok;

    // Get the active server
    auto scHandlerID = TSHelpers::GetActiveServerConnectionHandlerID();
    if(scHandlerID == NULL)
    {
        TSLogging::Log("Failed to get an active server, falling back to current server", LogLevel_DEBUG);
        scHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
        if(scHandlerID == NULL) return;
    }

    int status;
    if((error = ts3Functions.getConnectionStatus(scHandlerID, &status)) != ERROR_ok)
        TSLogging::Error("Error retrieving connection status",scHandlerID,error);

    if(status == STATUS_DISCONNECTED)
        return;

    /***** Communication *****/
    if(cmd == QLatin1String("TS3_PTT_ACTIVATE"))
        ptt->SetPushToTalk(scHandlerID, PTT_ACTIVATE);
    else if(cmd == QLating1String("TS3_PTT_DEACTIVATE"))
        ptt->SetPushToTalk(scHandlerID, PTT_DEACTIVATE);
    else if(cmd == QLatin1String("TS3_PTT_TOGGLE"))
        ptt->SetPushToTalk(scHandlerID, PTT_TOGGLE);
    else if((cmd == QLating1String("TS3_SWITCH_N_TALK_END")) || (cmd == QLatin1String("TS3_NEXT_TAB_AND_TALK_END")) || (cmd == QLatin1String("TS3_NEXT_TAB_AND_WHISPER_END"))) // universal OnKeyUp Handler
    {
        ptt->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings
        if (m_shallClearWhisper)
        {
            if ((error = ts3Functions.requestClientSetWhisperList(scHandlerID,NULL,NULL,NULL,NULL)) != ERROR_ok)
                TSLogging::Error("Could not release whisperlist.",scHandlerID,error);
            else
                m_shallClearWhisper = false;
        }

        if(m_returnToSCHandler != (uint64)NULL)
        {
            TSHelpers::SetActiveServer(m_returnToSCHandler);
            if((cmd == QLatin1String("TS3_NEXT_TAB_AND_TALK_END")) || (cmd == QLatin1String("TS3_NEXT_TAB_AND_WHISPER_END"))) // annoy user to upgrade
            {
                TSLogging::Error("Hotkeys Next Tab and Talk Stop & Next Tab and Whisper Stop are being DEPRECATED! Please use SnT Stop instead!",m_returnToSCHandler,NULL);
            }
            m_returnToSCHandler = (uint64)NULL;
        }
    }
    else if (cmd == "TS3_NEXT_TAB_AND_WHISPER_ALL_CC_START")
    {
        uint64 nextServer;
        if ((error = TSHelpers::GetActiveServerRelative(scHandlerID,true,&nextServer)) != ERROR_ok)
            TSLogging::Error("Could not get next server.",scHandlerID,error);

        if (scHandlerID == nextServer)
            return;

        if ((error = TSHelpers::SetWhisperList(nextServer,GROUPWHISPERTYPE_CHANNELCOMMANDER,GROUPWHISPERTARGETMODE_ALL)) != ERROR_ok)
        {
            if (error != ERROR_ok_no_update)
                TSLogging::Error("Could not set whisperlist",scHandlerID,error);

            return;
        }

        ptt->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt=true;
        m_returnToSCHandler=scHandlerID;
        m_shallClearWhisper = true;
        TSHelpers::SetActiveServer(nextServer);
    }
    else if(cmd == QLatin1String("TS3_NEXT_TAB_AND_TALK_START"))
    {
        uint64 nextServer;
        if ((error = TSHelpers::GetActiveServerRelative(scHandlerID,true,&nextServer)) != ERROR_ok)
            TSLogging::Error("Could not get next server.",scHandlerID,error);

        if (scHandlerID == nextServer)
            return;

        ptt->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt=true;
        m_returnToSCHandler=scHandlerID;
        TSHelpers::SetActiveServer(nextServer);
    }
    else if(cmd == QLatin1String("TS3_SWITCH_TAB_AND_TALK_START"))
    {
        if (args.isEmpty())
        {
            TSLogging::Error("No target server specified.",serverConnectionHandlerID,NULL);
            return;
        }
        QString name;
        if (args.count() == 1)
            name = args.at(0);
        else
            name = args.join(" ");

        uint64 targetServer = 0;
        if ((error = TSHelpers::GetServerHandler(name,&targetServer)) != ERROR_ok)
            return;

        if (targetServer == 0)
            return;

        ptt->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings; maybe not as necessary as below

        if (scHandlerID != targetServer)
        {
            m_shallActivatePtt=true;
            m_returnToSCHandler=scHandlerID;
            TSHelpers::SetActiveServer(targetServer);
        }
        else
            ptt->SetPushToTalk(scHandlerID,true);

    }
    else if (cmd == QLatin1String("TS3_SWITCH_TAB_AND_WHISPER_START"))
    {
        if (args.count() < 3)
        {
            TSLogging::Error("Too few arguments.",scHandlerID,NULL);
            return;
        }

        auto name = args.at(0);
        auto groupWhisperTypeArg = args.at(1);
        auto groupWhisperTargetModeArg = args.at(2);
        uint64 arg = (uint64)NULL;

        uint64 targetServer = 0;
        if ((error = TSHelpers::GetServerHandler(name,&targetServer)) != ERROR_ok)
        {
            TSLogging::Error("Could not get target server.",scHandlerID,error);
            return;
        }
        if (targetServer == 0)
            return;

        GroupWhisperType groupWhisperType = GetGroupWhisperType(groupWhisperTypeArg);
        if (groupWhisperType == GROUPWHISPERTYPE_ENDMARKER)
        {
            TSLogging::Error("Unsupported group whisper type.",scHandlerID,NULL);
            return;
        }
        else if (groupWhisperType == GROUPWHISPERTYPE_CHANNELGROUP)
        {
            auto serversInfo = TSServersInfo::instance();
            if (serversInfo != NULL)
            {
                if (args.count() == 4)
                {
                    auto serverInfo = serversInfo->GetServerInfo(targetServer);
                    if ((arg = serverInfo->GetChannelGroupId(args.at(3))) == (uint64)NULL)
                    {
                        TSLogging::Error("Could not find channel group.");
                        return;
                    }
                }
                else    // Get My Channel Group
                {
                    if ((error = TSHelpers::GetClientChannelGroup(targetServer,&arg)) != ERROR_ok)
                        return;

                    // Blacklist default channel group only with no arg
                    auto serverInfo = serversInfo->GetServerInfo(targetServer);
                    if (serverInfo->getDefaultChannelGroup() == arg)
                        return;
                }
            }
        }
        else if (groupWhisperType == GROUPWHISPERTYPE_SERVERGROUP)
        {
            if (args.count() < 4)
            {
                TSLogging::Error("Not enough arguments.");
                return;
            }

            auto serversInfo = TSServersInfo::instance();
            if (serversInfo != NULL)
            {
                auto serverInfo = serversInfo->GetServerInfo(targetServer);
                if ((arg = serverInfo->GetServerGroupId(args.at(3))) == (uint64)NULL)
                {
                    TSLogging::Error("Could not find server group.");
                    return;
                }
            }
        }

        auto groupWhisperTargetMode  = GetGroupWhisperTargetMode(groupWhisperTargetModeArg);
        if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ENDMARKER)
        {
            TSLogging::Error("Could not recognize group whisper target mode.",scHandlerID,NULL);
            return;
        }
        else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY)
        {
            TSLogging::Error("The group whisper target mode ANCESTORCHANNELFAMILY is not supported.",scHandlerID,NULL);
            return;
        }

        if ((error = TSHelpers::SetWhisperList(targetServer,groupWhisperType,groupWhisperTargetMode,m_returnCode,arg)) != ERROR_ok)
        {
            if (error != ERROR_ok_no_update)
                TSLogging::Error("Could not set whisperlist",scHandlerID,error);

            return;
        }
        m_returnCodeScHandler = targetServer;

        if(status != STATUS_DISCONNECTED)
            ptt->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallClearWhisper = true;
        if (scHandlerID != targetServer)
        {
            m_shallActivatePtt=true;
            m_returnToSCHandler=scHandlerID;
            TSHelpers::SetActiveServer(targetServer);
        }
    }
    else if (cmd == QLatin1String("TS3_NEXT_TAB_AND_WHISPER_START"))
    {
        uint64 nextServer;
        if ((error = TSHelpers::GetActiveServerRelative(scHandlerID,true,&nextServer)) != ERROR_ok)
            TSLogging::Error("Could not get next server.",scHandlerID,error);

        if (scHandlerID == nextServer)
            return;

        if (args.count() < 2)
        {
            TSLogging::Error("Too few arguments.",scHandlerID,NULL);
            return;
        }

        QString arg_qs;
        arg_qs = args.at(0);
        auto groupWhisperType = GetGroupWhisperType(arg_qs);
        if (groupWhisperType == GROUPWHISPERTYPE_ENDMARKER)
        {
            TSLogging::Error("Could not determine group whisper type.",scHandlerID,NULL);
            return;
        }
        else if (groupWhisperType == GROUPWHISPERTYPE_CHANNELGROUP)
        {
            TSServersInfo* serversInfo = TSServersInfo::instance();
            if (serversInfo != NULL)
            {
                // Get My Channel Group
                uint64 myChannelGroup;
                if ((error = TSHelpers::GetClientChannelGroup(nextServer,&myChannelGroup)) != ERROR_ok)
                    return;

                TSServerInfo* serverInfo = serversInfo->GetServerInfo(nextServer);
                if (serverInfo->getDefaultChannelGroup() == myChannelGroup)
                    return;
            }
        }

        arg_qs = args.at(1);
        auto groupWhisperTargetMode  = GetGroupWhisperTargetMode(arg_qs);
        if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ENDMARKER)
        {
            TSLogging::Error("Could not recognize group whisper target mode.",scHandlerID,NULL);
            return;
        }
        else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY)
        {
            TSLogging::Error("The group whisper target mode ANCESTORCHANNELFAMILY is not supported.",scHandlerID,NULL);
            return;
        }

        if ((error = TSHelpers::SetWhisperList(nextServer,groupWhisperType,groupWhisperTargetMode)) != ERROR_ok)
        {
            if (error != ERROR_ok_no_update)
                TSLogging::Error("Could not set whisperlist",scHandlerID,error);

            return;
        }

        if(status != STATUS_DISCONNECTED)
            ptt->SetPushToTalk(scHandlerID, false); //always do immediately regardless of delay settings; maybe not as necessary as below

        m_shallActivatePtt=true;
        m_returnToSCHandler=scHandlerID;
        m_shallClearWhisper = true;
        TSHelpers::SetActiveServer(nextServer);
    }
}

//! Turn Ptt off delayed by the user client setting
/*!
 * \brief SnT::PttDelayFinished Timer slot
 */
void SnT::PttDelayFinished()
{
    // Turn off PTT
    TSPtt::instance()->SetPushToTalk(TSHelpers::GetActiveServerConnectionHandlerID(), false);
}

GroupWhisperTargetMode SnT::GetGroupWhisperTargetMode(QString val)
{
    GroupWhisperTargetMode groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ENDMARKER;

    if (val.contains(QLatin1String("ALLPARENT")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ALLPARENTCHANNELS;
    else if (val.contains(QLatin1String("PARENT")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_PARENTCHANNEL;
    else if (val.contains(QLatin1String("CURRENT")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_CURRENTCHANNEL;
    else if (val.contains(QLatin1String("SUB")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_SUBCHANNELS;
    else if (val.contains(QLatin1String("ANCESTORCHANNELFAMILY")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY;
    else if (val.contains(QLatin1String("FAMILY")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_CHANNELFAMILY;
    else if (val.contains(QLatin1String("ALL")))
        groupWhisperTargetMode = GROUPWHISPERTARGETMODE_ALL;

    return groupWhisperTargetMode;
}

GroupWhisperType SnT::GetGroupWhisperType(QString val)
{
    GroupWhisperType groupWhisperType = GROUPWHISPERTYPE_ENDMARKER;
    if (val.contains(QLatin1String("COMMANDER"),Qt::CaseInsensitive))
        groupWhisperType = GROUPWHISPERTYPE_CHANNELCOMMANDER;
    else if (val.contains(QLatin1String("CHANNEL_GROUP"),Qt::CaseInsensitive))
        groupWhisperType = GROUPWHISPERTYPE_CHANNELGROUP;
    else if (val.contains(QLatin1String("SERVER_GROUP"),Qt::CaseInsensitive))
        groupWhisperType = GROUPWHISPERTYPE_SERVERGROUP;
    else if (val.contains(QLatin1String("ALLC"),Qt::CaseInsensitive))
        groupWhisperType = GROUPWHISPERTYPE_ALLCLIENTS;

    return groupWhisperType;
}
