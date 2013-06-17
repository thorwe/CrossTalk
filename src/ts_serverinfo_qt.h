#ifndef TS_SERVERINFO_QT_H
#define TS_SERVERINFO_QT_H

#include "public_definitions.h"

#include "ts_servergroups.h"

#include <QObject>
#include <QtCore>

class TSServerInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint64 serverConnectionHandlerID READ getServerConnectionHandlerID)
    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(QString uniqueId READ getUniqueId)

public:
    explicit TSServerInfo(QObject *parent = 0, uint64 serverConnectionHandlerID = 0);
    
    uint64 getServerConnectionHandlerID() const;
    QString getName() const;
    QString getUniqueId() const;

    // forwards from plugin.cpp
    void onServerGroupListEvent(uint64 serverGroupID, const char* name, int type, int iconID, int saveDB);
    void onServerGroupListFinishedEvent();

    uint64 GetServerGroupId(QString name) const;
    QString GetServerGroupName(uint64 id) const;

signals:
    void serverGroupListUpdated(uint64 serverConnectionHandlerID, QMap<uint64,QString>);

public slots:

private:
    uint64 m_ServerConnectionHandlerID;

    QMap<uint64,QString> m_ServerGroups;
    bool m_isServerGroupsUpdating;
};

#endif // TS_SERVERINFO_QT_H
