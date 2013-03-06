#include "ts_infodata_qt.h"
#include "ts3_functions.h"
#include "plugin.h"

TSInfoData* TSInfoData::m_Instance = 0;

TSInfoData::TSInfoData(){}
TSInfoData::~TSInfoData(){}


PluginItemType TSInfoData::getInfoType() const
{
    return m_InfoType;
}

uint64 TSInfoData::getInfoId() const
{
    return m_InfoId;
}

void TSInfoData::setHomeId(uint64 serverConnectionHandlerID)
{
    if (serverConnectionHandlerID == m_homeId)
        return;
    m_homeId = serverConnectionHandlerID;
}

void TSInfoData::requestUpdate(uint64 serverConnectionHandlerID)
{
    if (serverConnectionHandlerID != m_homeId)
        return;

    if (m_InfoType != PLUGIN_SERVER)
        return;

    if (m_InfoType > 0 && m_InfoId > 0) ts3Functions.requestInfoUpdate(serverConnectionHandlerID, m_InfoType, m_InfoId);
}

void TSInfoData::requestUpdate(uint64 serverConnectionHandlerID, uint64 channelID)
{
    if (serverConnectionHandlerID != m_homeId)
        return;

    if ((m_InfoType != PLUGIN_CHANNEL) || (m_InfoId != channelID))
        return;

    if (m_InfoType > 0 && m_InfoId > 0) ts3Functions.requestInfoUpdate(serverConnectionHandlerID, m_InfoType, m_InfoId);
}

void TSInfoData::requestUpdate(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (serverConnectionHandlerID != m_homeId)
        return;

    if (m_InfoType != PLUGIN_CLIENT || (m_InfoId != (uint64)clientID))
        return;

    if (m_InfoType > 0 && m_InfoId > 0) ts3Functions.requestInfoUpdate(serverConnectionHandlerID, m_InfoType, m_InfoId);
}

void TSInfoData::SetInfo(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type, QString ModuleName, QString info)
{
    switch(type)
    {
     case PLUGIN_SERVER:
    {
        m_Map_PLUGIN_SERVER.insert(qMakePair(serverConnectionHandlerID,ModuleName),info);
    }
        break;

     case PLUGIN_CHANNEL:
    {
        QPair<uint64,uint64> key = qMakePair(serverConnectionHandlerID,id);
        QPair<QString,QString> info_p = qMakePair(ModuleName,info);
        if (m_Map_PLUGIN_CHANNEL.contains(key))
        {
//            QList<QString,QString> m_Map_PLUGIN_CHANNEL.values(key);
            QMutableMapIterator<QPair<uint64,uint64>, QPair<QString,QString> > i (m_Map_PLUGIN_CHANNEL);
            while (i.hasNext())
            {
                i.next();
                if ((i.key().first == serverConnectionHandlerID) && (i.key().second == id) && (i.value().first == ModuleName))
                {
                    i.remove();
                    break;
                }
            }
        }
        m_Map_PLUGIN_CHANNEL.insert(key,info_p);
    }
        break;

     case PLUGIN_CLIENT:
    {
        QPair<uint64,anyID> key = qMakePair(serverConnectionHandlerID,(anyID)id);
        QPair<QString,QString> info_p = qMakePair(ModuleName,info);
        if (m_Map_PLUGIN_CLIENT.contains(key))
        {
            QMutableMapIterator<QPair<uint64,anyID>, QPair<QString,QString> > i (m_Map_PLUGIN_CLIENT);
            while (i.hasNext())
            {
                i.next();
                if ((i.key().first == serverConnectionHandlerID) && (i.key().second == (anyID)id) && (i.value().first == ModuleName))
                {
                    i.remove();
                    break;
                }
            }
        }
        m_Map_PLUGIN_CLIENT.insert(key,info_p);
    }
        break;

     default:
        break;
    }
    ts3Functions.requestInfoUpdate(serverConnectionHandlerID,type,id);
}
