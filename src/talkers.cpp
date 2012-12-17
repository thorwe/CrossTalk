#include "talkers.h"

#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"

Talkers* Talkers::m_Instance = 0;

Talkers::Talkers()
{
    ts = TSFunctions::instance();
    TalkersMap = new QMap<uint64,QMap<anyID,bool>* >;
}

Talkers::~Talkers()
{
}

// unused atm
unsigned int Talkers::RefreshTalkers(uint64 serverConnectionHandlerID)
{
    unsigned int error = ERROR_ok;
    int status;
    if ((error = ts3Functions.getConnectionStatus(serverConnectionHandlerID, &status)) != ERROR_ok)
        return error;

    if (status != STATUS_CONNECTION_ESTABLISHED)
        return ERROR_ok;

    anyID myID;
    if ((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        return error;

    int talking;
    if ((error = ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerID, CLIENT_FLAG_TALKING, &talking)) != ERROR_ok)
        return error;

    if (talking == STATUS_TALKING)
    {
        int isWhispering;
        if ((error = ts3Functions.isWhispering(serverConnectionHandlerID, myID, &isWhispering)) != ERROR_ok)
            return error;

        onTalkStatusChangeEvent(serverConnectionHandlerID,STATUS_TALKING,isWhispering,myID);
    }

    //Get all visible clients
    anyID *clientList;
    if ((error = ts3Functions.getClientList(serverConnectionHandlerID, &clientList)) != ERROR_ok)
        return error;

    for (int i=0; clientList[i]!=NULL ; ++i)
    {
        if(ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientList[i], CLIENT_FLAG_TALKING, &talking) == ERROR_ok)
        {
            if (talking == STATUS_TALKING)
            {
                int isWhispering;
                if ((error = ts3Functions.isWhispering(serverConnectionHandlerID, clientList[i], &isWhispering)) != ERROR_ok)
                    break;

                onTalkStatusChangeEvent(serverConnectionHandlerID,STATUS_TALKING,isWhispering,clientList[i]);
            }
        }
    }
    ts3Functions.freeMemory(clientList);
    return error;
}

unsigned int Talkers::RefreshAllTalkers()  // I assume getClientVariableAsInt only returns whisperer to me as talking and isWhispering == isWhisperingMe
{
    unsigned int error = ERROR_ok;
    uint64* serverList;
    if(ts3Functions.getServerConnectionHandlerList(&serverList) == ERROR_ok)
    {
        for(int i=0; serverList[i] != NULL; ++i)
        {
            if ((error = RefreshTalkers(serverList[i])) != ERROR_ok)
                break;
        }
        ts3Functions.freeMemory(serverList);
    }
    return error;
}

void Talkers::onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID)
{
    anyID myID;
    if(ts->GetClientId(serverConnectionHandlerID,&myID) != ERROR_ok)
        return;

    if (clientID == myID)
        return;

    bool isWhisper = (isReceivedWhisper == 1);

    if (status==STATUS_TALKING)
    {
        if (!(TalkersMap->contains(serverConnectionHandlerID)))
        {
            QMap<anyID,bool>* map = new QMap<anyID,bool>;
            map->insert(clientID,isWhisper);
            TalkersMap->insert(serverConnectionHandlerID, map);
        }
        else
        {
            QMap<anyID,bool>* map = TalkersMap->value(serverConnectionHandlerID);
            map->insert(clientID,isWhisper);
        }
    }
    else if (status==STATUS_NOT_TALKING)
    {
        QMap<anyID,bool>* map = TalkersMap->value(serverConnectionHandlerID);
        map->remove(clientID);
    }
    emit TalkStatusChanged(serverConnectionHandlerID,status,isWhisper,clientID);
}

void Talkers::onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    if (newStatus == STATUS_DISCONNECTED)
    {
        if (TalkersMap->contains(serverConnectionHandlerID))
        {
            QMap<anyID,bool>* map = TalkersMap->value(serverConnectionHandlerID);
            QMutableMapIterator<anyID,bool> i(*map);
            while (i.hasNext())
            {
                i.next();
                int isWhisper;
                if (i.value())
                    isWhisper = 1;
                else
                    isWhisper = 0;

                onTalkStatusChangeEvent(serverConnectionHandlerID, STATUS_NOT_TALKING, isWhisper, i.key());
            }
//            if (map->isEmpty())
//                ts->Print("Manually send talker false successfull.\n");

            TalkersMap->remove(serverConnectionHandlerID);
        }
    }
    emit ConnectStatusChanged(serverConnectionHandlerID, newStatus, errorNumber);
}

QMap<uint64, QMap<anyID, bool> *>* Talkers::GetTalkersMap() const
{
    return TalkersMap;
}
