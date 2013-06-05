#include "ts_servergroups.h"

TSServerGroups::TSServerGroups(QObject *parent) :
    QObject(parent),
    isUpdating(false)
{
}

void TSServerGroups::onServerGroupListEvent(uint64 serverGroupID, const char *name, int type, int iconID, int saveDB)
{
    Q_UNUSED(type);
    Q_UNUSED(iconID);
    Q_UNUSED(saveDB);

    if (!isUpdating)
        serverGroups.clear();

    serverGroups.insert(serverGroupID,name);
    isUpdating = true;
}

void TSServerGroups::onServerGroupListFinishedEvent()
{
    isUpdating = false;
    emit serverGroupListUpdated(serverGroups);
}

uint64 TSServerGroups::GetServerGroupId(QString name) const
{
    return (serverGroups.key(name,(uint64)NULL));
}

QString TSServerGroups::GetServerGroupName(uint64 id) const
{
    if (serverGroups.contains(id))
        return serverGroups.value(id);

    return QString::null;
}
