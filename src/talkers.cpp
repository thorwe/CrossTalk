#include "talkers.h"

#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"

#include "ts_logging_qt.h"

#include "plugin.h"

Talkers* Talkers::m_Instance = 0;

Talkers::Talkers():
    m_meTalkingScHandler(0)
{
//    TalkersMap = new QMap<uint64,QMap<anyID,bool>* >;
}

Talkers::~Talkers()
{
}

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

void Talkers::DumpTalkStatusChanges(QObject *p, int status)
{
    TalkInterface *iTalk = qobject_cast<TalkInterface *>(p);
    if (!iTalk)
    {
        TSLogging::Error("(Talkers) (DumpTalkStatusChange) Pointer doesn't implement TalkInterface");
        return;
    }

//    QMapIterator<uint64,QMap<anyID,bool>* > i(*TalkersMap);
//    while (i.hasNext())
//    {
//        i.next();
//        if (i.value()->isEmpty())
//            continue;

//        QMapIterator<anyID,bool> j(*i.value());
//        while (j.hasNext())
//        {
//            j.next();
//            iTalk->onTalkStatusChanged(i.key(),status,j.value(),j.key(),false);
//        }
//    }
    QMapIterator<uint64,anyID> i(TalkerMap);
    while (i.hasNext())
    {
        i.next();
        iTalk->onTalkStatusChanged(i.key(),status,false,i.value(),false);
    }
    i=WhisperMap;
    while (i.hasNext())
    {
        i.next();
        iTalk->onTalkStatusChanged(i.key(),status,true,i.value(),false);
    }
    if (m_meTalkingScHandler != 0)
    {
        unsigned int error;
        // Get My Id on this handler
        anyID myID;
        if((error = ts3Functions.getClientID(m_meTalkingScHandler,&myID)) != ERROR_ok)
        {
            TSLogging::Error("DumpTalkStatusChanges",m_meTalkingScHandler,error);
            return;
        }
        iTalk->onTalkStatusChanged(m_meTalkingScHandler,status,m_meTalkingIsWhisper,myID,true);
    }
}

//void Talkers::RegisterEventTalkStatusChange(QPointer<Module> p, bool isRegister)
//{
//    if (isRegister)
//    {
//        if (TalkStatusChangeModules.contains(p))
//            return;

//        TalkStatusChangeModules.append(p);
//    }
//    else
//    {
//        if (!TalkStatusChangeModules.contains(p))
//            return;

//        TalkStatusChangeModules.removeAt(TalkStatusChangeModules.indexOf(p));
//    }
//}

//void Talkers::FireTalkStatusChange(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID)
//{
//    if (TalkStatusChangeModules.isEmpty())
//        return;
//    for (int i=0;TalkStatusChangeModules.size();++i)
//    {
//        if(!TalkStatusChangeModules.at(i)->OnTalkStatusChanged(serverConnectionHandlerID,status,isReceivedWhisper,clientID))
//            break;
//    }
//}

bool Talkers::onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID)
{
    unsigned int error;

    // Get My Id on this handler
    anyID myID;
    if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
    {
        TSLogging::Error("onTalkStatusChangeEvent",serverConnectionHandlerID,error);
        return false;
    }

//    bool isWhisper = (isReceivedWhisper == 1);

    if (clientID == myID)
    {
        m_meTalkingIsWhisper = isReceivedWhisper;
        if (status==STATUS_TALKING)
        {
            m_meTalkingScHandler = serverConnectionHandlerID;
        }
        else
            m_meTalkingScHandler = 0;

        return true;
    }

//    if (status==STATUS_TALKING)
//    {
//        if (!(TalkersMap->contains(serverConnectionHandlerID)))
//        {
//            QMap<anyID,bool>* map = new QMap<anyID,bool>;
//            map->insert(clientID,isWhisper);
//            TalkersMap->insert(serverConnectionHandlerID, map);
//        }
//        else
//        {
//            QMap<anyID,bool>* map = TalkersMap->value(serverConnectionHandlerID);
//            map->insert(clientID,isWhisper);
//        }
//    }
//    else if (status==STATUS_NOT_TALKING)
//    {
//        QMap<anyID,bool>* map = TalkersMap->value(serverConnectionHandlerID);
//        map->remove(clientID);
//    }
    if (status == STATUS_TALKING)
    {
        if (isReceivedWhisper)
        {
            if (!WhisperMap.contains(serverConnectionHandlerID,clientID))   // pure safety measurement
                WhisperMap.insert(serverConnectionHandlerID,clientID);
        }
        else
        {
            if (!TalkerMap.contains(serverConnectionHandlerID,clientID))
                TalkerMap.insert(serverConnectionHandlerID,clientID);
        }
    }
    else if (status == STATUS_NOT_TALKING)
    {
        if (isReceivedWhisper)
            WhisperMap.remove(serverConnectionHandlerID,clientID);
        else
            TalkerMap.remove(serverConnectionHandlerID,clientID);
    }

    //emit TalkStatusChanged(serverConnectionHandlerID,status,isWhisper,clientID);
//    FireTalkStatusChange(serverConnectionHandlerID,status,isWhisper,clientID);
    return false;
}

void Talkers::onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    if (newStatus == STATUS_DISCONNECTED)
    {
//        if (TalkersMap->contains(serverConnectionHandlerID))
//        {
//            QMap<anyID,bool>* map = TalkersMap->value(serverConnectionHandlerID);
//            QMutableMapIterator<anyID,bool> i(*map);
//            while (i.hasNext())
//            {
//                i.next();
//                ts3plugin_onTalkStatusChangeEvent(serverConnectionHandlerID, STATUS_NOT_TALKING, ((i.value())?1:0), i.key());
//            }
//            if (map->isEmpty())
//                TSLogging::Print("Manually send talker false successfull.");

////            TalkersMap->remove(serverConnectionHandlerID);
//        }
        if (WhisperMap.contains(serverConnectionHandlerID))
        {
            QList<anyID> values = WhisperMap.values(serverConnectionHandlerID);
             for (int i = 0; i < values.size(); ++i)
                 ts3plugin_onTalkStatusChangeEvent(serverConnectionHandlerID, STATUS_NOT_TALKING, 0, values.at(i));
        }
        if (TalkerMap.contains(serverConnectionHandlerID))
        {
            QList<anyID> values = TalkerMap.values(serverConnectionHandlerID);
             for (int i = 0; i < values.size(); ++i)
                 ts3plugin_onTalkStatusChangeEvent(serverConnectionHandlerID, STATUS_NOT_TALKING, 0, values.at(i));
        }
    }
    emit ConnectStatusChanged(serverConnectionHandlerID, newStatus, errorNumber);
}

//QMap<uint64, QMap<anyID, bool> *>* Talkers::GetTalkersMap() const
//{
//    return TalkersMap;
//}

QMultiMap<uint64, anyID> Talkers::GetTalkerMap() const
{
    return TalkerMap;
}

QMultiMap<uint64, anyID> Talkers::GetWhisperMap() const
{
    return WhisperMap;
}

uint64 Talkers::isMeTalking() const
{
    return m_meTalkingScHandler;
}
