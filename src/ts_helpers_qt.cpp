#include "ts_helpers_qt.h"

#include "public_rare_definitions.h"
#include "public_errors.h"
#include "public_errors_rare.h"
#include "ts3_functions.h"
#include "plugin.h"

#include "ts_settings_qt.h"
#include "ts_logging_qt.h"

#include <QApplication>

#ifndef RETURNCODE_BUFSIZE
#define RETURNCODE_BUFSIZE 128
#endif

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

    unsigned int GetSubChannels(uint64 serverConnectionHandlerID, uint64 channelId, QVector<uint64>* result)
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
            TSLogging::Error("(TSHelpers::GetServerHandler)getServerConnectionHandlerList",NULL,error,true);
        else
        {
            // Find server in the list
            uint64* server;
            char* s_name;
            for(server = servers; *server != (uint64)NULL; ++server)
            {
                if ((error = ts3Functions.getServerVariableAsString(*server, VIRTUALSERVER_NAME, &s_name)) != ERROR_ok)
                {
                    if (error != ERROR_not_connected)
                    {
                        TSLogging::Error("(TSHelpers::GetServerHandler)getServerVariableAsString",NULL,error,true);
                        break;
                    }
                }
                else
                {
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

    namespace {

        unsigned int GetChannelsForGroupWhisperTargetMode(uint64 serverConnectionHandlerID, anyID myID, GroupWhisperTargetMode groupWhisperTargetMode, QVector<uint64>* targetChannels)
        {
            unsigned int error = ERROR_ok;
            if (groupWhisperTargetMode != GROUPWHISPERTARGETMODE_ALL)
            {
                // get my channel
                uint64 mychannel;
                if ((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&mychannel)) != ERROR_ok)
                {
                    TSLogging::Error("(TSHelpers::GetChannelsForGroupWhisperTargetMode)",serverConnectionHandlerID,error,true);
                    return error;
                }

                if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_CURRENTCHANNEL)
                    targetChannels->append(mychannel);
                else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_PARENTCHANNEL)
                {
                    uint64 channel;
                    if ((error = ts3Functions.getParentChannelOfChannel(serverConnectionHandlerID,mychannel,&channel)) != ERROR_ok)
                        return error;

                    targetChannels->append(channel);
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
                        targetChannels->append(channel);
                    }
                }
                else if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ANCESTORCHANNELFAMILY)    // ehr, hmm, dunno, whatever
                {

                }
                else if ((groupWhisperTargetMode == GROUPWHISPERTARGETMODE_CHANNELFAMILY) || (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_SUBCHANNELS))
                {
                    if ((error = TSHelpers::GetSubChannels(serverConnectionHandlerID, mychannel, targetChannels)) != ERROR_ok)
                        return error;

                    if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_CHANNELFAMILY) // channel family: "this channel and all sub channels"
                        targetChannels->append(mychannel);
                }
            }
            return error;
        }
    }

    unsigned int SetWhisperList(uint64 serverConnectionHandlerID, GroupWhisperType groupWhisperType, GroupWhisperTargetMode groupWhisperTargetMode, QString returnCode, uint64 arg)
    {
        unsigned int error = ERROR_ok;

        anyID myID;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        {
            TSLogging::Error("(TSHelpers::SetWhisperList)",serverConnectionHandlerID,error,true);
            return error;
        }

        // GroupWhisperTargetMode
        QVector<anyID> clientList;
        QVector<uint64> targetChannelIDs;

        if (groupWhisperTargetMode == GROUPWHISPERTARGETMODE_ALL)   // Get client list
        {
            anyID* clients;
            if ((error = ts3Functions.getClientList(serverConnectionHandlerID, &clients)) != ERROR_ok)
                return error;

            for (unsigned int i = 0; clients[i] != NULL; ++i)
            {
                if (myID != clients[i])
                    clientList.append(clients[i]);
            }

            ts3Functions.freeMemory(clients);
        }
        else if ((error = GetChannelsForGroupWhisperTargetMode(serverConnectionHandlerID,myID,groupWhisperTargetMode,&targetChannelIDs)) != ERROR_ok)
            return error;

        // GroupWhisperType
        if (groupWhisperType != GROUPWHISPERTYPE_ALLCLIENTS)
        {
            // We need a client list for all those, so convert channel list to clients
            if (!targetChannelIDs.isEmpty())  // same:(groupWhisperTargetMode != GROUPWHISPERTARGETMODE_ALL);
            {
                for (int i = 0; i < targetChannelIDs.count(); ++i)
                {
                    // get clients in channel
                    anyID *clients;
                    if ((error = ts3Functions.getChannelClientList(serverConnectionHandlerID,targetChannelIDs.at(i),&clients)) != ERROR_ok)
                        return error;

                    for (int j=0; clients[j]!=NULL ; ++j)
                    {
                        if (myID != clients[j])
                            clientList.append(clients[j]);
                    }

                    ts3Functions.freeMemory(clients);
                }
                targetChannelIDs.clear();
            }

            // Do the GroupWhisperType filtering
            QVector<anyID> filteredClients;
            if (groupWhisperType == GROUPWHISPERTYPE_SERVERGROUP)
            {
                if (arg == (uint64)NULL)
                {
                    TSLogging::Error("No target server group specified. Aborting.");
                    return error;
                }

                QSet<uint64> serverGroups;
                for (int i=0; i < clientList.count(); ++i)
                {
                    if ((error = GetClientServerGroups(serverConnectionHandlerID,clientList[i],&serverGroups)) != ERROR_ok)
                        return error;

                    if (serverGroups.contains(arg))
                        filteredClients.append(clientList[i]);

                    serverGroups.clear();
                }
            }
            else if (groupWhisperType == GROUPWHISPERTYPE_CHANNELGROUP)
            {
                // Get My Channel Group if no arg
                if (arg == (uint64)NULL)
                {
                    if ((error = GetClientChannelGroup(serverConnectionHandlerID,&arg)) != ERROR_ok)
                        return error;
                }

                for (int i=0; i < clientList.count(); ++i)
                {
                    uint64 channelGroup;
                    if ((error = GetClientChannelGroup(serverConnectionHandlerID, &channelGroup, clientList[i])) != ERROR_ok)
                        break;

                    if (channelGroup == arg)
                        filteredClients.append(clientList[i]);
                }
            }
            else if (groupWhisperType == GROUPWHISPERTYPE_CHANNELCOMMANDER)
            {
                for (int i=0; i < clientList.count(); ++i)
                {
                    int isChannelCommander;
                    if((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientList[i], CLIENT_IS_CHANNEL_COMMANDER, &isChannelCommander)) != ERROR_ok)
                        break;

                    if (isChannelCommander == 1)
                        filteredClients.append(clientList[i]);
                }
            }

            clientList = filteredClients;
        }

        if (targetChannelIDs.isEmpty() && clientList.isEmpty())
            return ERROR_ok_no_update;
        else
        {
            TSLogging::Log("CrossTalk's attempting to whisper to:",serverConnectionHandlerID, LogLevel_DEBUG);
            if (!targetChannelIDs.isEmpty())
            {
                QString string = "ChannelIds: ";
                for (int i = 0; i < targetChannelIDs.size(); ++i)
                {
                    string.append(QString("%1").arg(targetChannelIDs.at(i)));
                    string.append(" ");
                }

                TSLogging::Log(string.toLocal8Bit().constData(),serverConnectionHandlerID,LogLevel_DEBUG);
            }
            if (!clientList.isEmpty())
            {
                QString string = "Clients: ";
                for (int i = 0; i<clientList.size(); ++i)
                {
                    char name[512];
                    if((error = ts3Functions.getClientDisplayName(serverConnectionHandlerID, clientList.at(i), name, 512)) != ERROR_ok)
                        string.append("(Error getting client display name)");
                    else
                        string.append(name);

                    string.append(" ");
                }
                TSLogging::Log(string.toLocal8Bit().constData(),serverConnectionHandlerID,LogLevel_DEBUG);
            }

            if (!targetChannelIDs.isEmpty())
                targetChannelIDs.append(0);
            if (!clientList.isEmpty())
                clientList.append(0);

            error = ts3Functions.requestClientSetWhisperList(serverConnectionHandlerID, myID, (targetChannelIDs.isEmpty())?(uint64*)NULL:targetChannelIDs.constData(), (clientList.isEmpty())?(anyID*)NULL:clientList.constData(), (returnCode.isEmpty())?NULL:returnCode.toLocal8Bit().constData());
            if ((error != ERROR_ok) || (returnCode.isEmpty()))
                return error;

            // Bandaid for requestClientSetWhisperList NOT calling back when returnCode set. For waffle's sake!
            return ts3Functions.requestClientVariables(serverConnectionHandlerID,myID,returnCode.toLocal8Bit().constData());
        }
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

    unsigned int GetClientServerGroups(uint64 serverConnectionHandlerID, anyID clientID, QSet<uint64> *result)
    {
        unsigned int error;
        char* cP_result;
        if ((error = ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_SERVERGROUPS, &cP_result)) == ERROR_ok)
        {
            QStringList qsl_result = QString(cP_result).split(",",QString::SkipEmptyParts);
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

//            QString debOut = "Server Groups:";
//            QSetIterator<uint64> i(*result);
//            while (i.hasNext())
//                debOut.append(QString("%1; ").arg(i.next()));

//            TSLogging::Print(debOut);
        }
        return error;
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

        return GetClientServerGroups(serverConnectionHandlerID,myID,result);
    }

    unsigned int GetClientChannelGroup(uint64 serverConnectionHandlerID, uint64* result, anyID clientId)
    {
        unsigned int error;
        if (clientId == (anyID)NULL)    // use my id
        {
            if((error = ts3Functions.getClientID(serverConnectionHandlerID,&clientId)) != ERROR_ok)
            {
                TSLogging::Error("(GetClientChannelGroup)",serverConnectionHandlerID,error,true);
                return error;
            }
        }

        int channelGroupId;
        if ((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientId, CLIENT_CHANNEL_GROUP_ID, &channelGroupId)) != ERROR_ok)
        {
            TSLogging::Error("(GetClientChannelGroup)",serverConnectionHandlerID,error,true);
            return error;
        }
        *result = (uint64)channelGroupId;

        return error;
    }
}
