#ifndef TS_HELPERS_QT_H
#define TS_HELPERS_QT_H

#ifndef PATH_BUFSIZE
#define PATH_BUFSIZE 512
#endif

#include <QString>
#include <QWidget>
#include <QDir>

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
    unsigned int GetSubChannels(uint64 serverConnectionHandlerID, uint64 channelId, QVector<uint64> *result);

    /* credits @ Jules Blok (jules@aerix.nl),
     * see
     * https://github.com/Armada651/g-key
     * for the original std approach
     */
    unsigned int GetServerHandler(QString name, uint64 *result);
    uint64 GetActiveServerConnectionHandlerID();
    unsigned int GetActiveServerRelative(uint64 serverConnectionHandlerID, bool next, uint64 *result);
    int SetActiveServer(uint64 serverConnectionHandlerID);
    int SetActiveServerRelative(uint64 serverConnectionHandlerID, bool next);
    inline int SetNextActiveServer(uint64 serverConnectionHandlerID) { return SetActiveServerRelative(serverConnectionHandlerID, true); }
    inline int SetPrevActiveServer(uint64 serverConnectionHandlerID) { return SetActiveServerRelative(serverConnectionHandlerID, false); }
    unsigned int SetWhisperList(uint64 serverConnectionHandlerID, GroupWhisperType groupWhisperType, GroupWhisperTargetMode groupWhisperTargetMode, QString returnCode = QString::null, uint64 arg = (uint64)NULL);

    unsigned int GetDefaultProfile(PluginGuiProfile profile, QString &result);

    QWidget* GetMainWindow();

    unsigned int GetClientServerGroups(uint64 serverConnectionHandlerID, anyID clientID, QSet<uint64> *result);
    unsigned int GetClientSelfServerGroups(uint64 serverConnectionHandlerID, QSet<uint64> *result);
    unsigned int GetClientChannelGroup(uint64 serverConnectionHandlerID, uint64* result, anyID clientId = (anyID)NULL);

    bool GetCreatePluginConfigFolder(QDir &result);

    QString GetChannelVariableAsQString(uint64 serverConnectionHandlerID, uint64 channelID, ChannelProperties property);
    QString GetChannelPath(uint64 serverConnectionHandlerID, uint64 channel_id);
    uint64 GetChannelIDFromPath(uint64 serverConnectionHandlerID, QString path_q);
}

#endif // TS_HELPERS_QT_H
