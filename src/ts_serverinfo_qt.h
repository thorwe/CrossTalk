#pragma once

#include "teamspeak/public_definitions.h"

#include "ts_servergroups.h"

#include <QObject>
#include <QtCore>

class TSServerInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint64 serverConnectionHandlerID READ getServerConnectionHandlerID)
    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(QString uniqueId READ getUniqueId)
    Q_PROPERTY(uint64 defaultChannelGroup READ getDefaultChannelGroup)

public:
    explicit TSServerInfo(QObject *parent = 0, uint64 serverConnectionHandlerID = 0);
    
    uint64 getServerConnectionHandlerID() const;
    QString getName() const;
    QString getUniqueId() const;
    uint64 getDefaultChannelGroup() const;

    // forwards from plugin.cpp
    void onServerGroupListEvent(uint64 serverGroupID, const char* name, int type, int iconID, int saveDB);
    void onServerGroupListFinishedEvent();
    void onChannelGroupListEvent(uint64 channelGroupID, const char* name, int type, int iconID, int saveDB);
    void onChannelGroupListFinishedEvent();

    uint64 GetServerGroupId(QString name) const;
    QString GetServerGroupName(uint64 id) const;
    uint64 GetChannelGroupId(QString name) const;
    QString GetChannelGroupName(uint64 id) const;

signals:
    void serverGroupListUpdated(uint64 serverConnectionHandlerID, QMap<uint64,QString>);
    void channelGroupListUpdated(uint64 serverConnectionHandlerID, QMap<uint64,QString>);

public slots:

private:
    uint64 m_ServerConnectionHandlerID;

    QMap<uint64,QString> m_ServerGroups;
    bool m_is_server_groups_updating = false;
    QMap<uint64,QString> m_ChannelGroups;
    bool m_is_channel_groups_updating = false;
};
