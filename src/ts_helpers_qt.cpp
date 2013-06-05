#include "ts_helpers_qt.h"

#include "public_rare_definitions.h"
#include "public_errors.h"
#include "public_errors_rare.h"
#include "ts3_functions.h"
#include "plugin.h"

#include "ts_settings_qt.h"
#include "ts_logging_qt.h"

#include <QApplication>

namespace TSHelpers
{
    QString GetConfigPath()
    {
        // Find config path for config class
        char* configPath = (char*)malloc(PATH_BUFSIZE);
        ts3Functions.getConfigPath(configPath, PATH_BUFSIZE);
        return configPath;
    }

    QString GetFullConfigPath()
    {
        QString fullPath=GetConfigPath();
        fullPath.append(ts3plugin_name());
        fullPath.append(".ini");
        return fullPath;
    }

    QString GetLanguage()
    {
        QString lang = "";
        if (!TSSettings::instance()->GetLanguage(lang))
            TSLogging::Error("(TSHelpers::GetLanguage) " + TSSettings::instance()->GetLastError().text(), true);

        return lang;
    }

    int IsClientQuery(uint64 serverConnectionHandlerID, anyID clientID) //A normal Client-Connection (Voice-Connection) has client-type 0, a Query-Connection has client-type 1.
    {
        int type = 0;
        unsigned int error;
        if ((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientID, CLIENT_TYPE, &type)) != ERROR_ok)
            TSLogging::Error("Error checking if client is query.",serverConnectionHandlerID,error);

