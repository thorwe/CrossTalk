#ifndef TS_HELPERS_QT_H
#define TS_HELPERS_QT_H

#ifndef PATH_BUFSIZE
#define PATH_BUFSIZE 512
#endif

#include <QString>
#include <QWidget>

#include "public_definitions.h"
#include "plugin_definitions.h"

namespace TSHelpers
{
    QString GetConfigPath();
    QString GetFullConfigPath();

    QString GetLanguage();

    int IsClientQuery(uint64 serverConnectionHandlerID, anyID clientID);
    unsigned int GetClientUID(uint64 serverConnectionHandlerID, anyID clientID, QString &result);
    unsigned int GetTalkStatus(uint64 serverConnectionHandlerID, anyID clientID, int &status, int &isWhispering);
    unsigned int GetSubChannels(uint64 serverConnectionHandlerID, uint64 channelId, QList<uint64> *result);

    unsigned int GetServerHandler(QString name, uint64 *result);
    uint64 GetActiveServerConnectionHandlerID();
    unsigned int GetActiveServerRelative(uint64 serverConnectionHandlerID, bool next, uint64 *result);
    int SetActiveServer(uint64 serverConnectionHandlerID);
    int SetActiveServerRelative(uint64 serverConnectionHandlerID, bool next);
    inline int SetNextActiveServer(uint64 serverConnectionHandlerID) { return SetActiveServerRelative(serverConnectionHandlerID, true); }
    inline int SetPrevActiveServer(uint64 serverConnectionHandlerID) { return SetActiveServerRelative(serverConnectionHandlerID, false); }

    unsigned int SetWhisperList(uint64 serverConnectionHandlerID, GroupWhisperType groupWhisperType, GroupWhisperTargetMode groupWhisperTargetMode);

    unsigned int GetDefaultProfile(PluginGuiProfile profile, QString &result);

    QWidget* GetMainWindow();
}

#endif // TS_HELPERS_QT_H
