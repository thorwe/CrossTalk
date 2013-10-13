#include "ts_serversinfo.h"

#include "ts_logging_qt.h"

TSServersInfo* TSServersInfo::m_Instance = 0;
TSServersInfo::TSServersInfo()
{
}
TSServersInfo::~TSServersInfo(){}

TSServerInfo* TSServersInfo::GetServerInfo(uint64 serverConnectionHandlerID)
{
//    return (m_serverInfoMap.contains(serverConnectionHandlerID))?m_serverInfoMap.value(serverConnectionHandlerID):(const QPointer<TSServerInfo>)NULL;
    return m_serverInfoMap.value(serverConnectionHandlerID,NULL);
}

void TSServersInfo::onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    if (newStatus == STATUS_DISCONNECTED)
    {
        if (m_serverInfoMap.contains(serverConnectionHandlerID))
        {
            QPointer<TSServerInfo> p_ServerInfo = m_serverInfoMap.value(serverConnectionHandlerID);
            if (p_ServerInfo)
                p_ServerInfo.data()->deleteLater();

            m_serverInfoMap.remove(serverConnectionHandlerID);
        }
    }

    emit connectStatusChanged(serverConnectionHandlerID, newStatus, errorNumber);
}

void TSServersInfo::onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char *name, int type, int iconID, int saveDB)
{
    TSServerInfo* tsServerInfo = _GetServerInfo(serverConnectionHandlerID,true);
    if (tsServerInfo)
        tsServerInfo->onServerGroupListEvent(serverGroupID,name,type,iconID,saveDB);
}

void TSServersInfo::onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID)
{
    TSServerInfo* tsServerInfo = _GetServerInfo(serverConnectionHandlerID,false);
    if (tsServerInfo)
        tsServerInfo->onServerGroupListFinishedEvent();
}

void TSServersInfo::onChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char *name, int type, int iconID, int saveDB)
{
    TSServerInfo* tsServerInfo = _GetServerInfo(serverConnectionHandlerID,true);
    if (tsServerInfo)
        tsServerInfo->onChannelGroupListEvent(channelGroupID,name,type,iconID,saveDB);
}

void TSServersInfo::onChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID)
{
    TSServerInfo* tsServerInfo = _GetServerInfo(serverConnectionHandlerID,false);
    if (tsServerInfo)
        tsServerInfo->onChannelGroupListFinishedEvent();
}


TSServerInfo* TSServersInfo::_GetServerInfo(uint64 serverConnectionHandlerID, bool createOnNotExist)
{
    if (m_serverInfoMap.contains(serverConnectionHandlerID))
    {
        QPointer<TSServerInfo> p_ServerInfo = m_serverInfoMap.value(serverConnectionHandlerID);
        if (!p_ServerInfo)
        {
            TSLogging::Error("(TSServersInfo::_GetServerInfo): Invalid pointer.");
            return (TSServerInfo*)NULL;
        }
        return p_ServerInfo.data();
    }
    else
    {
        if (createOnNotExist)
        {
            TSServerInfo* tsServerInfo = new TSServerInfo(this,serverConnectionHandlerID);
            m_serverInfoMap.insert(serverConnectionHandlerID, tsServerInfo);
            this->connect(tsServerInfo,SIGNAL(serverGroupListUpdated(uint64,QMap<uint64,QString>)),SIGNAL(serverGroupListUpdated(uint64,QMap<uint64,QString>)),Qt::UniqueConnection);
            return tsServerInfo;
        }
        else
        {
            TSLogging::Error("(TSServersInfo::_GetServerInfo): serverConnectionHandlerID not found.");
            return (TSServerInfo*)NULL;
        }
    }
}