        return type;
    }

    unsigned int GetClientUID(uint64 serverConnectionHandlerID, anyID clientID, QString &result)
    {
        unsigned int error;
        char* res;
        if((error = ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_UNIQUE_IDENTIFIER, &res)) != ERROR_ok)
            TSLogging::Error("(TSHelpers::GetClientUID)",serverConnectionHandlerID,error,true);
        else
        {
            result = QString::fromUtf8(res);
            ts3Functions.freeMemory(res);
        }
        return error;
    }

    unsigned int GetTalkStatus(uint64 serverConnectionHandlerID, anyID clientID, int &status, int &isWhispering)
    {
        unsigned int error;
        if((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientID, CLIENT_FLAG_TALKING, &status)) != ERROR_ok)
            TSLogging::Error("(TSHelpers::GetTalkStatus)",serverConnectionHandlerID,error,true);
        else
        {
            if (status == STATUS_TALKING)
            {
                if((error = ts3Functions.isWhispering(serverConnectionHandlerID, clientID, &isWhispering)) != ERROR_ok)
                    TSLogging::Error("(TSHelpers::GetTalkStatus)",serverConnectionHandlerID,error,true);
            }
        }
        return error;
    }

    unsigned int GetSubChannels(uint64 serverConnectionHandlerID, uint64 channelId, QList<uint64>* result)
    {
        unsigned int error;

        uint64* channelList;
        if ((error = ts3Functions.getChannelList(serverConnectionHandlerID,&channelList)) != ERROR_ok)
            TSLogging::Error("(TSHelpers::GetSubChannels)",serverConnectionHandlerID,error,true);
        else
        {
            for (int i = 0; channelList[i]!=NULL; ++i)
            {
                uint64 channel;
                if ((error = ts3Functions.getParentChannelOfChannel(serverConnectionHandlerID,channelList[i],&channel)) != ERROR_ok)
                {
                    TSLogging::Error("(TSHelpers::GetSubChannels)",serverConnectionHandlerID,error,true);
                    break;
                }
                if (channel == channelId)
                    result->append(channel);
            }
            ts3Functions.freeMemory(channelList);
        }
        return error;
    }

    unsigned int GetServerHandler(QString name,uint64* result)
    {
        unsigned int error;
        uint64* servers;

        // Get server list
        if((error = ts3Functions.getServerConnectionHandlerList(&servers)) != ERROR_ok)
            TSLogging::Error("(TSHelpers::GetServerHandler)",NULL,error,true);
        else
        {
            // Find server in the list
            uint64* server;
            char* s_name;
            for(server = servers; *server != (uint64)NULL; ++server)
            {
                if ((error = ts3Functions.getServerVariableAsString(*server, VIRTUALSERVER_NAME, &s_name)) != ERROR_ok)
                {
                    TSLogging::Error("(TSHelpers::GetServerHandler)",NULL,error,true);
                    break;
                }

                if(name==s_name)
                {
                    ts3Functions.freeMemory(s_name);
                    *result = *server;
                    error = ERROR_ok;
                    break;
                }
                ts3Functions.freeMemory(s_name);
                error = ERROR_not_connected;
            }
            ts3Functions.freeMemory(servers);
        }
        return error;
    }

    uint64 GetActiveServerConnectionHandlerID()
    {
        unsigned int error;
        uint64* servers;
        uint64* server;
        uint64 handle = NULL;

        if((error = ts3Functions.getServerConnectionHandlerList(&servers)) != ERROR_ok)
        {
            TSLogging::Error("(TSHelpers::GetActiveServerConnectionHandlerID) Error retrieving list of servers",error);
            return NULL;
        }

        // Find the first server that matches the criteria
        for(server = servers; *server != (uint64)NULL && handle == NULL; server++)
        {
            int result;
            if((error = ts3Functions.getClientSelfVariableAsInt(*server, CLIENT_INPUT_HARDWARE, &result)) != ERROR_ok)
                TSLogging::Error("(TSHelpers::GetActiveServerConnectionHandlerID) Error retrieving client variable",*server,error);
            else if(result)
            {
                handle = *server;
                break;
            }
        }

        ts3Functions.freeMemory(servers);
        return handle;
    }

    unsigned int GetActiveServerRelative(uint64 serverConnectionHandlerID, bool next, uint64* result)
    {
        unsigned int error;
        uint64* servers;
        uint64* server;

        // Get server list
        if((error = ts3Functions.getServerConnectionHandlerList(&servers)) != ERROR_ok)
        {
            TSLogging::Error("(TSHelpers::GetActiveServerRelative) Error retrieving list of servers",NULL,error,true);
            return error;
        }

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

    int SetActiveServer(uint64 serverConnectionHandlerID)
    {
        unsigned int error;

        if((error = ts3Functions.activateCaptureDevice(serverConnectionHandlerID)) != ERROR_ok)
        {
            TSLogging::Error("(TSHelpers::SetActiveServer) Error activating server",serverConnectionHandlerID,error);
            return 1;
        }

        return 0;
    }

    int SetActiveServerRelative(uint64 serverConnectionHandlerID, bool next)
    {
        unsigned int error;

        uint64 server;
        if ((error = TSHelpers::GetActiveServerRelative(serverConnectionHandlerID,next,&server)) != ERROR_ok)
            return 1;

        // Check if already active
        int result;
        if((error = ts3Functions.getClientSelfVariableAsInt(server, CLIENT_INPUT_HARDWARE, &result)) != ERROR_ok)
        {
            TSLogging::Error("(TSHelpers::SetActiveServerRelative) Error retrieving client variable",serverConnectionHandlerID,error);
            return 1;
        }
        if(!result) SetActiveServer(server);

        return 0;
    }

    unsigned int SetWhisperList(uint64 serverConnectionHandlerID, GroupWhisperType groupWhisperType, GroupWhisperTargetMode groupWhisperTargetMode)
    {
        unsigned int error = ERROR_ok;

        anyID myID;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        {
            TSLogging::Error("(TSHelpers::SetWhisperList)",serverConnectionHandlerID,error,true);
            return error;
        }

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
            {
                TSLogging::Error("(TSHelpers::SetWhisperList)",serverConnectionHandlerID,error,true);
                return error;
            }

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
                if ((error = TSHelpers::GetSubChannels(serverConnectionHandlerID,mychannel,&targetChannels)) != ERROR_ok)
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

    unsigned int GetDefaultProfile(PluginGuiProfile profile, QString &result)
    {
        unsigned int error;
        char** profiles;
        int defaultProfile;
        if((error = ts3Functions.getProfileList(profile, &defaultProfile, &profiles)) != ERROR_ok)
            TSLogging::Error("Error retrieving profiles",error);
        else
        {
            result = profiles[defaultProfile];
            ts3Functions.freeMemory(profiles);
        }
        return error;
    }

    QWidget* GetMainWindow()
    {
        // Get MainWindow
        QList<QWidget*> candidates;
        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            if (widget->isWindow() && widget->inherits("QMainWindow") && !widget->windowTitle().isEmpty())
                candidates.append(widget);
        }

        if (candidates.count() == 1)
            return candidates.at(0);
        else
        {
            TSLogging::Error("Too many candidates for MainWindow. Report to the plugin developer.");
            return NULL;
        }
    }

    unsigned int GetClientSelfServerGroups(uint64 serverConnectionHandlerID, QSet<uint64> *result)
    {
        unsigned int error;
        anyID myID;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        {
            TSLogging::Error("(TSHelpers::GetClientSelfServerGroups)",serverConnectionHandlerID,error,true);
            return error;
        }

        char* cP_result;
        if ((error = ts3Functions.getClientVariableAsString(serverConnectionHandlerID, myID, CLIENT_SERVERGROUPS, &cP_result)) == ERROR_ok)
        {
            QStringList qsl_result = QString(cP_result).split(",",QString::SkipEmptyParts);
//            TSLogging::Print(cP_result);
            ts3Functions.freeMemory(cP_result);

            bool ok;
            while (!qsl_result.isEmpty())
            {
                *result << qsl_result.takeFirst().toInt(&ok,10);
                if (!ok)
                {
                    TSLogging::Error("Error converting Server Group to int");
                    return ERROR_not_implemented;
                }
            }

//            QString debOut = "My Server Groups:";
//            QSetIterator<uint64> i(*result);
//            while (i.hasNext())
//                debOut.append(QString("%1; ").arg(i.next()));

//            TSLogging::Print(debOut);
        }
        return error;
    }
}
