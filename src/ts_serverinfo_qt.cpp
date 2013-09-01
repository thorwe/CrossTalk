#include "ts_serverinfo_qt.h"

#include "public_errors.h"
#include "public_rare_definitions.h"

#include "ts_missing_definitions.h"

#include "ts_helpers_qt.h"
#include "ts_logging_qt.h"

TSServerInfo::TSServerInfo(QObject *parent, uint64 serverConnectionHandlerID) :
    QObject(parent),
    m_isServerGroupsUpdating(false),
    m_isChannelGroupsUpdating(false)
{
    m_ServerConnectionHandlerID = serverConnectionHandlerID;
}

uint64 TSServerInfo::getServerConnectionHandlerID() const
{
    return m_ServerConnectionHandlerID;
}

QString TSServerInfo::getName() const
{
    unsigned int error;
    char* s_name;
    if ((error = ts3Functions.getServerVariableAsString(m_ServerConnectionHandlerID, VIRTUALSERVER_NAME, &s_name)) != ERROR_ok)
    {
        TSLogging::Error("(TSServerInfo::getName())",NULL,error,true);
        return QString::null;
    }
    QString name = QString::fromUtf8(s_name);
    ts3Functions.freeMemory(s_name);
    return name;
}

QString TSServerInfo::getUniqueId() const
{
    unsigned int error;
    char* s_val;
    if ((error = ts3Functions.getServerVariableAsString(m_ServerConnectionHandlerID, VIRTUALSERVER_UNIQUE_IDENTIFIER, &s_val)) != ERROR_ok)
    {
        TSLogging::Error("(TSServerInfo::getUniqueId())",NULL,error,true);
        return QString::null;
    }
    QString val = QString::fromUtf8(s_val);
    ts3Functions.freeMemory(s_val);
    return val;
}

uint64 TSServerInfo::getDefaultChannelGroup() const
{
    unsigned int error;
    uint64 result;
    if ((error = ts3Functions.getServerVariableAsUInt64(m_ServerConnectionHandlerID,VIRTUALSERVER_DEFAULT_CHANNEL_GROUP, &result)) != ERROR_ok)
    {
        TSLogging::Error("Could not get default channel group", m_ServerConnectionHandlerID, error, true);
        return (uint64)NULL;
    }
//    TSLogging::Print(QString("Default Channel group: %1 %2").arg(result).arg(GetChannelGroupName(result)));
    return result;
}

uint64 TSServerInfo::GetServerGroupId(QString name) const
{
    return (m_ServerGroups.key(name,(uint64)NULL));
}

QString TSServerInfo::GetServerGroupName(uint64 id) const
{
    if (m_ServerGroups.contains(id))
        return m_ServerGroups.value(id);

    return QString::null;
}

uint64 TSServerInfo::GetChannelGroupId(QString name) const
{
    QList<uint64> list = m_ChannelGroups.keys(name);
    int elementCount = list.size();
    if (elementCount == 0)
        return (uint64)NULL;
    else if (elementCount == 1)
        return list.at(0);
    else
    {
        uint64 highest = 0;
        for (int i = 0; i < elementCount; ++i)
        {
            uint64 e_key = list.at(i);
            if (e_key>highest)
                highest = e_key;
        }
        return highest;
    }

    //return (m_ChannelGroups.key(name,(uint64)NULL));
}

QString TSServerInfo::GetChannelGroupName(uint64 id) const
{
    if (m_ChannelGroups.contains(id))
        return m_ChannelGroups.value(id);

    return QString::null;
}

void TSServerInfo::onServerGroupListEvent(uint64 serverGroupID, const char *name, int type, int iconID, int saveDB)
{
    Q_UNUSED(type);
    Q_UNUSED(iconID);
    Q_UNUSED(saveDB);

    if (!m_isServerGroupsUpdating)
        m_ServerGroups.clear();

    m_ServerGroups.insert(serverGroupID,name);
    m_isServerGroupsUpdating = true;
}

void TSServerInfo::onServerGroupListFinishedEvent()
{
    m_isServerGroupsUpdating = false;
//    QString debout = "ServerGroups: ";
//    QMapIterator<uint64,QString> i(m_ServerGroups);
//    while(i.hasNext())
//    {
//        i.next();
//        debout.append(QString("%1:%2;").arg(i.key()).arg(i.value()));
//    }
//    TSLogging::Print(debout);

    emit serverGroupListUpdated(m_ServerConnectionHandlerID, m_ServerGroups);
}

void TSServerInfo::onChannelGroupListEvent(uint64 channelGroupID, const char *name, int type, int iconID, int saveDB)
{
    if (type != PermGroupDBTypeRegular) // For now discard templates and Server Queries
        return;

    Q_UNUSED(iconID);
    Q_UNUSED(saveDB);

    if (!m_isChannelGroupsUpdating)
        m_ChannelGroups.clear();

    m_ChannelGroups.insert(channelGroupID,name);
    m_isChannelGroupsUpdating = true;
}

void TSServerInfo::onChannelGroupListFinishedEvent()
{
    m_isChannelGroupsUpdating = false;
    emit channelGroupListUpdated(m_ServerConnectionHandlerID, m_ChannelGroups);
}
