#ifndef TS_SERVERGROUPS_H
#define TS_SERVERGROUPS_H

#include "teamspeak/public_definitions.h"

#include <QObject>
#include <QtCore>

class TSServerGroups : public QObject
{
    Q_OBJECT
public:
    explicit TSServerGroups(QObject *parent = 0);

    // forwards from plugin.cpp
    void onServerGroupListEvent(uint64 serverGroupID, const char* name, int type, int iconID, int saveDB);
    void onServerGroupListFinishedEvent();

    uint64 GetServerGroupId(QString name) const;
    QString GetServerGroupName(uint64 id) const;

signals:
    void serverGroupListUpdated(QMap<uint64,QString>);

public slots:

private:
    QMap<uint64,QString> serverGroups;
    bool isUpdating;
};

#endif // TS_SERVERGROUPS_H
