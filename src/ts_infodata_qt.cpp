#include "ts_infodata_qt.h"
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "ts3_functions.h"
#include "plugin.h"

#include "ts_logging_qt.h"

#ifndef INFODATA_BUFSIZE
#define INFODATA_BUFSIZE 256    //128, ah, just increasing the value doesn't make anything boom
#endif

TSInfoData* TSInfoData::m_Instance = 0;

TSInfoData::TSInfoData(){}

PluginItemType TSInfoData::getInfoType() const
{
    return m_InfoType;
}

uint64 TSInfoData::getInfoId() const
{
    return m_InfoId;
}

//void TSInfoData::setHomeId(uint64 serverConnectionHandlerID)
//{
//    if (serverConnectionHandlerID == m_homeId)
//        return;
//    m_homeId = serverConnectionHandlerID;
//}

void TSInfoData::RequestUpdate(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type)
{
//    TSLogging::Print(QString("(TSInfoData::RequestUpdate) id: %1 type: %2").arg(id).arg(type),serverConnectionHandlerID,LogLevel_DEBUG);
//    TSLogging::Print(QString("(TSInfoData::RequestUpdate) m_id: %1 m_type: %2").arg(m_InfoId).arg(m_InfoType),m_homeId,LogLevel_DEBUG);
    if ((m_InfoId == 0) || (type != m_InfoType) || (serverConnectionHandlerID != m_homeId))
        return;

    switch(type)
    {
    case PLUGIN_CLIENT:
    {
        if (m_InfoId == id)
        {
//            TSLogging::Print("(TSInfoData::RequestUpdate) Requesting update (client).",serverConnectionHandlerID,LogLevel_DEBUG);
            ts3Functions.requestInfoUpdate(serverConnectionHandlerID, m_InfoType, m_InfoId);
        }
    }
        break;
    case PLUGIN_CHANNEL:
    {
        if (m_InfoId == id)
        {
//            TSLogging::Print("(TSInfoData::RequestUpdate) Requesting update (channel).",serverConnectionHandlerID,LogLevel_DEBUG);
            ts3Functions.requestInfoUpdate(serverConnectionHandlerID, m_InfoType, m_InfoId);
        }
    }
        break;
    case PLUGIN_SERVER:
    {
//        TSLogging::Print("(TSInfoData::RequestUpdate) Requesting update (server).",serverConnectionHandlerID,LogLevel_DEBUG);
        ts3Functions.requestInfoUpdate(serverConnectionHandlerID, m_InfoType, m_InfoId);
    }
        break;
    default:
        break;
    }
}

void TSInfoData::RequestSelfUpdate()
{
//    TSLogging::Print("RequestSelfUpdate()");
    if (m_homeId != 0)
    {
        unsigned int error;
        int status;
        if ((error = ts3Functions.getConnectionStatus(m_homeId,&status)) != ERROR_ok)
        {
            TSLogging::Error("(TSInfoData::RequestSelfUpdate)",NULL,error,false);
            return;
        }
        // Get My Id on this handler
        anyID myID;
        if((error = ts3Functions.getClientID(m_homeId,&myID)) != ERROR_ok)
        {
            TSLogging::Error("(TSInfoData::RequestSelfUpdate)",m_homeId,error);
            return;
        }
        if (myID == (anyID)m_InfoId)
            RequestUpdate(m_homeId,myID,PLUGIN_CLIENT);
    }
}

bool TSInfoData::Register(QObject *p, bool isRegister, int priority)
{
    InfoDataInterface *iInfoData = qobject_cast<InfoDataInterface *>(p);
    if (!iInfoData)
        return false;

    if (isRegister)
    {
        if (m_Callbacks.contains(QPointer<QObject>(p)))
            return true;

        if (m_Callbacks.size() < priority)
        {
            for (int i = m_Callbacks.size(); i<priority; ++i)
            {
                m_Callbacks.insert(i,0);
            }
        }
        else if (m_Callbacks.at(priority) != 0)
        {
            for (int i = priority+1; i>m_Callbacks.size(); ++i)
            {
                if (m_Callbacks.at(i) == 0)
                {
                    m_Callbacks.removeAt(i);
                    break;
                }
            }
        }
        m_Callbacks.insert(priority,QPointer<QObject>(p));
    }
    else
    {
        int rem;
        if ((rem = m_Callbacks.removeAll(QPointer<QObject>(p))) != 1)
        {
            TSLogging::Error(QString("Unexpected callback item amount: %1").arg(rem));
            return false;
        }
    }
    return true;
}

void TSInfoData::onInfoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data)
{
//    TSLogging::Print(QString("onInfoData %1 %2 %3 ").arg(serverConnectionHandlerID).arg(id).arg(type));
    // When disconnecting a server tab, an info update will be sent with this scHandlerID
    // That's rather not helpfull
    unsigned int error;
    int con_status;
    if ((error = ts3Functions.getConnectionStatus(serverConnectionHandlerID,&con_status)) != ERROR_ok)
    {
        TSLogging::Error("(TSInfoData::onInfoData)",serverConnectionHandlerID,error);
        return;
    }
    if (con_status == STATUS_DISCONNECTED)
        return;

    m_homeId = serverConnectionHandlerID;
    m_InfoType = type;
    m_InfoId = id;

    uint64 mine;
    if ((type == PLUGIN_CLIENT) || (type == PLUGIN_CHANNEL))
    {
        unsigned int error;
        // Get My Id on this handler
        anyID myID;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        {
            if (error != ERROR_not_connected)
                TSLogging::Error("(TSInfoData::onInfoData)",serverConnectionHandlerID,error);

            return;
        }
        if (type == PLUGIN_CLIENT)
        {
            mine = (uint64)myID;
        }
        else
        {
            // Get My channel on this handler
            uint64 channelID;
            if((error=ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
            {
                TSLogging::Error("(TSInfoData::onInfoData)",serverConnectionHandlerID,error);
                return;
            }
            mine = channelID;
        }
    }

    QString outstr;
    QTextStream data_stream(&outstr);
    for (int i = 0; i < m_Callbacks.size(); ++i)
    {
        if (m_Callbacks.at(i) != 0)
        {
            InfoDataInterface *iInfo = qobject_cast<InfoDataInterface *>(m_Callbacks.at(i));
            if(iInfo->onInfoDataChanged(serverConnectionHandlerID,id,type,mine,data_stream))
                data_stream << ".\n";
        }
    }
    if (outstr.isEmpty())
    {
//        TSLogging::Log("(TSInfoData::onInfoData): outstr is empty.");
        data = NULL;
    }
    else
    {
//        TSLogging::Log("(TSInfoData::onInfoData)" + outstr,LogLevel_DEBUG);
        *data = (char*)malloc(INFODATA_BUFSIZE * sizeof(char));
        if (outstr.size() > INFODATA_BUFSIZE-1)
        {
            TSLogging::Error("Infodata too long for teamspeak. Shortening.",true);
            outstr = outstr.trimmed();
            if (outstr.size() > INFODATA_BUFSIZE-1)
                outstr.truncate(INFODATA_BUFSIZE-1);
        }
        qstrncpy(*data,outstr.toLatin1().constData(),INFODATA_BUFSIZE-1);
    }
}
